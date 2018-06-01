#include "cupollingactivity.h"
#include "cudevicefactoryservice.h"
#include "tdevice.h"
#include "tsource.h"
#include "cutango-world.h"
#include "cutangoactioni.h"
#include <tango.h>
#include <cumacros.h>
#include <vector>
#include <map>

class ActionData {
public:
    ActionData(const TSource& ts, CuTangoActionI *a_ptr) {
        tsrc = ts;
        action = a_ptr;
    }

    TSource tsrc;
    CuTangoActionI *action;
};

class CmdData {
public:
    CmdData() {
        is_empty = true;
    }

    CmdData(const CuData& cmd_info, const Tango::DeviceData& dd_in, const std::vector<std::string> args) {
        cmdinfo = cmd_info;
        din = dd_in;
        is_empty = false;
        argins = args;
    }

    CuData cmdinfo;
    Tango::DeviceData din;
    bool is_empty;
    std::vector<std::string> argins;
};

CuAddPollActionEvent::CuAddPollActionEvent(const TSource &t, CuTangoActionI* a)
{
    tsource = t;
    action = a;
}

CuActivityEvent::Type CuAddPollActionEvent::getType() const
{
    return static_cast<CuActivityEvent::Type> (AddPollAction);
}

CuRemovePollActionEvent::CuRemovePollActionEvent(const TSource &t)
{
    tsource = t;
}

CuActivityEvent::Type CuRemovePollActionEvent::getType() const
{
    return static_cast<CuActivityEvent::Type> (RemovePollAction);
}

/* @private */
class CuPollingActivityPrivate
{
public:
    CuDeviceFactoryService *device_srvc;
    TDevice *tdev;
    int repeat, errCnt;
    std::string message;
    pthread_t my_thread_id, other_thread_id;
    CuVariant argins;
    CuData point_info;
    bool exiting;
    std::multimap<const std::string, const ActionData > actions_map;
    // cache for tango command_inout argins
    // multimap because argins may differ
    std::map<const std::string, CmdData> din_cache;
    CmdData emptyCmdData;
};

/*! \brief the class constructor that sets up a Tango polling activity
 *
 * @param token a CuData that will describe this activity
 * @param df a pointer to a CuDeviceFactoryService that is used by init and onExit to create/obtain and
 *        later get rid of a Tango device, respectively
 * @param argins input arguments that can optionally be passed to Tango commands as argins
 *
 * \par notes
 * \li the default polling period is 1000 milliseconds
 * \li if the "period" key is set on the token, then it is converted to int and it will be used
 *     to set up the polling period
 * \li CuADeleteOnExit is active.
 * \li CuAUnregisterAfterExec is disabled because if the Tango device is not defined into the database
 *     the poller is not started and the activity is suspended (repeat will return -1).
 */
CuPollingActivity::CuPollingActivity(const CuData &token,
                                     CuDeviceFactoryService *df,
                                     const CuVariant & argins)
    : CuContinuousActivity(token)
{
    d = new CuPollingActivityPrivate;
    d->device_srvc = df;
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
    //  flag CuActivity::CuADeleteOnExit is true
    setFlag(CuActivity::CuAUnregisterAfterExec, false);
}

/*! \brief the class destructor
 *
 * deletes the internal data
 */
CuPollingActivity::~CuPollingActivity()
{
    delete d;
}

/*! \brief set the input arguments if the poller is used to "read" Tango commands
 *         and they require argins
 *
 * @param argins input arguments to supply to Tango commands, if the reader's source is
 *        a command
 *
 */
void CuPollingActivity::setArgins(const CuVariant &argins)
{
    d->argins = argins;
}

size_t CuPollingActivity::actionsCount() const
{
    size_t cnt = 0;
    std::multimap <const std::string, const ActionData>::iterator it;
    for(it = d->actions_map.begin(); it != d->actions_map.end(); ++it)
        cnt += d->actions_map.count(it->first);
    return cnt;
}

