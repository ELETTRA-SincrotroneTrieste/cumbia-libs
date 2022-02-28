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

class CmdData { // define here in .cpp: needs tango.h
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

CuActivityEvent::Type CuAddPollActionEvent::getType() const {
    return static_cast<CuActivityEvent::Type> (AddPollAction);
}

CuRemovePollActionEvent::CuRemovePollActionEvent(const TSource &t) {
    tsource = t;
}

CuActivityEvent::Type CuRemovePollActionEvent::getType() const {
    return static_cast<CuActivityEvent::Type> (RemovePollAction);
}

CuActivityEvent::Type CuArgsChangeEvent::getType() const {
    return static_cast<CuActivityEvent::Type> (ArgsChangeEvent);
}

/* @private */
class CuPollingActivityPrivate
{
public:
    CuPollingActivityPrivate(CuDeviceFactoryService *df, const CuData &opt, const CuData &ta, CuPollDataUpdatePolicy upd_po)
        : device_srvc(df), consecutiveErrCnt{0},  successfulExecCnt{0}, options(opt), tag(ta), updpo{upd_po} {}

    CuDeviceFactoryService *device_srvc;
    TDevice *tdev;
    int repeat, period;
    int consecutiveErrCnt; // consecutive error counter
    int successfulExecCnt;
    std::string message;
    pthread_t my_thread_id, other_thread_id;
    CuVariant argins;
    CuData point_info;
    CuData options, tag;
    // map src name --> ActionData: actions are of
    // reader type only (no need for multimap)
    std::vector<TSource > cmds;

    // use two coupled vectors to avoid another wrapping class or map associating attribute
    // names with their respective data. When we add/remove to/from v_attd, do the same on v_attn
    std::vector<CuData> v_attd; // cache attribute values to optimize updates, if required
    std::vector<std::string> v_attn; // attribute names, coupled with v_attd

    // cache for tango command_inout argins
    // multimap because argins may differ
    std::map<const std::string, CmdData> din_cache;
    CmdData emptyCmdData;
    // maps consecutive error count to slowed down polling duration in millis
    std::map<int, int> slowDownRate;
    CuPollDataUpdatePolicy updpo;
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
                                     CuDeviceFactoryService *df, const CuData &options, const CuData &tag,
                                     CuPollDataUpdatePolicy updpo)
    : CuContinuousActivity(token)
{
    d = new CuPollingActivityPrivate(df, options, tag, updpo);
    d->other_thread_id = pthread_self();
    int period = 1000;
    if(token.containsKey("period"))
        token["period"].to<int>(period);
    d->repeat = d->period = period;
    setInterval(period);
    //  flag CuActivity::CuADeleteOnExit is true
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    // initialize period slow down policy in case of read errors
    d->slowDownRate[1] = 3;
    d->slowDownRate[2] = 5;
    d->slowDownRate[4] = 10;
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
void CuPollingActivity::setArgins(const CuVariant &argins) {
    d->argins = argins;
}

size_t CuPollingActivity::actionsCount() const {
    return d->cmds.size() + d->v_attd.size();
}

size_t CuPollingActivity::srcCount() const
{
    return d->cmds.size();
}

/*! \brief set a custom *slow down rate* to decrease polling period after consecutive
 *         read failures.
 *
 * The default slow down rate policy decreases the read timer to:
 *
 * \li three times the current period after one error
 * \li five times the current period after two consecutive errors
 * \li ten times the current period after four consecutive errors
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
 * sd_map[1] = 1.5;
 * sd_map[2] = 3;
 * sd_map[4] = 8;
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
                d->repeat = d->period * it->second;
                break;
            }
        }
    }
    else
        d->repeat = d->period;
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
    d->tdev = d->device_srvc->getDevice(tk["device"].toString(), threadToken());
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
 * \li "is_command": bool: true if the source is a command, false if it is an attribute (CuVariant::toBool)
 * \li "err": bool: true if an error occurred, false otherwise
 * \li "mode": string: the read mode: "E" or "P" ("P" in this case)
 * \li "period": integer: the polling period, in milliseconds (CuVariant::toInt)
 * \li "msg": string: a message describing a read error upon failure. Not set in case of success (since: 1.3.0)
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
    Tango::DeviceProxy *dev = d->tdev->getDevice();
    bool success = (dev != nullptr);
    size_t att_idx = 0;
    size_t res_offset = 0;
    if(dev) { // dev is not null
        results->reserve(d->cmds.size() + d->v_attd.size());
        // 1. commands (d->cmdmap)
        for(size_t i = 0; i <  d->cmds.size(); i++) {
            const TSource &tsrc = d->cmds[i];
            const std::string& srcnam = tsrc.getName();
            const std::string& point = tsrc.getPoint();
            results->push_back(d->tag);
            (*results)[i]["mode"] = "P";
            (*results)[i]["period"] = getTimeout();
            (*results)[i]["src"] = tsrc.getName();
            CmdData& cmd_data = d->din_cache[srcnam];
            if(dev && cmd_data.is_empty) {
                success = tangoworld.get_command_info(dev, point, (*results)[i]);
                if(success) {
                    const std::vector<std::string> &argins = tsrc.getArgs();
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
            res_offset++;
        } // end cmds
        for(size_t i = 0; att_idx >= 0 && i < d->v_attd.size(); i++) { // attributes
            success = tangoworld.read_atts(d->tdev->getDevice(), &d->v_attn, &d->v_attd, results, d->updpo);
            if(!success) {
                d->consecutiveErrCnt++;
            }
        }
    }

    if(success && d->repeat != d->period)
        d->repeat = d->period;
    else if(success) {
        d->consecutiveErrCnt = 0; // reset consecutive error count
        d->successfulExecCnt++;
    }
    else if(dev) {
        perr("%s", std::string("cupollingactivity::execute: read error: " + tangoworld.getLastMessage() + " dev " + d->tdev->getName()).c_str());
        decreasePolling();
    }
    else { // device not defined into database
        d->repeat = -1;
    }
    if(!success) {
        // dev is null or some other error (device not defined in database)
        CuData dev_err;
        !dev ? dev_err["msg"] =  d->tdev->getMessage() :
                dev_err["msg"] = "CuPollingActivity: read failed (last err: " + d->message + ")";
        dev_err["err"] = true;
        dev_err["name"] = d->tdev->getName();
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

void CuPollingActivity::m_registerAction(const TSource& ts) {
    bool is_command = ts.getType() == TSource::SrcCmd;
    if(is_command)
        d->cmds.push_back(ts);
    else {
        d->v_attd.push_back(d->tag.set("src", ts.getName()).set("mode", "P").set("period", d->period));
        d->v_attn.push_back(ts.getPoint());
    }
}

void CuPollingActivity::m_unregisterAction(const TSource &ts) {
    m_cmd_remove(ts.getName());
    m_v_attd_remove(ts.getName());
    if(d->cmds.size() == 0 && d->v_attd.size() == 0) {
        dispose(); // do not use this activity since now
        // unregister this from the thread
        CuThreadInterface *thread = getActivityManager()->getThread(this);
        /* CuActivityManager.removeConnection is invoked by the thread in order to ensure all scheduled events are processed */
        if(thread)
            thread->unregisterActivity(this);
    }
}

