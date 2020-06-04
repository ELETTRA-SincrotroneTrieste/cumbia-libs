#include "cutconfiguration.h"
#include <culog.h>
#include <cumacros.h>
#include <cuserviceprovider.h>
#include <cuactivity.h>
#include <cuevent.h>
#include <cudatalistener.h>
#include <set>
#include "cumbiatango.h"
#include "tsource.h"
#include "cudevicefactoryservice.h"
#include "cutconfigactivity.h"
#include "cuactionfactoryservice.h"
#include "cudatalistener.h"


class CuTAttConfigurationPrivate
{
public:
    CuTAttConfigurationPrivate(const TSource& t_src, CuTangoActionI::Type t) :
        tsrc(t_src), type(t) {
    }

    std::set<CuDataListener *> listeners;
    const TSource tsrc;
    CumbiaTango *cumbia_t;
    CuTConfigActivity *activity;
    bool exiting; // set to true by stop()
    CuConLogImpl li;
    CuLog log;
    CuVariant write_val;
    CuData conf_data; // save locally
    CuData options;
    const CuTangoActionI::Type type;
};

CuTConfiguration::CuTConfiguration(const TSource& src,
                                   CumbiaTango *ct,
                                   CuTangoActionI::Type t)
{
    d = new CuTAttConfigurationPrivate(src, t); // src, t are const
    d->cumbia_t = ct;
    d->exiting = false;
}

CuTConfiguration::~CuTConfiguration()
{
    printf("~CuTConfiguration: %p [%s]\n", this, d->tsrc.getName().c_str());
    delete d;
}

void CuTConfiguration::setDesiredAttributeProperties(const std::vector<string> props) {
    d->options["fetch_props"] = props;
}

void CuTConfiguration::setOptions(const CuData &options) {
    d->options = options;
}

void CuTConfiguration::onProgress(int step, int total, const CuData &data)
{
    (void) step; // unused
    (void) total;
    (void) data;
}

void CuTConfiguration::onResult(const CuData &data)
{
    d->conf_data = data;
    if(data["exit"].toBool()) // ! important: evaluate data["exit"] before deleting this
    {
        printf("CuTConfiguration::onResult %p [%s] exiting\n", this, d->tsrc.getName().c_str());
        d->exiting = true; // for action factory to unregisterAction, exiting must return true
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
    else { // do not update configuration data if exit
        std::set<CuDataListener *> listeners = d->listeners;
        std::set<CuDataListener *>::iterator it;
        for(it = listeners.begin(); it != listeners.end(); ++it) {
            (*it)->onUpdate(data);
        }
    }
}

/*! \brief unused. Complies with CuThreadListener interface
 *
 */
void CuTConfiguration::onResult(const std::vector<CuData> &datalist)
{
    (void) datalist;
}

CuData CuTConfiguration::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("attconfig");
    return da;
}

TSource CuTConfiguration::getSource() const
{
    return d->tsrc;
}

CuTangoActionI::Type CuTConfiguration::getType() const
{
    return d->type;
}

void CuTConfiguration::addDataListener(CuDataListener *l)
{
    printf("CuTConfiguration.addDataListener %p [%s] - size before %ld ", l, d->tsrc.getName().c_str(), d->listeners.size());
    d->listeners.insert(l);
    printf("after %ld\n", d->listeners.size());
    l->setValid();
    /* if a new listener is added after onResult, call onUpdate.
     * This happens when multiple items connect to the same source
     * Post the result, so that it is delivered later.
     */
    if(!d->conf_data.isEmpty()) {
        d->activity->publishResult(d->conf_data);
    }
}

void CuTConfiguration::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(!d->listeners.size())
        stop();
}

void CuTConfiguration::sendData(const CuData &) {
}

void CuTConfiguration::getData(CuData &d_inout) const {

}

size_t CuTConfiguration::dataListenersCount() {
    return d->listeners.size();
}

void CuTConfiguration::start()
{
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["device"] = d->tsrc.getDeviceName();
    at["point"] = d->tsrc.getPoint();
    at["argins"] = d->tsrc.getArgs();
    at["activity"] = "attconfig";
    at["is_command"] = d->tsrc.getType() == TSource::SrcCmd;
    at.merge(d->options);

    CuData tt = CuData("device", d->tsrc.getDeviceName());
    if(d->options.containsKey("thread_token"))
        tt["thread_token"] = d->options["thread_token"];
    CuTConfigActivity::Type t;
    d->type == CuTangoActionI::ReaderConfig ? t = CuTConfigActivity::CuReaderConfigActivityType :
            t = CuTConfigActivity::CuWriterConfigActivityType;
    d->activity = new CuTConfigActivity(at, df, t);
    d->activity->setOptions(d->options);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, tt, fi, bf);
}

/*! \brief calls unregisterActivity on Cumbia in order to enter the exit state
 *
 * - sets the exiting flag to true
 * - calls Cumbia::unregisterActivity
 */
void CuTConfiguration::stop()
{
    if(!d->exiting) {
        d->exiting = true;
        d->cumbia_t->unregisterActivity(d->activity);
    }
}

/*! \brief CuActionFactory relies on this returning true to unregister the action
 */
bool CuTConfiguration::exiting() const
{
    return d->exiting;
}