size_t CuPollingActivity::srcCount() const
{
    return d->actions_map.size();
}

/** \brief returns true if the passed token's *device* *activity* and *period* values matche this activity token's
 *         *device* and *activity* and *period* values.
 *
 * @param token a CuData containg key/value pairs of another activity's token
 * @return true if the input token's "period" *device* and *activity* values match this token's
 * "period" *device* and *activity* values
 *
 * Two CuPollingActivity match if they refer to the same device and have the same period.
 *
 * @implements CuActivity::matches
 */
bool CuPollingActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["device"] == mytok["device"] && mytok["activity"] == token["activity"]
            && token["period"] == mytok["period"];
}

/*! \brief the implementation of the CuActivity::init hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \par Notes
 * \li in cumbia-tango, threads are grouped by device
 * \li CuDeviceFactoryService::getDevice is called to obtain a reference to a Tango device (in the form
 *     of TDevice)
 * \li TDevice's user refrence count is incremented with TDevice::addRef
 *
 * See also CuActivity::init, execute and onExit
 *
 * @implements CuActivity::init
 *
 */
void CuPollingActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();

    /* get a reference to TDevice */
    d->tdev = d->device_srvc->getDevice(tk["device"].toString());
    /* if polling activity is a fallback because event subscription fails, no need to add ref */
    //  if(!tk["fallback"].toBool())
    d->tdev->addRef();
    tk["conn"] = d->tdev->isValid();
    tk["err"] = !d->tdev->isValid();
    tk["msg"] = d->tdev->getMessage();
    tk.putTimestamp();
    pgreentmp("CuPollingActivity.init complete");
}

/*! \brief the implementation of the CuActivity::execute hook
 *
 * This is repeatedly called in the CuActivity's thread of execution, according to the
 * period chosen for the reader.
 *
 * If the reader's source is a command, the first time execute is run CuTangoWorld::get_command_info
 * is called, so that the data type and format of the command output argument is detected.
 *
 * Either command_inout (through CuTangoWorld::cmd_inout) or read_attribute (CuTangoWorld::read_att)
 * are then invoked. They interpret the result and place it in a CuData that is finally
 * delivered to the main thread by means of publishResult.
 *
 * \par note
 * A CuPollingActivity can be started after a CuEventActivity failure to subscribe to
 * the Tango event system. Moreover, polling is the only possible option to read commands.
 *
 * \par Error handling
 * If an operation fails, the next executions are delayed: the next two attempts take place
 * after 5 seconds and the following ones at intervals of 10 seconds until a successful read.
 *
 * \par contents of the CuData delivered by publishResult ("key": value)
 * \li "device": string: the Tango device name (use CuVariant::toString to convert)
 * \li "point": string: the Tango point (command or attribute name) (CuVariant::toString)
 * \li "is_command": bool: true if the source is a command, false if it is an attribute (CuVariant::toBool)
 * \li "err": bool: true if an error occurred, false otherwise
 * \li "mode": string: the read mode: "event" or "polled" ("polled" in this case)
 * \li "period": integer: the polling period, in milliseconds (CuVariant::toInt)
 * \li "msg": string: a message describing the read operation and its success/failure
 * \li refer to \ref md_lib_cudata_for_tango for a complete description of the CuData key/value
 *     pairs that result from attribute or command read operations.
 *
 *
 * See also CuActivity::execute and CuEventActivity
 *
 * @implements CuActivity::execute
 */
