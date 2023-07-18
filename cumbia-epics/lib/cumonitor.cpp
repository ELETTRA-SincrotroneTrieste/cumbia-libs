#include "cumonitor.h"
#include "cumbiaepics.h"
#include "cuepactionfactoryservice.h"
#include "cuepcaservice.h"
#include "epsource.h"
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <list>
#include <algorithm>
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
    CuData property_d, value_d;
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
    pdelete("CuMonitor %p", this);
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
    std::list<CuDataListener *>::const_iterator it;
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::list<CuDataListener *> l_cp = d->listeners;
    for(it = l_cp.begin(); it != l_cp.end(); ++it) {
        (*it)->onUpdate(data);
    }

    /* remove last listener and delete this only if this result with the "exit" flag belongs to the current
     * activity, to avoid that old results, queued and delivered late, delete this before the current activity
     * posts its "exit" result
     */
    if(d->exit && data[CuDType::Exit].toBool() && data[CuDType::Activity] == d->current_activity->getToken()[CuDType::Activity])  // data["exit"], data["activity"]
    {
        CuEpicsActionFactoryService * af = static_cast<CuEpicsActionFactoryService *>(d->cumbia_e->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuEpicsActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuMonitor::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da[CuDType::Type] = std::string("reader");  // da["type"]
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
    if(data.containsKey(CuDType::RefreshMode))  // data.containsKey("refresh_mode")
        d->refresh_mode = static_cast<CuMonitor::RefreshMode>(data[CuDType::RefreshMode].toInt());  // data["refresh_mode"]
    if(data.containsKey(CuDType::Period))  // data.containsKey("period")
        d->period = data[CuDType::Period].toInt();  // data["period"]

    if(d->current_activity && data.containsKey(CuDType::RefreshMode))  // data.containsKey("refresh_mode")
        setRefreshMode(d->refresh_mode);
    if(d->current_activity && d->current_activity->getType() == CuMonitorActivity::CuMonitorActivityType)
        static_cast<CuMonitorActivity *>(d->current_activity)->setInterval(d->period);
    if(!d->current_activity)
        perr("CuMonitor.sendData: cannot send data without a running activity");
}

void CuMonitor::getData(CuData &d_inout) const
{
    if(d_inout.containsKey(CuDType::Period))  // d_inout.containsKey("period")
        d_inout[CuDType::Period] = d->period;  // d_inout["period"]
    if(d_inout.containsKey(CuDType::RefreshMode))  // d_inout.containsKey("refresh_mode")
        d_inout[CuDType::RefreshMode] = d->refresh_mode;  // d_inout["refresh_mode"]
    if(d_inout["cache"].toString() == std::string("property"))
        d_inout = d->property_d;
    else if(d_inout["cache"].toString() == std::string("value"))
        d_inout = d->value_d;
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
    m_startMonitorActivity();
}

/*
 * main thread
 */
void CuMonitor::stop()
{
    if(d->exit)
        d->log.write("CuMonitor.stop", CuLog::LevelError, CuLog::CategoryRead, "stop called twice for reader %s", this->getToken()["source"].toString().c_str());
    else
    { 
        d->exit = true;
        int t = d->current_activity->getType();
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
    d->listeners.erase(std::find(d->listeners.begin(), d->listeners.end(), l));
    if(d->listeners.size() == 0) {
        stop();
    }
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
    CuData at(CuDType::Src, d->tsrc.getName()); /* activity token */  // CuData at("src", d->tsrc.getName()
    at[CuDType::Pv] = d->tsrc.getPV();  // at["pv"]
    at[CuDType::Activity] = "monitor";  // at["activity"]
    at["is_pv"] = (d->tsrc.getType() == EpSource::PV);
    at[CuDType::Period] = d->period;  // at["period"]

    std::string tt("epics_monitor"); /* thread token */
    d->current_activity = new CuMonitorActivity(at, df, d->tsrc.getArgs());
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_e->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_e->getThreadFactoryImpl());
    d->cumbia_e->registerActivity(d->current_activity, this, tt, fi, bf);
}



bool CuMonitor::exiting() const
{
    return d->exit;
}


void CuMonitor::onResult(const std::vector<CuData> &datalist)
{
}
