#include "cutreader.h"
#include "cumbiatango.h"
#include "tdevice.h"
#include "cudevicefactoryservice.h"
#include "cuactionfactoryservice.h"
#include "cupollingservice.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <set>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include "cueventactivity.h"
#include "cupollingactivity.h" // for CuPollingActivity::Type
#include "cupoller.h"
#include <culog.h>
#include <tango.h>

class TSource;

class CuTReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    TSource tsrc;
    CumbiaTango *cumbia_t;
    CuActivity *event_activity;
    bool exit;
    CuConLogImpl li;
    CuLog log;
    CuData property_d, value_d;
    int period;
    CuTReader::RefreshMode refresh_mode;
    bool polling_fallback;
    std::list<void *> activities;
    int manual_mode_period;
    CuData options;
};

CuTReader::CuTReader(const TSource& src, CumbiaTango *ct) : CuTangoActionI()
{
    printf("CuTReader.CuTReader constructor %s\n", src.getName().c_str());
    d = new CuTReaderPrivate;
    d->tsrc = src;
    d->cumbia_t = ct;
    d->event_activity = NULL;
    d->exit = false;  // set to true by stop
    d->log = CuLog(&d->li);
    d->period = 1000;
    d->refresh_mode = ChangeEventRefresh;
    d->polling_fallback = false;
    d->manual_mode_period = 1000 * 3600 * 24 * 10; // ten days
}

CuTReader::~CuTReader()
{
    pdelete("~CuTReader %p", this);
    delete d;
}

/*! \brief progress notification callback
 *
 * @param step the completed steps in the background
 * @param total the total number of steps
 * @param data CuData with data from the background activity
 *
 * The current implementation does nothing
 */
void CuTReader::onProgress(int step, int total, const CuData &data)
{
    (void) step;  (void) total;  (void) data;
}

void CuTReader::onResult(const std::vector<CuData> &datalist)
{
    (void) datalist;
}

/*
 * \brief delivers to the main thread the result of a task executed in background.
 *
 * See  \ref md_lib_cudata_for_tango
 *
 * The d->exit flag is true only if the CuTReader::stop has been called. (data listener destroyed
 * or reader disconnected ("unset source") )
 * Only in this case CuTReader auto deletes itself when data["exit"] is true.
 * data["exit"] true is not enough to dispose CuTReader because CuTReader handles two types of
 * activities (polling and event).
 *
 * If the error flag is set by the CuEventActivity because subscribe_event failed, the poller is started
 * and the error *is not* notified to the listener(s)
 *
 */