void CuPollingActivity::m_edit_args(const TSource &src, const std::vector<string> &args) {
    std::map<std::string, CmdData>::iterator it = d->din_cache.find(src.getName());
    if(it != d->din_cache.end()) {
        it->second.argins = args;
        it->second.din = CuTangoWorld().toDeviceData(args, it->second.cmdinfo);
    }
}

void CuPollingActivity::m_v_attd_remove(const string &src) {
    printf("CuPollingActivity::m_v_attd_remove: before : size %ld\n", d->v_attd.size());
    for(const CuData& d : d->v_attd)
        printf("- %s\n", datos(d));

    d->v_attd.erase(std::find_if(d->v_attd.begin(), d->v_attd.end(), [src](const CuData& da) {  return da.s("src") == src; }) );
    d->v_attn.erase(std::find(d->v_attn.begin(), d->v_attn.end(), src));

    printf("CuPollingActivity::m_v_attd_remove: after lambda:\n");
    for(const CuData& d : d->v_attd)
        printf("- %s\n", datos(d));
}

void CuPollingActivity::m_cmd_remove(const string &src) {
    std::vector<TSource>::iterator it = d->cmds.begin();
    while(it != d->cmds.end()) {
        if(it->getName() == src) {
            printf("\e[1;35mm_cmds_remove removed command %s\e[0m\n", it->getName().c_str());
            it = d->cmds.erase(it);
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
void CuPollingActivity::event(CuActivityEvent *e) {
    assert(d->my_thread_id == pthread_self());
    if(e->getType() == static_cast<int>(CuAddPollActionEvent::AddPollAction)) {
        m_registerAction(static_cast<CuAddPollActionEvent *>(e)->tsource);
    }
    else if(e->getType() == static_cast<int>(CuRemovePollActionEvent::RemovePollAction)) {
        m_unregisterAction(static_cast<CuRemovePollActionEvent *>(e)->tsource);
    }
    else if(e->getType() == static_cast<int>(CuArgsChangeEvent::ArgsChangeEvent)) {
        m_edit_args(static_cast<CuArgsChangeEvent* >(e)->ts, static_cast<CuArgsChangeEvent *>(e)->args);
    }
    else
        CuContinuousActivity::event(e);
}

/*! \brief returns the type of the polling activity
 *
 * @return the CuPollingActivityType value defined in the Type enum
 */
int CuPollingActivity::getType() const {
    return CuPollingActivityType;
}

/*! \brief returns the polling period, in milliseconds
 *
 * @return the polling period, in milliseconds
 *
 * @implements CuActivity::repeat
 */
int CuPollingActivity::repeat() const {
    assert(d->my_thread_id == pthread_self());
    int ret;
    isDisposable() ? ret = -1 : ret = d->repeat;
    return ret;
}
