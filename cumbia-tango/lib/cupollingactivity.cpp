#include "cupollingactivity.h"
#include "cudevicefactoryservice.h"
#include "tdevice.h"
#include "tsource.h"
#include "cuactivitymanager.h"
#include "cuthreadinterface.h"
#include "cutango-world.h"
#include "cutangoactioni.h"
#include <tango.h>
#include <cumacros.h>
#include <vector>
#include <map>

class CmdData {
public:
    CmdData() {
        is_empty = true;
    }

    CmdData(const CuData& cmd_info, const Tango::DeviceData& dd_in, const std::vector<std::string>& args) {
        cmdinfo = cmd_info;
        din = dd_in;
        is_empty = false;
        argins = args;
    }

    CuData& getCmdInfoRef() {
        return cmdinfo;
    }

    Tango::DeviceData& getDeviceDataRef() {
        return din;
    }

    /*! \brief returns true if this CuData input argument list is empty
     *
     * @return true if CmdData was build with empty args, false otherwise.
     *
     * This can be considered equivalent to testing Tango::DeviceData::is_empty()
     *
     * \note Tango::DeviceData::is_empty throws an exception unless
     *       Tango::DeviceData::reset_exceptions(DeviceData::isempty_flag) is called
     *       on the DeviceData itself
     */
    bool hasInputArgs() const {
        return argins.size() == 0;
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
    int repeat, period;
    int consecutiveErrCnt; // consecutive error counter
    int successfulExecCnt;
    std::string message;
    pthread_t my_thread_id, other_thread_id;
    CuVariant argins;
    CuData point_info;
    std::multimap<const std::string, const ActionData > actions_map;
    // cache for tango command_inout argins
    // multimap because argins may differ
    std::map<const std::string, CmdData> din_cache;
    CmdData emptyCmdData;
    // maps consecutive error count to slowed down polling duration in millis
    std::map<int, int> slowDownRate;
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
                                     CuDeviceFactoryService *df)
    : CuContinuousActivity(token)
{
    d = new CuPollingActivityPrivate;
    d->device_srvc = df;
    d->consecutiveErrCnt = 0;
    d->other_thread_id = pthread_self();
    d->successfulExecCnt = 0;

    int period = 1000;
    if(token.containsKey("period"))
        period = token["period"].toInt();
    d->repeat = d->period = period;
    setInterval(period);
    //  flag CuActivity::CuADeleteOnExit is true
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    // initialize period slow down policy in case of read errors
    d->slowDownRate[1] = 3000;
    d->slowDownRate[2] = 5000;
    d->slowDownRate[4] = 10000;
}

/*! \brief the class destructor
 *
 * deletes the internal data
 */
CuPollingActivity::~CuPollingActivity()
{
    pdelete("CuPollingActivity %p", this);
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

/*! \brief set a custom *slow down rate* to decrease polling period after consecutive
 *         read failures.
 *
 * The default slow down rate policy decreases the read timer to:
 *
 * \li three seconds after one error
 * \li five seconds after two consecutive errors
 * \li ten seconds after four consecutive errors
 *
 * The default polling period is restored after a successful reading
 *
 * @param a map defining how to decrease the polling period after read failures
 *
 * \par Example
 * A map defined like this:
 *
 * \code
 * std::map<int, int> sd_map;
 * sd_map[1] = 2000;
 * sd_map[2] = 5000;
 * sd_map[4] = 10000;
 * activity->setSlowDownRate(sd_map);
 * \endcode
 *
 * behaves as described in the list above if we deal with a default polling period of one second.
 *
 * @see slowDownRate
 */
void CuPollingActivity::setSlowDownRate(const std::map<int, int> &sr_millis)
{
    d->slowDownRate = sr_millis;
}

/*! \brief returns the currently employed slow down rate policy in case of read errors
 *
 * Refer to setSlowDownRate for further details
 */
const std::map<int, int> &CuPollingActivity::slowDownRate() const {
    return d->slowDownRate;
}

void CuPollingActivity::decreasePolling() {
    if(d->consecutiveErrCnt > 0) {
        for(std::map<int,int>::const_reverse_iterator it = d->slowDownRate.rbegin(); it != d->slowDownRate.rend(); ++it) {
            if(d->consecutiveErrCnt >= it->first) {
                d->repeat = it->second;
                break;
            }
        }
    }
    else
        d->repeat = d->period;
    printf("\e[0;33mCuPollingActivity::decreasePolling: \"%s\": error count is %d period is %d d->repeat has been set to %d\e[0m\n",
           getToken().toString().c_str(), d->consecutiveErrCnt, d->period, d->repeat);
}

/*!
 * \brief CuPollingActivity::successfulExecCnt count the number of succesful executions
 * \return the number of successful executions
 *
 * @see consecutiveErrCnt
 */
int CuPollingActivity::successfulExecCnt() const {
    return d->successfulExecCnt;
}

/*!
 * \brief CuPollingActivity::consecutiveErrCnt count the consecutive errors during execution
 * \return the number of consecutive errors encountered within the *execute* method.
 *
 * \note
 * Upon each successful execution, the counter is reset to 0
 *
 * @see successfulExecCnt
 */
int CuPollingActivity::consecutiveErrCnt() const {
    return d->consecutiveErrCnt;
}

const std::multimap<const string, const ActionData> CuPollingActivity::actionsMap() const {
    return d->actions_map;
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
    d->tdev = d->device_srvc->getDevice(tk["device"].toString(), threadToken());
    // CHECK THIS /* if polling activity is a fallback because event subscription fails, no need to add ref */
    //if(!tk["fallback"].toBool())
    d->device_srvc->addRef(d->tdev->getName(), threadToken());
    tk["conn"] = d->tdev->isValid();
    tk["err"] = !d->tdev->isValid();
    tk["msg"] = d->tdev->getMessage();
    tk.putTimestamp();
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
    assert(d->tdev != NULL);
    assert(d->my_thread_id == pthread_self());
    CuTangoWorld tangoworld;
    std::vector<CuData> *results = new std::vector<CuData>();
    std::vector<CuData> attdatalist;
    Tango::DeviceProxy *dev = d->tdev->getDevice();
    bool success = (dev != NULL);
    TSource tsrc;
    size_t i = 0;
    size_t att_idx = 0;
    size_t att_offset = 0;
    if(dev) { // dev is not null
        results->resize(d->actions_map.size());
        attdatalist.resize(d->actions_map.size());
        std::multimap<const std::string, const ActionData>::iterator it;
        for(it = d->actions_map.begin(); it != d->actions_map.end(); ++it) {
            const ActionData &action_data = it->second;
            tsrc = action_data.tsrc;
            const std::string srcnam = tsrc.getName();
            std::string point = tsrc.getPoint();
            std::vector<std::string> argins = tsrc.getArgs();
            void *action_ptr = action_data.action;
            bool is_command = tsrc.getType() == TSource::SrcCmd;
            if(is_command) { // write into results[i]
                (*results)[i] = getToken();
                (*results)[i]["mode"] = "polled";
                (*results)[i]["period"] = getTimeout();
                (*results)[i]["src"] = tsrc.getName();
                (*results)[i]["point"] = point;
                (*results)[i]["device"] = tsrc.getDeviceName();
                (*results)[i]["action_ptr"] = CuVariant(action_ptr);
                //        pgreen2tmp("CuPollingActivity.execute: executing for \"%s\" period %d recipient %p source get name %s\n",
                //                   res["device"].toString().c_str(), getTimeout(), action_ptr, tsrc.getName().c_str());
                CmdData& cmd_data = d->din_cache[srcnam];
                if(dev && cmd_data.is_empty) {
                    success = tangoworld.get_command_info(dev, point, (*results)[i]);
                    if(success) {
                        d->din_cache[srcnam] = CmdData((*results)[i], tangoworld.toDeviceData(argins, (*results)[i]), argins);
                    }
                }
                if(dev && success) {  // do not try command_inout if no success so far
                    // there is no multi-command_inout version
                    CmdData& cmdd = d->din_cache[srcnam];
                    bool has_argout = cmdd.getCmdInfoRef()["out_type"].toLongInt() != Tango::DEV_VOID;
                    (*results)[i]["err"] = !success;
                    if(!success) {
                        (*results)[i]["msg"] = std::string("CuPollingActivity.execute: get_command_info failed for \"") + tsrc.getName() + std::string("\"");
                        d->consecutiveErrCnt++;
                    }
                    else {
                        tangoworld.cmd_inout(dev, point, cmdd.din, has_argout, (*results)[i]);
                    }
                }
                att_offset++;
            }
            else { // save into attdatalist
                attdatalist[att_idx] = getToken();
                attdatalist[att_idx]["mode"] = "polled";
                attdatalist[att_idx]["period"] = getTimeout();
                attdatalist[att_idx]["src"] = tsrc.getName();
                attdatalist[att_idx]["point"] = point;
                attdatalist[att_idx]["device"] = tsrc.getDeviceName();

                attdatalist[att_idx]["action_ptr"] = CuVariant(action_ptr);
                att_idx++;
            }
            i++;

        } // for(it = d->actions_map.begin()

        // attributes now

        if(dev && att_idx > 0) {
            attdatalist.resize(att_idx);
            success = tangoworld.read_atts(d->tdev->getDevice(), attdatalist, results, att_offset);
            if(!success) {
                d->consecutiveErrCnt++;
            }
        }
    }
    else {

    }
    if(success && d->repeat != d->period)
        d->repeat = d->period;
    else if(success) {
        d->consecutiveErrCnt = 0; // reset consecutive error count
        d->successfulExecCnt++;
    }
    else if(dev) {
        decreasePolling();
    }
    else { // device not defined into database
        d->repeat = -1;
    }
    if(!success) {
        // dev is null or some other error (device not defined in database)
        CuData dev_err;
        !dev ? dev_err["msg"] =  d->tdev->getMessage() :
                dev_err["msg"] = "CuPollingActivity: failed to read \"" + tsrc.getName() + "\" (last err: " + d->message + ")";
        dev_err["err"] = true;
        dev_err["name"] = d->tdev->getName();
        dev_err["src"] = tsrc.getName();
        dev_err.putTimestamp();
        results->push_back(dev_err);
    }

    publishResult(results);
}

/*! \brief the implementation of the CuActivity::onExit hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \li client reference counter is decreased on the TDevice (TDevice::removeRef)
 * \li the result of the operation is *published* to the main thread through publishResult
 *
 * See also CuActivity::onExit
 *
 * @implements CuActivity::onExit
 */
void CuPollingActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    dispose();
    CuData at = getToken(); /* activity token */
    // thread safely remove ref and let d->device_srvc dispose TDev if no more referenced
    d->device_srvc->removeRef(at["device"].toString(), threadToken());
    // do not publishResult because CuPoller (which is our listener) may be deleted by CuPollingService
    // from the main thread when its action list is empty (see CuPollingService::unregisterAction)
}

void CuPollingActivity::m_registerAction(const TSource& ts, CuTangoActionI *a)
{
    ActionData adata(ts, a);
    //    if(ts.getType() == TSource::Cmd)
    //        d->cmd_data[d->cmd_data_idx++] = adata;
    //    else
    //        d->att_data[d->att_data_idx++] = adata;
    if(d->actions_map.find(ts.getName()) != d->actions_map.end())
        perr("CuPollingActivity %p m_registerAction: source \"%s\" period %d already registered", this, ts.getName().c_str(), getTimeout());
    else {
        d->actions_map.insert(std::pair<const std::string, const ActionData>(ts.getName(), adata)); // multimap
        pgreen(" + CuPollingActivity %p event: added %s to poller\n", this, ts.toString().c_str());
    }
}

void CuPollingActivity::m_unregisterAction(const TSource &ts)
{
    std::multimap< const std::string, const ActionData>::iterator it = d->actions_map.begin();
    while(it != d->actions_map.end()) {
        if(it->first == ts.getName() && it->second.tsrc == ts) {
            it = d->actions_map.erase(it);
        }
        else
            ++it;
    }
    if(d->actions_map.size() == 0) {
        dispose(); // do not use this activity since now
        // unregister this from the thread
        CuThreadInterface *thread = getActivityManager()->getThread(this);
        /* CuActivityManager.removeConnection is invoked by the thread in order to ensure all scheduled events are processed */
        if(thread)
            thread->unregisterActivity(this);
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
    int ret;
    isDisposable() ? ret = -1 : ret = d->repeat;
    return ret;
}

