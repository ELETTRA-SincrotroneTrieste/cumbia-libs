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
#include "cupoller.h"
#include <culog.h>

#include <tango.h>

/// to remove
#include <cupollingactivity.h>

class TSource;

class CuTReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    TSource tsrc;
    CumbiaTango *cumbia_t;
    CuActivity *event_activity;
    CuPoller *poller;
    bool exit;
    CuConLogImpl li;
    CuLog log;
    CuData property_d, value_d;
    int period;
    CuTReader::RefreshMode refresh_mode;
    bool polling_fallback;
    std::list<void *> activities;
};

CuTReader::CuTReader(const TSource& src, CumbiaTango *ct) : CuTangoActionI()
{
    d = new CuTReaderPrivate;
    d->tsrc = src;
    d->cumbia_t = ct;
    d->event_activity = NULL;
    d->poller = NULL;
    d->exit = false;  // set to true by stop
    d->log = CuLog(&d->li);
    d->period = 1000;
    d->refresh_mode = ChangeEventRefresh;
    d->polling_fallback = false;
}

CuTReader::~CuTReader()
{
    pdelete("~CuTReader deleting %p", this);
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
    bool polling_fallback = false;
    bool err = data["err"].toBool();
    bool a_exit = data["exit"].toBool(); // activity exit flag
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::set<CuDataListener *> lis_copy = d->listeners;
    std::set<CuDataListener *>::iterator it;
    bool event_subscribe_fail = err && !d->exit && data["event"].toString() == "subscribe";

    if(a_exit) // remove from list of started activities
        d->activities.remove(data["ptr"].toVoidP()); // when list is null, can delete this

    // if it's just subscribe_event failure, do not notify listeners
    for(it = lis_copy.begin(); it != lis_copy.end() && !event_subscribe_fail;   ++it) {
        (*it)->onUpdate(data);
    }

    if(err && !d->exit)
    {
        polling_fallback = true;
        cuprintf("starting polling activity cuz event is err %d\n", data["err"].toBool());
        // stop event activity. it will auto delete.
        // m_unregisterEventActivity will set d->event_activity to NULL
        if(d->event_activity)
            m_unregisterEventActivity();
        m_registerToPoller();
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

/** \brief Send data with parameters to configure the reader.
 *
 * @param data a CuData bundle with the settings to apply to the reader.
 *
 * \par Valid keys
 * \li "period": integer. Change the polling period, if the refresh mode is CuTReader::PolledRefresh
 * \li "refresh_mode". A CuTReader::RefreshMode value to change the current refresh mode.
 * \li "read" (value is irrelevant). If the read mode is CuTReader::PolledRefresh, a read will be
 *     performed.
 *
 * @see getData
 *
 */
void CuTReader::sendData(const CuData &data)
{
    printf("\e[1;35msendData sending %s\e[0m\n", data.toString().c_str());
    if(data.containsKey("refresh_mode"))
        d->refresh_mode = static_cast<CuTReader::RefreshMode>(data["refresh_mode"].toInt());
    if(data.containsKey("period"))
        d->period = data["period"].toInt();

    if(d->event_activity && data.containsKey("refresh_mode"))
        setRefreshMode(d->refresh_mode);
    if(d->poller && d->period != d->poller->period()) {
        printf("\e[1;33mchanging polling period to new one %d\n", d->period);
        m_unregisterFromPoller(); // unregister from old poller with old period
        m_registerToPoller();  // find an existing one or get a new poller for the new period
    }

//    if(d->event_activity && d->event_activity->getType() == CuPollingActivity::CuPollingActivityType && data.containsKey("read"))
//    {
//        d->cumbia_t->postEvent(d->event_activity, new CuExecuteEvent());
//    }
//    if(!d->event_activity)
//        perr("CuTReader.sendData: cannot send data without a running activity");
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

/*! \brief set or change the reader's refresh mode
 *
 * If the reading activity hasn't been started yet, the mode is saved for later.
 * If an activity is already running and the requested mode is different, the current
 * activity is unregistered and a new one is started.
 *
 * @param rm a value chosen from CuTReader::RefreshMode.
 *
 *
 */
void CuTReader::setRefreshMode(CuTReader::RefreshMode rm)
{
    d->refresh_mode = rm;
    // start a new event activity if
    // 1. rm is an event driven mode AND
    // 2a. running activity is a poller OR
    // 2b. running activity refresh mode is not a CuPollingActivityType but refresh mode is different
    //     from the desired one
    if(!d->event_activity && isEventRefresh(rm) ) {
        // need an event activity and there is no one
        m_unregisterFromPoller();
        m_startEventActivity();
    }
    else if(d->event_activity &&  d->event_activity->getToken()["rmode"].toString() != refreshModeStr()) {
        // already have one but want different event mode
        m_unregisterEventActivity();
        m_startEventActivity();
    }
    else if(d->event_activity && (rm == CuTReader::PolledRefresh || rm == CuTReader::Manual))
    {
        m_unregisterEventActivity();
        d->polling_fallback = false;
        m_registerToPoller();
    }
    // if the desired mode is Manual, the current activity is a polling activity
    if(rm == CuTReader::Manual)
    {
        printf("\e[1;31mTO IMPLEMENT\e[0m: mode is set to MANUAL -----> pausing activity!\n");
        //d->poller->pause();
    }

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

void CuTReader::setPeriod(int millis)
{
    d->period = millis;
}

/*
 * main thread
 */
void CuTReader::stop()
{
    if(d->exit)
        d->log.write("CuTReader.stop", CuLog::Error, CuLog::Read, "stop called twice for reader %s", this->getToken()["source"].toString().c_str());
    else
    {
        d->exit = true;
        if(d->event_activity)
            m_unregisterEventActivity();
        else
            m_unregisterFromPoller();
    }
}

void CuTReader::addDataListener(CuDataListener *l)
{
    std::set<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuTReader::removeDataListener(CuDataListener *l)
{
    if(l->invalid())
    {
        d->listeners.erase(l);
        if(!d->listeners.size()) {
            stop();
        }
    }
    else if(d->listeners.size() == 1) {
        stop();
    }
    else
        d->listeners.erase(l);
}

size_t CuTReader::dataListenersCount()
{
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
    return d->refresh_mode == ChangeEventRefresh || d->refresh_mode == PeriodicEventRefresh
            || d->refresh_mode == ArchiveEventRefresh;
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
 * If the refresh mode is Manual, a CuPollingActivity is started and CumbiaTango::pauseActivity is
 * called immediately. To trigger a read in Manual mode, use sendData with a CuData containing a
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
    if(d->refresh_mode == Manual)
        d->cumbia_t->pauseActivity(d->event_activity);
}

void CuTReader::m_startEventActivity()
{
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));

    CuData at("src", d->tsrc.getName()); /* activity token */
    at["device"] = d->tsrc.getDeviceName();
    at["point"] = d->tsrc.getPoint();
    at["activity"] = "event";
    at["rmode"] = refreshModeStr();

    CuData tt("device", d->tsrc.getDeviceName()); /* thread token */
    d->event_activity = new CuEventActivity(at, df);
    d->activities.push_back(d->event_activity);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->event_activity, this, tt, fi, bf);
    cuprintf("> CuTReader.m_startEventActivity reader %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->event_activity);
}

void CuTReader::m_registerToPoller()
{
    CuPollingService *polling_service = static_cast<CuPollingService *>(d->cumbia_t->getServiceProvider()->
                                                                        get(static_cast<CuServices::Type> (CuPollingService::CuPollingServiceType)));
    d->poller = polling_service->getPoller(d->cumbia_t, d->period);
    d->poller->registerAction(d->tsrc, this);
    cuprintf("> CuTReader.m_startPollingActivity reader %p thread 0x%lx ACTIVITY %p == \e[0;32mSTARTING POLLING\e[0m\n\n", this, pthread_self(), d->event_activity);
}

void CuTReader::m_unregisterFromPoller()
{
    if(d->poller) {
        d->poller->unregisterAction(this);
    }
}

void CuTReader::m_unregisterEventActivity()
{
    d->cumbia_t->unregisterActivity(d->event_activity);
    d->event_activity = NULL; // not safe to dereference henceforth
}

