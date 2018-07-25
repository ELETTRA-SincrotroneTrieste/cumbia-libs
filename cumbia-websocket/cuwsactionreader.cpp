#include "cuwsactionreader.h"
#include "cumbiawebsocket.h"
#include "cuwsactionfactoryservice.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <set>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <curandomgenactivity.h>

#include "cuwsactionreader.h"
#include <cumacros.h>


class CuWSActionReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    WSSource tsrc;
    CumbiaWebSocket *cumbia_ws;
    bool exit;
    CuRandomGenActivity *randomgen_a;
    CuData property_d, value_d;
    int period;
    CuWSActionReader::RefreshMode refresh_mode;
};

CuWSActionReader::CuWSActionReader(const WSSource& src, CumbiaWebSocket *ct) : CuWSActionI()
{
    d = new CuWSActionReaderPrivate;
    d->tsrc = src;
    d->cumbia_ws = ct;
    d->exit = false;  // set to true by stop
    d->period = 1000;
    d->refresh_mode = RandomGenerator;
}

CuWSActionReader::~CuWSActionReader()
{
    pdelete("~CuWSActionReader %p", this);
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
void CuWSActionReader::onProgress(int step, int total, const CuData &data)
{
    (void) step;  (void) total;  (void) data;
}

void CuWSActionReader::onResult(const std::vector<CuData> &datalist)
{
    (void) datalist;
}

/*
 * \brief delivers to the main thread the result of a task executed in background.
 *
 * See  \ref md_lib_cudata_for_tango
 *
 * The d->exit flag is true only if the CuWSActionReader::stop has been called. (data listener destroyed
 * or reader disconnected ("unset source") )
 * Only in this case CuWSActionReader auto deletes itself when data["exit"] is true.
 * data["exit"] true is not enough to dispose CuWSActionReader because CuWSActionReader handles two types of
 * activities (polling and event).
 *
 * If the error flag is set by the CuEventActivity because subscribe_event failed, the poller is started
 * and the error *is not* notified to the listener(s)
 *
 */
void CuWSActionReader::onResult(const CuData &data)
{
    bool err = data["err"].toBool();
    bool a_exit = data["exit"].toBool(); // activity exit flag
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::set<CuDataListener *> lis_copy = d->listeners;
    std::set<CuDataListener *>::iterator it;
    bool event_subscribe_fail = err && !d->exit && data["event"].toString() == "subscribe";

    // if it's just subscribe_event failure, do not notify listeners
    for(it = lis_copy.begin(); it != lis_copy.end() && !event_subscribe_fail;   ++it) {
        (*it)->onUpdate(data);
    }

    if(err && !d->exit)
    {

    }

    /* remove last listener and delete this
     * - if d->exit is set to true (CuWSActionReader has been stop()ped )
     */
    if(d->exit && a_exit)
    {
        CuWSActionFactoryService * af = static_cast<CuWSActionFactoryService *>(d->cumbia_ws->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuWSActionFactoryService::CuWSActionFactoryServiceType)));
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
CuData CuWSActionReader::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("reader");
    return da;
}

/*! returns the *tango* source (TSource)
 *
 * @return a TSource object that describes the Tango source
 */
WSSource CuWSActionReader::getSource() const
{
    return d->tsrc;
}

CuWSActionI::Type CuWSActionReader::getType() const
{
    return CuWSActionI::Reader;
}

/** \brief Send data with parameters to configure the reader.
 *
 * @param data a CuData bundle with the settings to apply to the reader.
 *
 * \par Valid keys
 * \li "period": integer. Change the polling period, if the refresh mode is CuWSActionReader::PolledRefresh
 * \li "refresh_mode". A CuWSActionReader::RefreshMode value to change the current refresh mode.
 * \li "read" (value is irrelevant). If the read mode is CuWSActionReader::PolledRefresh, a read will be
 *     performed.
 *
 * @see getData
 *
 */
void CuWSActionReader::sendData(const CuData &data)
{
    printf("\e[1;35msendData sending %s\e[0m\n", data.toString().c_str());

    if(data.containsKey("refresh_mode"))
        d->refresh_mode = static_cast<CuWSActionReader::RefreshMode>(data["refresh_mode"].toInt());
    if(data.containsKey("period")) {
        int period2 = data["period"].toInt();

    }
}

/** \brief Get parameters from the reader.
 *
 * @param d_inout a reference to a CuData bundle containing the parameter names
 *        as keys. getData will associate the values to the keys.
 *        Unrecognized keys are ignored.
 *
 * \par Valid keys
 * \li "period": returns an int with the polling period
 * \li "refresh_mode": returns a CuWSActionReader::RefreshMode that can be converted to int
 * \li "mode": returns a string representation of the CuWSActionReader::RefreshMode
 *
 * @see sendData
 */
void CuWSActionReader::getData(CuData &d_inout) const
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
 * @param rm a value chosen from CuWSActionReader::RefreshMode.
 *
 *
 */
void CuWSActionReader::setRefreshMode(CuWSActionReader::RefreshMode rm)
{
    d->refresh_mode = rm;
}

string CuWSActionReader::refreshModeStr() const
{
    switch(d->refresh_mode)
    {
    case CuWSActionReader::RandomGenerator:
        return "RandomGenerator";
    default:
        return "InvalidRefreshMode";
    }
}

int CuWSActionReader::period() const
{
    return d->period;
}

CuWSActionReader::RefreshMode CuWSActionReader::refreshMode() const
{
    return d->refresh_mode;
}

void CuWSActionReader::setPeriod(int millis)
{
    d->period = millis;
}

/*
 * main thread
 */
void CuWSActionReader::stop()
{
//    if(d->exit)
//        d->log.write("CuWSActionReader.stop", CuLog::Error, CuLog::Read, "stop called twice for reader %s", this->getToken()["source"].toString().c_str());
//    else
    if(!d->exit)
    {
        d->exit = true;
        m_stopRandomGenActivity();
    }
}

void CuWSActionReader::addDataListener(CuDataListener *l)
{
    std::set<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuWSActionReader::removeDataListener(CuDataListener *l)
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

size_t CuWSActionReader::dataListenersCount()
{
    return d->listeners.size();
}

bool CuWSActionReader::exiting() const
{
    return d->exit;
}

/*! \brief returns true if the parameter is an event driven RefreshMode, false otherwise.
 *
 * @param rm a value picked from CuWSActionReader::RefreshMode enumeration
 * @return true if rm is ChangeEventRefresh, PeriodicEventRefresh or ArchiveEventRefresh,
 *         false otherwise
 *
 */
bool CuWSActionReader::isEventRefresh(CuWSActionReader::RefreshMode rm) const
{
    return false;
}

void CuWSActionReader::start()
{
     m_startRandomGenActivity();
}

void CuWSActionReader::m_startRandomGenActivity()
{
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["device"] = d->tsrc.getDeviceName();
    at["point"] = d->tsrc.getPoint();
    at["activity"] = "event";
    at["rmode"] = refreshModeStr();

    CuData tt("device", d->tsrc.getDeviceName()); /* thread token */
    d->randomgen_a = new CuRandomGenActivity(at);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_ws->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_ws->getThreadFactoryImpl());
    d->cumbia_ws->registerActivity(d->randomgen_a, this, tt, fi, bf);
    cuprintf("> CuWSActionReader.m_startEventActivity reader %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->randomgen_a);
}

void CuWSActionReader::m_stopRandomGenActivity()
{
    d->cumbia_ws->unregisterActivity(d->randomgen_a);
    d->randomgen_a = NULL; // not safe to dereference henceforth
}