void CuTReader::onResult(const CuData &data)
{
    bool err = data["err"].toBool();
    bool a_exit = data["exit"].toBool(); // activity exit flag
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::set<CuDataListener *> lis_copy = d->listeners;
    std::set<CuDataListener *>::iterator it;
    bool event_subscribe_fail = err && !d->exit && data["event"].toString() == "subscribe";
    if(a_exit) // remove from list of started activities
        d->activities.remove(data["ptr"].toVoidP()); // when list is null, can delete this

    // if it's just subscribe_event failure, do not notify listeners
    for(it = lis_copy.begin();
        !a_exit  && !event_subscribe_fail && it != lis_copy.end();   ++it) {
        (*it)->onUpdate(data);
    }
    if(err && !d->exit)
    {
        // stop event activity. it will auto delete.
        // m_unregisterEventActivity will set d->event_activity to NULL
        if(d->event_activity)
            m_unregisterEventActivity();
        CuPollingService *polling_service = static_cast<CuPollingService *>(d->cumbia_t->getServiceProvider()->
                                                                            get(static_cast<CuServices::Type> (CuPollingService::CuPollingServiceType)));
        if(!polling_service->actionRegistered(this, d->period) ) {
            d->refresh_mode = CuTReader::PolledRefresh;
            m_registerToPoller();
        }
    }

    /* remove last listener and delete this
     * - it is safe to remove if d->activities is empty
     * - if d->exit is set to true (CuTReader has been stop()ped )
     */
    if(d->exit && a_exit && d->activities.size() == 0)
    {
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

/*! \brief returns the CuData storing the token that identifies this action
 *
 * @return a CuData with the following key/value pairs
 *
 * \li "source" --> the name of the source (string)
 * \li "type" --> a constant string: "reader"
 */
CuData CuTReader::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("reader");
    return da;
}

/*! returns the *tango* source (TSource)
 *
 * @return a TSource object that describes the Tango source
 */
TSource CuTReader::getSource() const
{
    return d->tsrc;
}

CuTangoActionI::Type CuTReader::getType() const
{
    return CuTangoActionI::Reader;
}

/** \brief Get parameters from the reader.
 *
 * @param d_inout a reference to a CuData bundle containing the parameter names
 *        as keys. getData will associate the values to the keys.
 *        Unrecognized keys are ignored.
 *
 * \par Valid keys
 * \li "period": returns an int with the polling period
 * \li "refresh_mode": returns a CuTReader::RefreshMode that can be converted to int
 * \li "mode": returns a string representation of the CuTReader::RefreshMode
 *
 * @see sendData
 */
void CuTReader::getData(CuData &d_inout) const
{
    if(d_inout.containsKey("period"))
        d_inout["period"] = d->period;
    if(d_inout.containsKey("refresh_mode"))
        d_inout["refresh_mode"] = d->refresh_mode;
    if(d_inout.containsKey("mode"))
        d_inout["mode"] = refreshModeStr();
}


/*!
 * \brief CuTReader::setOptions sets the options on the reader
 * \param options CuData key/value bundle
 *
 * \par Options
 * \list
 * \li manual [bool]: manual mode: the caller will not receive updates
 * \li refresh_mode [int, one of CuTReader::RefreshMode] set the refresh mode to mode
 * \li period set the period that will be used to refresh the source if the mode is polled
 *
 * \note If the current mode is not CuTReader::PolledRefresh, setting the *period* is not
 *       enough to change the refresh mode. On the other hand, setting a *period* will
 *       save the value shall the refresh mode change to polling
 *
 * @see sendData
 *
 */
void CuTReader::setOptions(const CuData &options) {
    d->options = options;
    if(options.containsKey("manual") && options["manual"].toBool())
        setRefreshMode(CuTReader::Manual);
    else {
        if(options.containsKey("period")) {
            int p = 1000;
            options["period"].to<int>(p);
            if(p > 0 )
                setPeriod(p);
        }
        if(options.containsKey("refresh_mode")) {
            int rm = CuTReader::PolledRefresh;
            options["refresh_mode"].to<int>(rm);
            if(rm >= PolledRefresh && rm <= Manual)
                setRefreshMode(static_cast<CuTReader::RefreshMode>(rm));
        }
    }
}

void CuTReader::m_update_options(const CuData newo) {
    int rm = -1, p = -1;
    if(newo.containsKey("manual")) {
        d->options["manual"] = newo["manual"];
        rm = CuTReader::Manual;
    }
    if(newo.containsKey("refresh_mode"))
        newo["refresh_mode"].to<int>(rm);
    if(newo.containsKey("period"))
        newo["period"].to<int>(p);
    if(rm >= CuTReader::PolledRefresh && rm <= CuTReader::Manual)
        d->options["refresh_mode"] = rm;
    if(rm == CuTReader::Manual)
        d->options["period"] = d->manual_mode_period;
}

/** \brief Send data with parameters to configure the reader.
 *
 * @param data a CuData bundle with the settings to apply to the reader.
 *
 * \par Valid keys
 * \li "period": integer. Change the polling period. Does not imply a change in refresh mode
 * \li "refresh_mode". A CuTReader::RefreshMode value to change the current refresh mode.
 * \li "manual" Equivalent to *refresh_mode* set to CuTReader::Manual, but more "engine unaware", recommended
 * \li "read" [any value, *empty* included]: ask an immediate reading
 * \li "args" [std::vector<std::string>]: change the arguments of an ongoing reading (applies to commands with
 *     argins)
 *
 * \note *period, manual and refresh_mode* update internal *options*.
 *
 * @see getData
 *
 */
void CuTReader::sendData(const CuData &data) {
    m_update_options(data);
    bool do_read = data.containsKey("read");
    bool has_payload = data.containsKey("payload");
    bool has_args = data.containsKey("args");
    int rm = -1, period = -1;
    if(data.containsKey("manual"))
        rm = CuTReader::Manual;
    else if(data.containsKey("refresh_mode"))
        data["refresh_mode"].to<int>(rm);

    if(data.containsKey("period"))
        data["period"].to<int>(period);

    if(rm > -1 && rm != d->refresh_mode) { // refresh mode changed
        setRefreshMode(static_cast<CuTReader::RefreshMode>(rm), period);
    }
    else if(period > 0 && d->refresh_mode == CuTReader::PolledRefresh) { // refresh mode unchanged, period changed
        CuPollingService *polling_service = static_cast<CuPollingService *>(d->cumbia_t->getServiceProvider()->
                                                                            get(static_cast<CuServices::Type> (CuPollingService::CuPollingServiceType)));
        CuPoller *poller = polling_service->getPoller(d->cumbia_t, d->period); // poller with current period
        if(poller && period != poller->period()) {
            m_unregisterFromPoller(); // unregister from old poller (d->period must stay unchanged)
            d->period = period;       // update d->period - mode unchanged
            m_registerToPoller();     // with new period
        }
    }
    else if(do_read) { // post a CuExecuteEvent to a polling activity
        CuActivity *activity = m_find_Activity();
        if(activity)
            d->cumbia_t->postEvent(activity, new CuExecuteEvent());
    }
    else if(do_read && isEventRefresh(d->refresh_mode) && d->event_activity) {
        CuData errdat(getToken());
        errdat.set("err", true).set("msg", "CuTReader.sendData: \"read\" request cannot be forwarded to an event type activity");
        perr("CuTReader.sendData: error %s (posted to event activity %p)\n", errdat.toString().c_str(), d->event_activity);
        d->cumbia_t->postEvent(d->event_activity, new CuDataEvent(errdat));
    }
    if(has_args || has_payload) {
        CuActivity *activity = m_find_Activity();
        if(activity && has_payload)  d->cumbia_t->postEvent(activity, new CuDataEvent(data));
        else if(activity) d->cumbia_t->postEvent(activity, new CuArgsChangeEvent(d->tsrc, data["args"].toStringVector()));
    }

}

/*! \brief set or change the reader's refresh mode
 *
 * If the reading activity hasn't been started yet, the mode is saved for later.
 * If an activity is already running and the requested mode is different, the current
 * activity is unregistered and a new one is started.
 *
 * @param rm a value chosen from CuTReader::RefreshMode.
 */
void CuTReader::setRefreshMode(CuTReader::RefreshMode rm, int period)
{
    CuPollingService *polling_service = static_cast<CuPollingService *>(d->cumbia_t->getServiceProvider()->
                                                                        get(static_cast<CuServices::Type> (CuPollingService::CuPollingServiceType)));
    bool polled = polling_service->actionRegistered(this, d->period);
    if(d->event_activity || polled) {

        // start a new event activity if
        // 1. rm is an event driven mode AND
        // 2a. running activity is a poller OR
        // 2b. running activity refresh mode is not a CuPollingActivityType but refresh mode is different
        //     from the desired one
        if(!d->event_activity && isEventRefresh(rm) ) {
            // need an event activity and there is no one
            m_unregisterFromPoller();
            d->refresh_mode = rm;
            m_startEventActivity();
        }
        else if(isEventRefresh(rm) && d->event_activity && d->refresh_mode != rm) {
            // already have one but want different event mode
            m_unregisterEventActivity();  // unregister current with the current event refresh mode
            d->refresh_mode = rm;
            m_startEventActivity();       // register a new one with the desired event refresh mode
        }
        else if(rm == CuTReader::PolledRefresh || rm == CuTReader::Manual) {
            printf("CuTReader::setRefreshMode: rm %d d->refresh_mode %d d->event_activity %p period %d d->period %d\n",
                   rm, d->refresh_mode, d->event_activity, period, d->period);
            if(d->event_activity)
                m_unregisterEventActivity();
            if(rm == CuTReader::Manual)
                period = d->manual_mode_period;
            if(period > -1 && d->period != period) {
                m_unregisterFromPoller(); // unregister from old poller with old period
                d->period = period;
            }
            if(d->refresh_mode != rm) {
                d->polling_fallback = false;
                printf("CuTReader::setRefreshMode calling m_registerToPoller d->period %d options %s\n", d->period, datos(d->options));
                m_registerToPoller();
            }
        }
    }
    else if(rm == CuTReader::Manual)
        d->period = d->manual_mode_period;
    if(d->refresh_mode != rm) // time to store rm into d->refresh_mode if not already done before
        d->refresh_mode = rm;
}

string CuTReader::refreshModeStr() const
{
    switch(d->refresh_mode)
    {
    case CuTReader::PolledRefresh:
        return "PolledRefresh";
    case CuTReader::ChangeEventRefresh:
        return "ChangeEventRefresh";
    case CuTReader::Manual:
        return "Manual";
    case CuTReader::ArchiveEventRefresh:
        return "ArchiveEventRefresh";
    case CuTReader::PeriodicEventRefresh:
        return "PeriodicEventRefresh";
    default:
        return "InvalidRefreshMode";
    }
}

int CuTReader::period() const
{
    return d->period;
}

CuTReader::RefreshMode CuTReader::refreshMode() const
{
    return d->refresh_mode;
}

void CuTReader::setPeriod(int millis) {
    d->period = millis;
}

/*
 * main thread
 */
void CuTReader::stop()
{
    if(!d->exit)
    {
        d->exit = true;
        if(d->event_activity)
            m_unregisterEventActivity();
        else
            m_unregisterFromPoller();
    }
}

void CuTReader::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuTReader::removeDataListener(CuDataListener *l) {
    d->listeners.erase(l);
    if(!d->listeners.size()) {
        stop();
    }
}

size_t CuTReader::dataListenersCount() {
    return d->listeners.size();
}

bool CuTReader::exiting() const
{
    return d->exit;
}

/*! \brief returns true if the parameter is an event driven RefreshMode, false otherwise.
 *
 * @param rm a value picked from CuTReader::RefreshMode enumeration
 * @return true if rm is ChangeEventRefresh, PeriodicEventRefresh or ArchiveEventRefresh,
 *         false otherwise
 *
 */
bool CuTReader::isEventRefresh(CuTReader::RefreshMode rm) const
{
    return rm == ChangeEventRefresh || rm == PeriodicEventRefresh || rm == ArchiveEventRefresh;
}

/*! \brief creates and registers a CuEventActivity or a CuPollingActivity to read from the Tango control system
 *
 * \note
 * This function is internally used by the library. Clients shouldn't need to deal with it.
 *
 * This method
 * \li fills in a CuData called *activity token*, that will be passed to the CuWriteActivity
 * \li fills in a CuData called *thread token*, used to register the CuWriteActivity and make
 *     the activity shared between writers with the same target.
 * \li instantiates and registers (i.e. starts) either a CuEventActivity or a CuPollingActivity,
 *     according to the RefreshMode value. If the CuEventActivity fails subscribing to the Tango
 *     event system, it is replaced by CuPollingActivity (in CuTReader::onResult)
 *
 * \note
 * start is usually called by CumbiaTango::addAction, which in turn is called by qumbia-tango-controls
 * CuTControlsReader::setSource
 *
 * If the refresh mode is Manual, a CuPollingActivity is started, with a very long period.
 * To trigger a read in Manual mode, use sendData with a CuData containing a
 * key named "*read*". See sendData for further details.
 *
 */
void CuTReader::start()
{
    pr_thread();
    if(d->refresh_mode == ChangeEventRefresh)
        m_startEventActivity();
    else {
        d->polling_fallback = false;
        m_registerToPoller();
    }
}

void CuTReader::m_startEventActivity()
{
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));

    CuData at("src", d->tsrc.getName()); /* activity token */
    at.set("device", d->tsrc.getDeviceName()).set("point", d->tsrc.getPoint()).set("activity", "event")
            .set("rmode", refreshModeStr());
    at.merge(d->options);
    // thread token: by default device name, but can be tuned
    // through the "thread_token" option (setOptions)
    CuData thtok= CuData("device", d->tsrc.getDeviceName());
    if(d->options.containsKey("thread_token"))
        thtok["thread_token"] = d->options["thread_token"];
    d->event_activity = new CuEventActivity(at, df);
    d->activities.push_back(d->event_activity);
    d->refresh_mode = ChangeEventRefresh; // update refresh mode
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->event_activity, this, thtok, fi, bf);
}