void CuPollingActivity::execute()
{
    //    assert(d->tdev != NULL);
    //    assert(d->my_thread_id == pthread_self());


    bool cmd_success = true;
    CuTangoWorld tangoworld;
    CuData at = getToken(); /* activity token */
    std::vector<CuData> results;
    std::vector<CuData> attdatalist;
    results.reserve(actionsCount());

    std::map<CuTangoActionI *, CuData > datamap;

    Tango::DeviceProxy *dev = d->tdev->getDevice();
    at["mode"] = "polled";
    at["period"] = getTimeout();

    std::multimap<const std::string, const ActionData>::iterator it;
    for(it = d->actions_map.begin(); it != d->actions_map.end(); ++it) {
        CuData res(at);
        const ActionData &action_data = it->second;
        const TSource& tsrc = action_data.tsrc;
        std::string point = tsrc.getPoint();
        std::string srcnam = tsrc.getName();
        std::vector<std::string> argins = tsrc.getArgs();
        void *action_ptr = action_data.action;
        res["src"] = tsrc.getName();
        res["point"] = point;
        res["device"] = tsrc.getDeviceName();
        res["action_ptr"] = CuVariant(action_ptr);
        pgreen2tmp("CuPollingActivity.execute: executing for \"%s\" period %d recipient %p source get name %s\n",
                   res["device"].toString().c_str(), getTimeout(), action_ptr, tsrc.getName().c_str());
        bool is_command = tsrc.getType() == TSource::Cmd;
        CmdData cmd_data = d->din_cache[srcnam];
        CmdData newCmdData;
        if(dev && is_command && cmd_data.is_empty) {
            cmd_success = tangoworld.get_command_info(d->tdev->getDevice(), point, res);
            if(cmd_success) {
                newCmdData = CmdData(res, tangoworld.toDeviceData(argins, res), argins);
                d->din_cache.insert(std::pair<const std::string, CmdData>(srcnam, newCmdData));
            }
        }
        if(dev && is_command && cmd_success) {  // do not try command_inout if no success so far
            // there is no multi-command_inout version
            CmdData& cmdd = d->emptyCmdData;
            if(cmd_data.is_empty) // was found in cache
                cmdd = newCmdData;
            else
                cmdd = cmd_data;
            bool has_argout = cmdd.cmdinfo["out_type"].toLongInt() != Tango::DEV_VOID;
            res["err"] = !cmd_success;
            if(!cmd_success) {
                res["msg"] = std::string("CuPollingActivity.execute: get_command_info failed for \"") + tsrc.getName() + std::string("\"");
                d->errCnt++;
            }
            else {
                cmd_success = tangoworld.cmd_inout(dev, point, cmdd.din, has_argout, res);
            }
        }
        else if(!is_command) { // fill the list of attributes for read_attributes
            attdatalist.push_back(res);
        }

        if(is_command) // attribute results are pushed back later
            results.push_back(res);


        /// temporary prints
        ///
        printf("- point \"%s\" is command %d argins size %d ", point.c_str(), is_command, argins.size());
        if(argins.size() > 0) {
            pgreen2tmp(": { ");
            for(size_t i = 0; i < argins.size(); i++)
                pgreen2tmp("%s, ", argins[i].c_str());
            pgreen2tmp(" }");
        }
        printf("\n");
        ///
        /// end of temporary prints

    } // for(it = d->actions_map.begin()

    // attributes now
    if(dev && attdatalist.size() > 0) {
        bool success = tangoworld.read_atts(d->tdev->getDevice(), attdatalist, results);

    }


    ////    tangoworld.fillThreadInfo(at, this); /* put thread and activity addresses as info */

    //    if(dev && !is_command)
    //    {
    //        cmd_success = tangoworld.read_att(dev, point, at);
    //    }
    //    else if(dev) /* command */
    //    {
    //        if(d->point_info.isEmpty()) {
    //            printf("\e[1;33mget_command_info\e[0m\n");
    //            cmd_success = tangoworld.get_command_info(d->tdev->getDevice(), at["point"].toString(), d->point_info);
    //        }
    //        if(d->point_info.isEmpty())
    //            d->errCnt++;
    //        else
    //        {
    //            cmd_success = tangoworld.cmd_inout(dev, point, d->argins, d->point_info, at);
    //        }
    //    }

    //    if(dev && cmd_success)
    //    {
    //        d->repeat = getTimeout();
    //        d->errCnt = 0;
    //    }
    //    else if(dev && !cmd_success)
    //    {
    //        ++(d->errCnt) > 0 && d->errCnt < 3 ? d->repeat = 5000 : d->repeat = 10000;
    //        printf("\e[1;31mfailed to read attribute for \"%s/%s\": \"%s\" ---> decreasing timeout to %d cuz errcnt is %d\e[0m\n",
    //              devnam.c_str(), point.c_str(), at["msg"].toString().c_str(), d->repeat, d->errCnt);
    //    }
    //    if(dev) {
    //        at["msg"] = tangoworld.getLastMessage();
    //        at["err"] = tangoworld.error();
    //    }
    //    else {
    //        at["msg"] = d->tdev->getMessage();
    //        at["err"] = true;
    //        d->repeat = -1;
    //    }
    publishResult(results);
}

