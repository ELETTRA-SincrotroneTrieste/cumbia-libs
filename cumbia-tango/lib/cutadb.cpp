#include "cutadb.h"
#include <culog.h>
#include <cumacros.h>
#include <cuserviceprovider.h>
#include <cuactivity.h>
#include <cuevent.h>
#include <cudatalistener.h>
#include "cumbiatango.h"
#include "tsource.h"
#include "cudevicefactoryservice.h"
#include "cutadbactivity.h"
#include "cuactionfactoryservice.h"
#include "cudatalistener.h"


class CuTaDbPrivate
{
public:
    CuTaDbPrivate(const TSource& t_src) : tsrc(t_src){
    }

    std::list<CuDataListener *> listeners;
    const TSource tsrc;
    CumbiaTango *cumbia_t;
    CuTaDbActivity *activity;
    bool xit; // set to true by stop()
    CuData cached_data; // save locally
    CuData options;
};

CuTaDb::CuTaDb(const TSource& src, CumbiaTango *ct) {
    d = new CuTaDbPrivate(src); // src, t are const
    d->cumbia_t = ct;
    d->xit = false;
}

CuTaDb::~CuTaDb()
{
    pdelete("~CuTaDb: %p", this);
    delete d;
}

void CuTaDb::setOptions(const CuData &options) {
    d->options = options;
}

void CuTaDb::onProgress(int step, int total, const CuData &data) {
    (void) step; // unused
    (void) total;
    (void) data;
}

void CuTaDb::onResult(const CuData &data)
{
    d->cached_data = data;
    cuprintf("CuTaDb.onResult %s exit? %d listeners %ld\n", data["src"].toString().c_str(), data["exit"].toBool(), d->listeners.size());

    if(data["exit"].toBool()) // ! important: evaluate data["exit"] before deleting this
    {
        d->xit = true; // for action factory to unregisterAction, exiting must return true
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
    else { // do not update configuration data if exit
        std::list <CuDataListener *> listeners = d->listeners;
        std::list<CuDataListener *>::iterator it;

        for(it = listeners.begin(); it != listeners.end(); ++it) {
            cuprintf("CuTaDb.onResult %s exit? %d listeners %ld onUpdate on %p\n", data["src"].toString().c_str(), data["exit"].toBool(), d->listeners.size(), (*it));
            (*it)->onUpdate(data);
        }
    }
}

/*! \brief unused. Complies with CuThreadListener interface
 *
 */
void CuTaDb::onResult(const std::vector<CuData> &datalist)
{
    (void) datalist;
}

CuData CuTaDb::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["action"] = std::string("cutadb");
    return da;
}

TSource CuTaDb::getSource() const
{
    return d->tsrc;
}

CuTangoActionI::Type CuTaDb::getType() const
{
    return CuTangoActionI::TaDb;
}

void CuTaDb::addDataListener(CuDataListener *l)
{
    std::list<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
    l->setValid();
    /* if a new listener is added after onResult, call onUpdate.
     * This happens when multiple items connect to the same source
     * Post the result, so that it is delivered later.
     */
    if(!d->cached_data.isEmpty()) {
        d->activity->publishResult(d->cached_data);
    }
}

void CuTaDb::removeDataListener(CuDataListener *l) {
    d->listeners.remove(l);
    if(!d->listeners.size())
        stop();
}

void CuTaDb::sendData(const CuData &) { }

void CuTaDb::getData(CuData &d_inout) const { (void) d_inout; }

size_t CuTaDb::dataListenersCount() {
    return d->listeners.size();
}

void CuTaDb::start()
{
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    CuData tt("thread_token", "tangodb"); // same thread for all db accesses
    at["activity"] = "cutadb";
    at.merge(d->options);
    if(d->options.containsKey("thread_token"))
        tt["thread_token"] = d->options["thread_token"];
    d->activity = new CuTaDbActivity(at, d->tsrc);
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
void CuTaDb::stop()
{
    if(!d->xit) {
        d->xit = true;
        d->listeners.clear();
        d->cumbia_t->unregisterActivity(d->activity);
    }
}

/*! \brief CuActionFactory relies on this returning true to unregister the action
 */
bool CuTaDb::exiting() const
{
    return d->xit;
}