void CuTReader::m_registerToPoller()
{
    CuPollingService *polling_service = static_cast<CuPollingService *>(d->cumbia_t->getServiceProvider()->
                                                                        get(static_cast<CuServices::Type> (CuPollingService::CuPollingServiceType)));
    polling_service->registerAction(d->cumbia_t, d->tsrc, d->period, this, d->options);
}

void CuTReader::m_unregisterFromPoller()
{
    CuPollingService *polling_service = static_cast<CuPollingService *>(d->cumbia_t->getServiceProvider()->
                                                                        get(static_cast<CuServices::Type> (CuPollingService::CuPollingServiceType)));
    polling_service->unregisterAction(d->period, this);
}

void CuTReader::m_unregisterEventActivity()
{
    d->cumbia_t->unregisterActivity(d->event_activity);
    d->event_activity = NULL; // not safe to dereference henceforth
    // but leave the activity in the list of activities!
}

CuActivity *CuTReader::m_find_Activity() {
    CuActivityManager *am = static_cast<CuActivityManager *>(d->cumbia_t->getServiceProvider()->
                                                             get(static_cast<CuServices::Type> (CuServices::ActivityManager)));
    CuData at = getToken(); // activity manager needs device period activity
    at.set("device", d->tsrc.getDeviceName()).set("period", d->period).set("activity", "poller");
    CuActivity *activity = am->findActiveMatching(at);
    if(activity && activity->getType() == CuPollingActivity::CuPollingActivityType)
        return activity;
    return nullptr;
}


