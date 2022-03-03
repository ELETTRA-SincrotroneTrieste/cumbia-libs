#include "cutconfiguration.h"
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
#include "cutconfigactivity_executor_i.h"

class CuTAttConfigurationPrivate
{
public:
    CuTAttConfigurationPrivate(const TSource& t_src,
                               CumbiaTango *ct,
                               CuTangoActionI::Type t,
                               const CuData& op,
                               const CuData& ta,
                               const CuTConfigActivityExecutor_I* cx) :
        tsrc(t_src), cumbia_t(ct), activity(nullptr), type(t), options(op), tag(ta), c_xecutor(cx) {
    }

    std::set<CuDataListener *> listeners;
    const TSource tsrc;
    CumbiaTango *cumbia_t;
    CuTConfigActivity *activity;
    bool exiting; // set to true by stop()
    const CuTangoActionI::Type type;
    CuData options, tag;
    const CuTConfigActivityExecutor_I *c_xecutor;
};

CuTConfiguration::CuTConfiguration(const TSource& src,
                                   CumbiaTango *ct,
                                   CuTangoActionI::Type t,
                                   const CuData& options,
                                   const CuData& tag,
                                   const CuTConfigActivityExecutor_I* cx) {
    d = new CuTAttConfigurationPrivate(src, ct, t, options, tag,
                                       cx != nullptr ? cx : new CuTConfigActivityExecutor_Default); // src, t are const
    d->exiting = false;
}

CuTConfiguration::~CuTConfiguration() {
    pdelete("~CuTConfiguration: %p [%s]", this, d->tsrc.getName().c_str());
    delete d; // d->c_xecutor deleted by activity
}

void CuTConfiguration::setDesiredAttributeProperties(const std::vector<string> props) {
    d->options["fetch_props"] = props;
}

void CuTConfiguration::setOptions(const CuData &options) {
    d->options = options;
}

void CuTConfiguration::setTag(const CuData &tag) {
    d->tag = tag;
}

void CuTConfiguration::onProgress(int , int , const CuData &) { }

void CuTConfiguration::onResult(const CuData &data) {
    // activity publishes only once from execute
    // we can clean everything after listeners update
    std::set<CuDataListener *>::iterator it;
    // need a local copy of d->listeners because the iterator may be invalidated by
    // removeDataListener in this same thread by the client from onUpdate
    std::set<CuDataListener *> ls = d->listeners;
    for(it =  ls.begin(); it !=  ls.end(); ++it) {
        (*it)->onUpdate(data);
    }
    d->exiting = true; // for action factory to unregisterAction, exiting must return true
    CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                       ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
    af->unregisterAction(d->tsrc.getName(), getType());
    d->listeners.clear();
    delete this;
}

/*! \brief unused. Complies with CuThreadListener interface
 *
 */
void CuTConfiguration::onResult(const std::vector<CuData> &datalist) {
    (void) datalist;
}

CuData CuTConfiguration::getToken() const {
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("property");
    return da;
}

TSource CuTConfiguration::getSource() const {
    return d->tsrc;
}

CuTangoActionI::Type CuTConfiguration::getType() const {
    return d->type;
}

void CuTConfiguration::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuTConfiguration::removeDataListener(CuDataListener *l) {
    d->listeners.erase(l);
    if(!d->listeners.size())
        stop();
}

void CuTConfiguration::sendData(const CuData &) {
}

void CuTConfiguration::getData(CuData &d_inout) const { (void) d_inout; }

size_t CuTConfiguration::dataListenersCount() {
    return d->listeners.size();
}

void CuTConfiguration::start() {
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));
    const std::string tt =d->options.containsKey("thread_token") ? // d->tsrc will be moved: no string reference
                d->options.s("thread_token") : d->tsrc.getDeviceName();
    CuTConfigActivity::Type t;
    d->type == CuTangoActionI::ReaderConfig ? t = CuTConfigActivity::CuReaderConfigActivityType :
            t = CuTConfigActivity::CuWriterConfigActivityType;
    d->activity =  new CuTConfigActivity(std::move(d->tsrc), df, t, d->c_xecutor, std::move(d->options), std::move(d->tag));
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

bool CuTConfiguration::is_running() const {
    return d->activity != nullptr;
}