/*! \brief the implementation of the CuActivity::execute hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \li client reference counter is decreased on the TDevice (TDevice::removeRef)
 * \li CuDeviceFactoryService::removeDevice is called to remove the device from the device factory
 *     if the reference count is zero
 * \li the result of the operation is *published* to the main thread through publishResult
 *
 * See also CuActivity::onExit
 *
 * @implements CuActivity::onExit
 */
void CuPollingActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    d->exiting = true;
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    at["msg"] = "EXITED";
    at["mode"] = "POLLED";
    //    CuTangoWorld utils;
    //    utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
    if(d->tdev)
        refcnt = d->tdev->removeRef();
    cuprintf("\e[1;31mCuPollingActivity::onExit(): refcnt = %d called actionRemove for device %s att %s\e[0m\n",
             refcnt, at["device"].toString().c_str(), at["src"].toString().c_str());
    if(refcnt == 0)
    {
        d->device_srvc->removeDevice(at["device"].toString());
        d->tdev = NULL;
    }
    at["exit"] = true;
    publishResult(at);
}

void CuPollingActivity::m_registerAction(const TSource& ts, CuTangoActionI *a)
{
    ActionData adata(ts, a);
    d->actions_map.insert(std::pair<const std::string, const ActionData>(ts.getPoint(), adata)); // multimap
    pgreentmp(" + CuPollingActivity.event: added %s to poller\n", ts.toString().c_str());
}

void CuPollingActivity::m_unregisterAction(const TSource &ts)
{
    std::multimap< const std::string, const ActionData>::iterator it = d->actions_map.begin();
    while(it != d->actions_map.end()) {
        if(it->first == ts.getPoint() && it->second.tsrc == ts) {
            it = d->actions_map.erase(it);
            predtmp(" - CuPollingActivity.event: removed %s from poller\n",  ts.toString().c_str());
        }
        else
            ++it;
    }
}

/** \brief Receive events *from the main thread to the CuActivity thread*.
 *
 * @param e the event. Do not delete e after use. Cumbia will delete it after this method invocation.
 *
 * @see CuActivity::event
 *
 * \note the CuActivityEvent is forwarded to CuContinuousActivity::event
 */
void CuPollingActivity::event(CuActivityEvent *e)
{
    assert(d->my_thread_id == pthread_self());
    if(e->getType() == CuAddPollActionEvent::AddPollAction) {
        m_registerAction(static_cast<CuAddPollActionEvent *>(e)->tsource, static_cast<CuAddPollActionEvent *>(e)->action);
    }
    else if(e->getType() == CuRemovePollActionEvent::RemovePollAction) {
        m_unregisterAction(static_cast<CuRemovePollActionEvent *>(e)->tsource);
    }
    else
        CuContinuousActivity::event(e);
}

/*! \brief returns the type of the polling activity
 *
 * @return the CuPollingActivityType value defined in the Type enum
 */
int CuPollingActivity::getType() const
{
    return CuPollingActivityType;
}

/*! \brief returns the polling period, in milliseconds
 *
 * @return the polling period, in milliseconds
 *
 * @implements CuActivity::repeat
 */
int CuPollingActivity::repeat() const
{
    assert(d->my_thread_id == pthread_self());
    return d->repeat;
}

