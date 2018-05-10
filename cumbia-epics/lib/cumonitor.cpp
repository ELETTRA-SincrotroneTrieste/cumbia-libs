#include "cumonitor.h"
#include "cumbiaepics.h"
#include "cuepactionfactoryservice.h"
#include "cuepcaservice.h"
#include "epsource.h"
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <list>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include "cumonitoractivity.h"
#include <culog.h>

#include <cadef.h>

class EpSource;

class CuMonitorPrivate
{
public:
    std::list<CuDataListener *> listeners;
    EpSource tsrc;
    CumbiaEpics *cumbia_e;
    CuActivity *current_activity;
    bool exit;
    CuConLogImpl li;
    CuLog log;

    int period;
    CuMonitor::RefreshMode refresh_mode;
};

CuMonitor::CuMonitor(const EpSource& src, CumbiaEpics *ct) : CuEpicsActionI()
{
    d = new CuMonitorPrivate;
    d->tsrc = src;
    d->cumbia_e = ct;
    d->current_activity = NULL;
    d->exit = false;
    d->log = CuLog(&d->li);
    d->period = 1000;
    d->refresh_mode = MonitorRefresh;
}

CuMonitor::~CuMonitor()
{
    pdelete("~CuMonitor deleting %p", this);
    delete d;
}

void CuMonitor::onProgress(int step, int total, const CuData &data)
{

}

/*
 * this is invoked in main thread
 */
void CuMonitor::onResult(const CuData &data)
{
    printf("CuMonitor.onResult: data received %s THREAD 0x%lx\n", data.toString().c_str(), pthread_self());
    std::list<CuDataListener *>::iterator it;
    for(it = d->listeners.begin(); it != d->listeners.end(); ++it)
        (*it)->onUpdate(data);


    /* remove last listener and delete this only if this result with the "exit" flag belongs to the current
     * activity, to avoid that old results, queued and delivered late, delete this before the current activity
     * posts its "exit" result
     */
    if(d->exit && data["exit"].toBool() && data["activity"] == d->current_activity->getToken()["activity"])
    {
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_e->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuMonitor::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("reader");
    return da;
}

EpSource CuMonitor::getSource() const
{
    return d->tsrc;
}

CuEpicsActionI::Type CuMonitor::getType() const
{
    return CuEpicsActionI::Reader;
}

void CuMonitor::sendData(const CuData &data)
{
    if(data.containsKey("refresh_mode"))
        d->refresh_mode = static_cast<CuMonitor::RefreshMode>(data["refresh_mode"].toInt());
    if(data.containsKey("period"))
        d->period = data["period"].toInt();

    if(d->current_activity && data.containsKey("refresh_mode"))
        setRefreshMode(d->refresh_mode);
    if(d->current_activity && d->current_activity->getType() == CuMonitorActivity::CuMonitorActivityType)
        static_cast<CuMonitorActivity *>(d->current_activity)->setInterval(d->period);
    if(!d->current_activity)
        perr("CuMonitor.sendData: cannot send data without a running activity");
}

void CuMonitor::getData(CuData &d_inout) const
{
    if(d_inout.containsKey("period"))
        d_inout["period"] = d->period;
    if(d_inout.containsKey("refresh_mode"))
        d_inout["refresh_mode"] = d->refresh_mode;
}

void CuMonitor::setRefreshMode(CuMonitor::RefreshMode rm)
{
    cuprintf("CuMonitor.setRefreshMode -> %d -- unimplemented\n", rm);
}

int CuMonitor::period() const
{
    return d->period;
}

CuMonitor::RefreshMode CuMonitor::refreshMode() const
{
    return d->refresh_mode;
}

void CuMonitor::setPeriod(int millis)
{
    d->period = millis;
}

void CuMonitor::start()
{
    pr_thread();
    printf("\e[2;33mCumonitor.start()m_startMonitorActivity \e[0m\n ");
    m_startMonitorActivity();
}

/*
 * main thread
 */
void CuMonitor::stop()
{
    if(d->exit)
        d->log.write("CuMonitor.stop", CuLog::Error, CuLog::Read, "stop called twice for reader %s", this->getToken()["source"].toString().c_str());
    else
    { 
        d->exit = true;
        int t = d->current_activity->getType();
        cuprintf("\e[1;35mCuMonitor.stop(): unregistering activity %p current type %d looking for %d\n",
                 d->current_activity, t, CuMonitorActivity::CuMonitorActivityType);
        if(t == CuMonitorActivity::CuMonitorActivityType)
            d->cumbia_e->unregisterActivity(d->current_activity);
    }
}

void CuMonitor::addDataListener(CuDataListener *l)
{
    std::list<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuMonitor::removeDataListener(CuDataListener *l)
{
    d->listeners.remove(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuMonitor::dataListenersCount()
{
    return d->listeners.size();
}

void CuMonitor::m_startMonitorActivity()
{
    CuEpCAService *df =
            static_cast<CuEpCAService *>(d->cumbia_e->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuEpCAService::CuEpicsChannelAccessServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["ioc"] = d->tsrc.getIOC();
    at["pv"] = d->tsrc.getPV();
    at["activity"] = "monitor";
    at["is_pv"] = (d->tsrc.getType() == EpSource::PV);
    at["period"] = d->period;

    CuData tt("activity_thread", "epics_monitor"); /* thread token */
    d->current_activity = new CuMonitorActivity(at, df, d->tsrc.getArgs());
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_e->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_e->getThreadFactoryImpl());
    d->cumbia_e->registerActivity(d->current_activity, this, tt, fi, bf);
    cuprintf("> CuMonitor.m_startMonitorActivity reader %p thread 0x%lx ACTIVITY %p == \e[0;32mSTARTING MONITOR FLAGS %d\e[0m\n\n",
             this, pthread_self(), d->current_activity, d->current_activity->getFlags());
}

