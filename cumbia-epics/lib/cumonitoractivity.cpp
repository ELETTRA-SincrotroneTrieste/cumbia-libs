#include "cumonitor.h"
#include "cumonitoractivity.h"
#include "epsource.h"
#include "cuepics-world.h"
#include <cadef.h>
#include <cumacros.h>
#include <stdlib.h>
#include <string.h>
#include <cuactivitymanager.h>

class CuMonitorActivityPrivate
{
public:
    CuEpCAService *device_srvc;
    int repeat, errCnt;
    std::string message;
    pthread_t my_thread_id, other_thread_id;
    CuVariant argins;
    CuData point_info;
    bool exiting;
};

CuMonitorActivity::CuMonitorActivity(const CuData &token,
                                     CuEpCAService *ep_s,
                                     const CuVariant & argins)
    : CuContinuousActivity(token)
{
    d = new CuMonitorActivityPrivate;
    setFlag(CuActivity::CuAUnregisterAfterExec, false);
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->device_srvc = ep_s;
    d->repeat = 1000;
    d->errCnt = 0;
    d->other_thread_id = pthread_self();
    d->argins = argins;
    d->exiting = false;

    int period = 1000;
    if(token.containsKey("period"))
        period = token["period"].toInt();
    d->repeat = period;
    setInterval(period);
}

CuMonitorActivity::~CuMonitorActivity()
{
    delete d;
}

void CuMonitorActivity::setArgins(const CuVariant &argins)
{
    d->argins = argins;
}

bool CuMonitorActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

void CuMonitorActivity::init()
{
    int caTimeout = DEFAULT_TIMEOUT;
    int reqElems = 1;
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    int nPvs = 1;
    CuPV* pv;                    /* PV structure */
    int result = ca_context_create(ca_enable_preemptive_callback);

    /* install my exception event handler */

    ca_add_exception_event (exception_handler_cb, this);

    if (result != ECA_NORMAL) {
        char msg[256];
        snprintf(msg, 256, "CA error %s occurred while trying "
                           "to start channel access.\n", ca_message(result));
        m_setTokenError(msg, tk);
    }
    else
    {
        /* Allocate PV structure */
        pv = (CuPV *) malloc (sizeof(CuPV));
        if (!pv) {
            m_setTokenError("Memory allocation for channel structures failed.", tk);
        }
        else {
            /* Connect channels */
            memset(pv[0].name, 0, 256);
            strncpy(pv[0].name, tk["src"].toString().c_str(), 255);
            pv[0].onceConnected = 0;
            pv[0].monitor_activity = this;

            /* Create CA connections */
            int returncode = create_pvs(pv, nPvs, connection_handler_cb);
            if ( returncode )
                m_setTokenError(("Error creating pv " + tk["src"].toString()).c_str(), tk);
        }
    }
    publishResult(tk);
   d->repeat = -1;
}

void CuMonitorActivity::execute()
{
    assert(d->my_thread_id == pthread_self());
}

void CuMonitorActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    d->exiting = true;
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    at["msg"] = "EXITED";
    at["mode"] = "POLLED";
    CuEpicsWorld utils;
    utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */

    /* Shut down Channel Access */
    printf("\e[1;31monExit calling >>>>>>>>> ca_context_destroy <<<<<<<<<<<< \e[0m\n");
    ca_context_destroy();

    at["exit"] = true;
    publishResult(at);
}

void CuMonitorActivity::m_setTokenError(const char *msg, CuData &d)
{
    d["msg"] = std::string(msg);
    d["err"] = true;
}

/*+**************************************************************************
 *
 * Function:	event_handler
 *
 * Description:	CA event_handler for request type callback
 * 		Prints the event data
 *
 * Arg(s) In:	args  -  event handler args (see CA manual)
 *
 **************************************************************************-*/

void CuMonitorActivity::event_handler_cb (evargs args)
{
    CuPV *ppv = ( CuPV * ) ca_puser ( args.chid );
    CuMonitorActivity* cu_mona = ppv->monitor_activity;
    cu_mona->event_handler(args);
}

void CuMonitorActivity::connection_handler_cb ( struct connection_handler_args args )
{
    CuPV *ppv = ( CuPV * ) ca_puser ( args.chid );
    CuMonitorActivity* cu_mona = ppv->monitor_activity;
    cu_mona->connection_handler(args);
}

