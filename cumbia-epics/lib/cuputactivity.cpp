#include "cuputactivity.h"
#include "cuepactioni.h"
#include "cuepics-world.h"
#include "cuepcaservice.h"
#include <cadef.h>
#include <cumacros.h>
#include <cudata.h>
#include <cudatatypes_ex.h>
#include <stdlib.h>
#include <string.h>

/* Valid EPICS string */
typedef char EpicsStr[MAX_STRING_SIZE];

class CuPutActivityPrivate
{
public:
    const double default_timeout = 1.0;
    CuEpCAService *epics_service;
    std::string msg;
    bool err;
    pthread_t my_thread_id, other_thread_id;
    CuData point_info; /* attribute or command info */
};

CuPutActivity::CuPutActivity(const CuData &token,
                             CuEpCAService *df)
    : CuIsolatedActivity(token)
{
    d = new CuPutActivityPrivate;
    d->epics_service = df;
    d->err = false;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->other_thread_id = pthread_self();
}

CuPutActivity::~CuPutActivity()
{
    pdelete("~CuWriteActivity %p", this);
    delete d;
}

void CuPutActivity::exception_handler_cb(exception_handler_args excargs)
{
    CuPutActivity *cu_puta = static_cast<CuPutActivity *>(excargs.usr);
    cu_puta->exception_handler(excargs);
}

void CuPutActivity::exception_handler(exception_handler_args excargs)
{
    CuData d = getToken();
    d[CuDType::Type] = "exception";
    d[CuDType::Err] = true;
    CuEpicsWorld ew;
    std::string msg = ew.extractException(excargs, d);
    d[CuDType::Message] = "error: \"" + d[CuXDType::Pv].toString() + "\":\n" + msg;
    ca_signal(excargs.stat, msg.c_str());
    publishResult(d);
}

void CuPutActivity::event(CuActivityEvent *e)
{

}

bool CuPutActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token[CuDType::Src] == mytok[CuDType::Src] && mytok[CuDType::Activity] == token[CuDType::Activity];
}

void CuPutActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();

}

void CuPutActivity::execute()
{
    d->err = false;
    char msg[256];
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    CuVariant wval = at[CuXDType::InputValue];
    /* get configuration and then write */
    printf("\e[1;32mCuPutActivity execute enter...[%s]\e[0m\n", at.toString().c_str());

    printf("\e[1;36mwould write %s into %s [%s|%s]\e[0m\n", at[CuXDType::InputValue].toString().c_str(), at[CuDType::Src].toString().c_str(),
            wval.dataFormatStr(wval.getFormat()).c_str(), wval.dataTypeStr(wval.getType()).c_str());

    CuPV* pvs;
    EpicsStr *sbuf;
    chtype dbrType = DBR_STRING;
    size_t count;
    int nPvs = 1;
    double caTimeout = d->default_timeout; // Wait time, specifies CA timeout, default is  d->default_timeout
 //   epicsEventId epId = epicsEventCreate(epicsEventEmpty);  /* Create empty EPICS event (semaphore) */

    if(at.containsKey("ca_timeout") && at["ca_timeout"].toDouble() >= 0)
        caTimeout = at["ca_timeout"].toDouble();

    // CA result
    int result  = ca_context_create(ca_enable_preemptive_callback);

    /* install my exception event handler */
    ca_add_exception_event (exception_handler_cb, this);

    if (result != ECA_NORMAL) {
        snprintf(msg, 256, "CA error %s occurred while trying to start channel access.", ca_message(result));
        m_setTokenError(msg, at);
    }
    else {
        pvs =  (CuPV *) malloc (sizeof(CuPV));
        if (!pvs) {
            snprintf(msg, 256, "CuPutActivity.execute: memory allocation for channel structure failed");
            m_setTokenError(msg, at);
        }
        else {
            memset(pvs[0].name, 0, 256);
            strncpy(pvs[0].name, at[CuXDType::Pv].toString().c_str(), 255);
            result =  CuEpicsWorld().connect_pvs(pvs, nPvs);
            if (result) {
                snprintf(msg, 256, "CuPutActivity.execute: connect_pvs failed for \"%s\"", pvs[0].name);
                m_setTokenError(msg, at);
                ca_context_destroy();
            }
            else { // connect_pvs OK
                if(ca_field_type(pvs[0].ch_id) == DBR_ENUM) {
                    printf("\e[1;31m\"%s\" is a DBR_ENUM type\e[0m -- unsupported yet --\n", pvs[0].name);
                }
                else { // if(ca_field_type(pvs[0].chid) == DBR_STRING){
                    dbrType = DBR_STRING;
                    printf("CuPutActivity.execute: %s ca_field_type is DBR_STRING\n", pvs[0].name);
                    count = wval.getSize();
                    sbuf = (EpicsStr *) calloc(count, sizeof(EpicsStr));
                    if(count == 1) {
                        std::string str = wval.toString();
                        strncpy(sbuf[0], str.c_str(), sizeof(EpicsStr));
                    }
                    else {
                        std::vector<std::string> vs = wval.toStringVector();
                        for(size_t i = 0; i < vs.size(); i++) {
                           const std::string & vi = vs[i];
                           strncpy(sbuf[i], vi.c_str(), sizeof(EpicsStr));
                        }
                    }


                    // we are already running in a separate thread. use ca_array_put, not ca_array_put_callback
                    result = ca_array_put(dbrType, count, pvs[0].ch_id, sbuf);

                    if (result == ECA_TIMEOUT) {
                        snprintf(msg, 256, "CuPutActivity.execute: operation on %s [value: %s] unsuccessful: data was not written",
                                 at[CuXDType::Pv].toString().c_str(), at[CuXDType::InputValue].toString().c_str());
                        m_setTokenError(msg, at);
                     }
                    result = ca_pend_io(caTimeout);
                    if (result == ECA_TIMEOUT) {
                        snprintf(msg, 256, "CuPutActivity.execute: operation on %s [value: %s] timed out: data was not written",
                                 at[CuXDType::Pv].toString().c_str(), at[CuXDType::InputValue].toString().c_str());
                        m_setTokenError(msg, at);
                     }

                    delete sbuf;
                }

            }
        }
        ca_context_destroy();

    } // ca_context_create successful

    publishResult(at);
}

void CuPutActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    at[CuDType::Message] = d->msg;
    at[CuDType::Mode] = "WRITE";
    at[CuDType::Err] = d->err;
    CuEpicsWorld utils;
    utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
    /* remove reference to this activity ? */

    cuprintf("\e[1;31mrefcnt = %d called actionRemove for device %s att %s\e[0m\n",
             refcnt, at[CuXDType::Device].toString().c_str(), at[CuDType::Src].toString().c_str());
    if(refcnt == 0)
    {
        // d->epics_service->remove...
    }
    at[CuDType::Exit] = true;
    publishResult(at);
}

void CuPutActivity::m_setTokenError(const char *msg, CuData &dat)
{
    dat[CuDType::Message] = std::string(msg);
    dat[CuDType::Err] = true;
    d->err = true;
}
