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
    printf("\e[1;31m~~~~~~~~~~~~~~~~~ DELETING CuEpConfiguration %p\n\n\e[0m\n", this);
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
    d->exit = data[CuDType::Exit].toBool();
    if(!d->exit)
    {
        std::list<CuDataListener *>::iterator it;
        for(it = d->listeners.begin(); it != d->listeners.end(); ++it)
        {
            printf("CuEpConfiguration::onResult: calling on update on %p\n\n", (*it));
            (*it)->onUpdate(data);
        }
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
    da[CuDType::Type] = std::string("attconfig");
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
    printf("\e[1;35mCuEpConfiguration.addDataListener: adding %p to this %p \e[0m\n", l, this);
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
            static_cast<CuEpCAService *>(d->cumbia_epics->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuEpCAService::CuEpicsChannelAccessServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    at[CuXDType::Pv = d->tsrc.getPV();
    at[CuDType::Activity] = "attconfig";
    at["is_pv"] = d->tsrc.getType() == EpSource::PV;

    CuData tt("pv", d->tsrc.getPV()); /* thread token */
    d->activity = new CuEpConfigActivity(at, df);
    static_cast<CuEpConfigActivity *>(d->activity)->setDesiredAttributeProperties(d->desired_props);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_epics->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_epics->getThreadFactoryImpl());
    d->cumbia_epics->registerActivity(d->activity, this, tt, fi, bf);
    cuprintf("> CuEpConfiguration.start this %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->activity);
}

void CuEpConfiguration::stop()
{
    d->exit = true;
}

bool CuEpConfiguration::exiting() const
{
    return d->exit;
}
