#include "cuepconfiguration.h"
#include <culog.h>
#include <cumbiaepics.h>
#include <cumacros.h>
#include <cuserviceprovider.h>
#include "epsource.h"
#include <cuactivity.h>
#include "cuepcaservice.h"
#include "cuepconfigactivity.h"
#include "cuepactionfactoryservice.h"
#include "cudatalistener.h"

class CuEpConfigurationPrivate
{
public:
    std::list<CuDataListener *> listeners;
    EpSource tsrc;
    CumbiaEpics *cumbia_epics;
    CuActivity *activity;
    bool exit;
    CuConLogImpl li;
    CuLog log;
    CuVariant write_val;
    std::vector<std::string> desired_props;
    CuData conf_data;
};

CuEpConfiguration::CuEpConfiguration(const EpSource &src,
                                     CumbiaEpics *ct)
{
    d = new CuEpConfigurationPrivate;
    d->cumbia_epics = ct;
    d->tsrc = src;
    d->exit = false;
}

CuEpConfiguration::~CuEpConfiguration()
{
    pdelete("CuEpConfiguration %p", this);
    delete d;
}

void CuEpConfiguration::setDesiredPVProperties(const std::vector<string> props)
{
    d->desired_props = props;
}

void CuEpConfiguration::onProgress(int step, int total, const CuData &data)
{
    (void) step; // unused
    (void) total;
    (void) data;
}

void CuEpConfiguration::onResult(const CuData &data)
{
    d->conf_data = data;
    d->exit = data["exit"].toBool();
    if(!d->exit)
    {
        std::list<CuDataListener *>::iterator it;
        for(it = d->listeners.begin(); it != d->listeners.end(); ++it)
            (*it)->onUpdate(data);
    }
    else
    {
        CuEpicsActionFactoryService * af = static_cast<CuEpicsActionFactoryService *>(d->cumbia_epics->getServiceProvider()
                                                                                      ->get(static_cast<CuServices::Type>(CuEpicsActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuEpConfiguration::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("attconfig");
    return da;
}

EpSource CuEpConfiguration::getSource() const
{
    return d->tsrc;
}

CuEpicsActionI::Type CuEpConfiguration::getType() const
{
    return CuEpicsActionI::PropConfig;
}

void CuEpConfiguration::addDataListener(CuDataListener *l)
{
    std::list<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
    /* if a new listener is added after onResult, call onUpdate.
     * This happens when multiple items connect to the same source.
     * Post the result, so that it's delivered later.
     */
    if(!d->conf_data.isEmpty()) {
        d->activity->publishResult(d->conf_data);
    }
}

void CuEpConfiguration::removeDataListener(CuDataListener *l) {
    if(d->listeners.size() > 0)
        d->listeners.remove(l);
    if(d->listeners.size() == 0)
        stop();
}

void CuEpConfiguration::sendData(const CuData &) {
}

void CuEpConfiguration::getData(CuData &d_inout) const {
    (void) d_inout;
}

size_t CuEpConfiguration::dataListenersCount() {
    return d->listeners.size();
}

void CuEpConfiguration::start() {
    CuEpCAService *df =
            static_cast<CuEpCAService *>(d->cumbia_epics->getServiceProvider()->
                                         get(static_cast<CuServices::Type> (CuEpCAService::CuEpicsChannelAccessServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["pv"] = d->tsrc.getPV();
    at["activity"] = "attconfig";
    at["is_pv"] = d->tsrc.getType() == EpSource::PV;
    CuData tt("pv", d->tsrc.getPV()); /* thread token */
    d->activity = new CuEpConfigActivity(at, df);
    static_cast<CuEpConfigActivity *>(d->activity)->setDesiredAttributeProperties(d->desired_props);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_epics->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_epics->getThreadFactoryImpl());
    d->cumbia_epics->registerActivity(d->activity, this, tt, fi, bf);
}

void CuEpConfiguration::stop() {
    d->exit = true;
}

bool CuEpConfiguration::exiting() const {
    return d->exit;
}

void CuEpConfiguration::onResult(const std::vector<CuData> &datalist) {
    (void) datalist;
}
