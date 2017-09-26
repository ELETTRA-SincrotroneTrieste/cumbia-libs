#include "cutattconfiguration.h"
#include <culog.h>
#include <cumbiatango.h>
#include <cumacros.h>
#include <cuserviceprovider.h>
#include "tsource.h"
#include <cuactivity.h>
#include "cudevicefactoryservice.h"
#include "cutattconfigactivity.h"
#include "cuactionfactoryservice.h"
#include "cudatalistener.h"

class CuTAttConfigurationPrivate
{
public:
    std::list<CuDataListener *> listeners;
    TSource tsrc;
    CumbiaTango *cumbia_t;
    CuActivity *activity;
    bool exit;
    CuConLogImpl li;
    CuLog log;
    CuVariant write_val;
    std::vector<std::string> desired_props;
    CuData conf_data;
    bool iter_lock;
};

CuTAttConfiguration::CuTAttConfiguration(const TSource& src,
                                         CumbiaTango *ct)
{
    d = new CuTAttConfigurationPrivate;
    d->cumbia_t = ct;
    d->tsrc = src;
    d->exit = false;
    d->iter_lock = false;
}

CuTAttConfiguration::~CuTAttConfiguration()
{
    printf("\e[1;31m~~~~~~~~~~~~~~~~~ DELETING CuTattCnfguration %p\n\n\e[0m\n", this);
    delete d;
}

void CuTAttConfiguration::setDesiredAttributeProperties(const std::vector<string> props)
{
    d->desired_props = props;
}

void CuTAttConfiguration::onProgress(int step, int total, const CuData &data)
{
    (void) step; // unused
    (void) total;
    (void) data;
}

void CuTAttConfiguration::onResult(const CuData &data)
{
    d->conf_data = data;
    d->exit = data["exit"].toBool();
    if(!d->exit)
    {
        // iterator can be invalidated if listener's onUpdate wants to unset source
        std::list <CuDataListener *> listeners = d->listeners;
        std::list<CuDataListener *>::iterator it;
        printf("1. CuTAttConfiguration::onResult: this is %p calling on update there are %d listeners\e[0m\n",
               this, d->listeners.size());
        d->iter_lock = true;
        for(it = listeners.begin(); it != listeners.end(); ++it)
        {
            printf("2. CuTAttConfiguration::onResult: this is %p calling on update on %p this thread 0x%lx\n", this, (*it), pthread_self());
            (*it)->onUpdate(data);
        }
        d->iter_lock = false;
    }
    else
    {
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        printf("\e[1;31mCuTAttConfiguration %p - EXITING \e[0m\n", this);
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuTAttConfiguration::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("attconfig");
    return da;
}

TSource CuTAttConfiguration::getSource() const
{
    return d->tsrc;
}

CuTangoActionI::Type CuTAttConfiguration::getType() const
{
    return CuTangoActionI::AttConfig;
}

void CuTAttConfiguration::addDataListener(CuDataListener *l)
{
    printf("\e[1;35mCuTAttConfiguration.addDataListener: adding %p to this %p \e[0m\n", l, this);
    if(d->iter_lock)
        perr("CuTAttConfiguration::addDataListener not permitted here");
    std::list<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
    /* if a new listener is added after onResult, call onUpdate.
     * This happens when multiple items connect to the same source
     */
    if(!d->conf_data.isEmpty())
        l->onUpdate(d->conf_data);
    printf("\e[1;35mCuTAttConfiguration.addDataListener: added %p to this %p thre are %d listeners\e[0m\n",
           l, this, d->listeners.size());
}

void CuTAttConfiguration::removeDataListener(CuDataListener *l)
{
    if(d->iter_lock)
        perr("CuTAttConfiguration::removeDataListener not permitted here");
    d->listeners.remove(l);
    printf("\e[1;35;3mCuTAttConfiguration::removeDataListener removed listener %p from this %p size now is %d this thread 0x%lx\e[0m\n",
        l, this, d->listeners.size(), pthread_self());
    if(!d->listeners.size())
        stop();
}

void CuTAttConfiguration::sendData(const CuData &)
{
}

void CuTAttConfiguration::getData(CuData &d_inout) const
{

}

size_t CuTAttConfiguration::dataListenersCount()
{
    return d->listeners.size();
}

void CuTAttConfiguration::start()
{
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));

    CuData at("src", d->tsrc.getName()); /* activity token */
    at["device"] = d->tsrc.getDeviceName();
    at["point"] = d->tsrc.getPoint();
    at["activity"] = "attconfig";
    at["is_command"] = d->tsrc.getType() == TSource::Cmd;

    CuData tt("device", d->tsrc.getDeviceName()); /* thread token */
    d->activity = new CuTAttConfigActivity(at, df);
    static_cast<CuTAttConfigActivity *>(d->activity)->setDesiredAttributeProperties(d->desired_props);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, tt, fi, bf);
    cuprintf("> CuTWriter.start writer %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->activity);
}

void CuTAttConfiguration::stop()
{
    d->exit = true;
}

bool CuTAttConfiguration::stopping() const
{
    return d->exit;
}