void CuMonitorActivity::exception_handler_cb(exception_handler_args excargs)
{
    CuMonitorActivity *cu_mona = static_cast<CuMonitorActivity *>(excargs.usr);
    cu_mona->exception_handler(excargs);
}

void CuMonitorActivity::event_handler(evargs args)
{
    CuPV* _pv = (CuPV *) args.usr;
    CuData d = getToken();
    CuEpicsWorld utils;
    cuprintf("CuMonitorActivity.event_handler: in thread: 0x%lx pv %s activity %p\n", pthread_self(), _pv->name);
    utils.fillThreadInfo(d, this); /* put thread and activity info */

    _pv->status = args.status;
    if (args.status == ECA_NORMAL)
    {
        _pv->dbrType = args.type;
        _pv->nElems = args.count;
        _pv->value = (void *) args.dbr;    /* casting away const */

        utils.extractData(_pv, d);

        d["msg"] = utils.getLastMessage();
        d["err"] = utils.error();
        _pv->value = NULL;
    }
    else
    {
        d["msg"] = "CuMonitorActivity.event_handler: status error (" + std::to_string(args.status) + ")";
        d["err"] = true;
    }
    publishResult(d);
}

void CuMonitorActivity::connection_handler(connection_handler_args args)
{
    int nConn = 0;
    CuData d = getToken();
    d["type"] = "connection";

    unsigned long eventMask = DBE_VALUE | DBE_ALARM;
    int floatAsString = 0;
    CuPV *ppv = ( CuPV * ) ca_puser ( args.chid );

    if ( args.op == CA_OP_CONN_UP ) {
        nConn++;
        if (!ppv->onceConnected) {
            ppv->onceConnected = 1;
            /* Set up pv structure */
            /* ------------------- */

            /* Get natural type and array count */
            ppv->dbfType = ca_field_type(ppv->ch_id);
            ppv->dbrType = dbf_type_to_DBR_TIME(ppv->dbfType); /* Use native type */
            if (dbr_type_is_ENUM(ppv->dbrType))                /* Enums honour -n option */
            {
                if (enumAsNr) ppv->dbrType = DBR_TIME_INT;
                else          ppv->dbrType = DBR_TIME_STRING;
            }
            else if (floatAsString &&
                     (dbr_type_is_FLOAT(ppv->dbrType) || dbr_type_is_DOUBLE(ppv->dbrType)))
            {
                ppv->dbrType = DBR_TIME_STRING;
            }
            /* Set request count */
            ppv->nElems   = ca_element_count(ppv->ch_id);
            ppv->reqElems = 1;

            /* Issue CA request */
            /* ---------------- */
            /* install monitor once with first connect */
            ppv->status = ca_create_subscription(ppv->dbrType,
                                                 ppv->reqElems, ppv->ch_id,
                                                 eventMask,
                                                 CuMonitorActivity::event_handler_cb,
                                                 (void*)ppv, NULL);

            /* subscribe to property change */
            ppv->ctrl_status = ca_create_subscription(dbf_type_to_DBR_CTRL(ppv->dbfType),
                                                 ppv->reqElems, ppv->ch_id,
                                                 DBE_PROPERTY,
                                                 CuMonitorActivity::event_handler_cb,
                                                 (void*)ppv, NULL);
        }
    }
    else if ( args.op == CA_OP_CONN_DOWN ) {
        nConn--;
        ppv->status = ECA_DISCONN;
    }

    d["status"] = ppv->status;
    publishResult(d);
}

void CuMonitorActivity::exception_handler(exception_handler_args excargs)
{
    CuData d = getToken();
    d["type"] = "exception";
    d["err"] = true;
    CuEpicsWorld ew;
    std::string msg = ew.extractException(excargs, d);
    d["msg"] = "error: \"" + d["src"].toString() + "\":\n" + msg;
    ca_signal(excargs.stat, msg.c_str());
    publishResult(d);
}



void CuMonitorActivity::event(CuActivityEvent *e)
{
    assert(d->my_thread_id == pthread_self());
    CuContinuousActivity::event(e);
}

int CuMonitorActivity::getType() const
{
    return CuMonitorActivityType;
}

int CuMonitorActivity::repeat() const
{
    assert(d->my_thread_id == pthread_self());
    return d->repeat;
}
