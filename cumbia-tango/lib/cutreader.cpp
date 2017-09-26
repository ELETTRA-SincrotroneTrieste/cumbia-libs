#include "cutreader.h"
#include "cumbiatango.h"
#include "tdevice.h"
#include "cudevicefactoryservice.h"
#include "cuactionfactoryservice.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <list>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include "cueventactivity.h"
#include "cupollingactivity.h"
#include <culog.h>

#include <tango.h>

class TSource;

class CuTReaderPrivate
{
public:
    std::list<CuDataListener *> listeners;
    TSource tsrc;
    CumbiaTango *cumbia_t;
    CuActivity *current_activity;
    bool exit;
    CuConLogImpl li;
    CuLog log;

    int period;
    CuTReader::RefreshMode refresh_mode;
};

CuTReader::CuTReader(const TSource& src,
                     CumbiaTango *ct)
{
    d = new CuTReaderPrivate;
    d->tsrc = src;
    d->cumbia_t = ct;
    d->current_activity = NULL;
    d->exit = false;
    d->log = CuLog(&d->li);
    d->period = 1000;
    d->refresh_mode = ChangeEventRefresh;
}

CuTReader::~CuTReader()
{
    printf("~CuTReader deleting %p", this);
    delete d;
}

void CuTReader::onProgress(int step, int total, const CuData &data)
{

}

/*
 * this is invoked in main thread
 */
void CuTReader::onResult(const CuData &data)
{
    bool polling_fallback = false;
    printf("CuTReader %p onResult: data received %s listeners size %d\n", this, data.toString().c_str(), d->listeners.size());
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::list<CuDataListener *> lis_copy = d->listeners;
    std::list<CuDataListener *>::iterator it;
    for(it = lis_copy.begin(); it != lis_copy.end(); ++it)
    {
        printf("CuTReader::onResult: calling on update on %p this thread 0x%lx\n", (*it), pthread_self());
        (*it)->onUpdate(data);
    }
    if(!d->exit && data["activity"].toString() == "event" && data["err"].toBool()
            && d->current_activity->getType() == CuEventActivity::CuEventActivityType )
    {
        polling_fallback = true;
        printf("starting polling activity cuz event is err %d\n", data["err"].toBool());
        /* stop event activity forever */
        d->cumbia_t->unregisterActivity(d->current_activity);
        m_startPollingActivity(polling_fallback);
    }

    /* remove last listener and delete this only if this result with the "exit" flag belongs to the current
     * activity, to avoid that old results, queued and delivered late, delete this before the current activity
     * posts its "exit" result
     */
    if(d->exit && data["exit"].toBool() && data["activity"] == d->current_activity->getToken()["activity"])
    {
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuTReader::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("reader");
    return da;
}

TSource CuTReader::getSource() const
{
    return d->tsrc;
}

CuTangoActionI::Type CuTReader::getType() const
{
    return CuTangoActionI::Reader;
}

void CuTReader::sendData(const CuData &data)
{
    if(data.containsKey("refresh_mode"))
        d->refresh_mode = static_cast<CuTReader::RefreshMode>(data["refresh_mode"].toInt());
    if(data.containsKey("period"))
        d->period = data["period"].toInt();

    if(d->current_activity && data.containsKey("refresh_mode"))
        setRefreshMode(d->refresh_mode);
    if(d->current_activity && d->current_activity->getType() == CuPollingActivity::CuPollingActivityType)
        static_cast<CuPollingActivity *>(d->current_activity)->setInterval(d->period);
    if(!d->current_activity)
        perr("CuTReader.sendData: cannot send data without a running activity");
}

void CuTReader::getData(CuData &d_inout) const
{
    if(d_inout.containsKey("period"))
        d_inout["period"] = d->period;
    if(d_inout.containsKey("refresh_mode"))
        d_inout["refresh_mode"] = d->refresh_mode;
}

void CuTReader::setRefreshMode(CuTReader::RefreshMode rm)
{
    cuprintf("CuTReader.setRefreshMode -> %d\n", rm);
    d->refresh_mode = rm;
    if(d->current_activity && rm == CuTReader::ChangeEventRefresh &&
            d->current_activity->getType() == CuPollingActivity::CuPollingActivityType)
    {
        cuprintf("CuTReader.setRefreshMode: changing from event to polling...\n");
        d->cumbia_t->unregisterActivity(d->current_activity);
        m_startEventActivity();
    }
    else if(d->current_activity && rm == CuTReader::PolledRefresh &&
            d->current_activity->getType() == CuEventActivity::CuEventActivityType)
    {
        cuprintf("CuTReader.setRefreshMode: changing from polling to event mode...\n");
        d->cumbia_t->unregisterActivity(d->current_activity);
        m_startPollingActivity(false);
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

void CuTReader::start()
{
    pr_thread();
    if(d->refresh_mode == ChangeEventRefresh)
        m_startEventActivity();
    else
        m_startPollingActivity(false);
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
        cuprintf("\e[1;35mCuTReader.stop(): unregistering activity %p\n", d->current_activity);
        d->exit = true;
        int t = d->current_activity->getType();
        if(t == CuEventActivity::CuEventActivityType || t == CuPollingActivity::CuPollingActivityType)
            d->cumbia_t->unregisterActivity(d->current_activity);
    }
}

void CuTReader::addDataListener(CuDataListener *l)
{
    std::list<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuTReader::removeDataListener(CuDataListener *l)
{
    d->listeners.remove(l);
    if(d->listeners.size() == 0)
       stop();
}

size_t CuTReader::dataListenersCount()
{
    return d->listeners.size();
}

bool CuTReader::stopping() const
{
    return d->exit;
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

    CuData tt("device", d->tsrc.getDeviceName()); /* thread token */
    d->current_activity = new CuEventActivity(at, df);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->current_activity, this, tt, fi, bf);
    cuprintf("> CuTReader.m_startEventActivity reader %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->current_activity);
}

void CuTReader::m_startPollingActivity(bool fallback)
{
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["device"] = d->tsrc.getDeviceName();
    at["point"] = d->tsrc.getPoint();
    at["activity"] = "poller";
    at["fallback"] = fallback;
    at["is_command"] = (d->tsrc.getType() == TSource::Cmd);
    at["period"] = d->period;

    CuData tt("device", d->tsrc.getDeviceName()); /* thread token */
    d->current_activity = new CuPollingActivity(at, df, d->tsrc.getArgs());
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->current_activity, this, tt, fi, bf);
    cuprintf("> CuTReader.m_startPollingActivity reader %p thread 0x%lx ACTIVITY %p == \e[0;32mSTARTING POLLING\e[0m\n\n", this, pthread_self(), d->current_activity);
}


