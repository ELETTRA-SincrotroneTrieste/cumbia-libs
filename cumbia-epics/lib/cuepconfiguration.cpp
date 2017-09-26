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

class CuTAttConfigurationPrivate
{
public:
    std::list<CuDataListener *> listeners;
    EpSource tsrc;
    CumbiaEpics *cumbia_t;
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
    d = new CuTAttConfigurationPrivate;
    d->cumbia_t = ct;
    d->tsrc = src;
}

CuEpConfiguration::~CuEpConfiguration()
{
    printf("\e[1;31m~~~~~~~~~~~~~~~~~ DELETING CuTattCnfguration %p\n\n\e[0m\n", this);
    delete d;
}

void CuEpConfiguration::setDesiredAttributeProperties(const std::vector<string> props)
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
    bool exit = data["exit"].toBool();
    if(!exit)
    {
        std::list<CuDataListener *>::iterator it;
        for(it = d->listeners.begin(); it != d->listeners.end(); ++it)
        {
            printf("CuTAttConfiguration::onResult: calling on update on %p\n\n", (*it));
            (*it)->onUpdate(data);
        }
    }
    else
    {
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
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
    return CuEpicsActionI::AttConfig;
}

void CuEpConfiguration::addDataListener(CuDataListener *l)
{
    printf("\e[1;35mCuTAttConfiguration.addDataListener: adding %p to this %p \e[0m\n", l, this);
    std::list<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
    /* if a new listener is added after onResult, call onUpdate.
     * This happens when multiple items connect to the same source
     */
    if(!d->conf_data.isEmpty())
        l->onUpdate(d->conf_data);
}

void CuEpConfiguration::removeDataListener(CuDataListener *l)
{
    if(d->listeners.size() > 0)
        d->listeners.remove(l);
    else
        stop();
}

void CuEpConfiguration::sendData(const CuData &)
{
}

void CuEpConfiguration::getData(CuData &d_inout) const
{

}

size_t CuEpConfiguration::dataListenersCount()
{
    return d->listeners.size();
}

void CuEpConfiguration::start()
{
    CuEpCAService *df =
            static_cast<CuEpCAService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuEpCAService::CuEpicsChannelAccessServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["ioc"] = d->tsrc.getIOC();
    at["pv"] = d->tsrc.getPV();
    at["activity"] = "attconfig";
    at["is_pv"] = d->tsrc.getType() == EpSource::PV;

    CuData tt("ioc", d->tsrc.getIOC()); /* thread token */
    d->activity = new CuEpConfigActivity(at, df);
    static_cast<CuEpConfigActivity *>(d->activity)->setDesiredAttributeProperties(d->desired_props);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, tt, fi, bf);
    cuprintf("> CuTWriter.start writer %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->activity);
}

void CuEpConfiguration::stop()
{

}
