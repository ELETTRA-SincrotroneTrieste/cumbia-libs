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


class CuTaDbPrivate {
public:
    CuTaDbPrivate(const TSource& t_src, CumbiaTango *ct, const CuData &_options, const CuData &_tag)
        : tsrc(t_src), cumbia_t(ct), activity(nullptr), options(_options), tag(_tag) {
    }
    std::list<CuDataListener *> listeners;
    const TSource tsrc;
    CumbiaTango *cumbia_t;
    CuTaDbActivity *activity;
    CuData options, tag;
};

CuTaDb::CuTaDb(const TSource& src, CumbiaTango *ct, const CuData &options, const CuData &tag) {
    d = new CuTaDbPrivate(src, ct, options, tag); // src, t are const
}

CuTaDb::~CuTaDb()
{
    pdelete("~CuTaDb: %p", this);
    delete d;
}

void CuTaDb::setOptions(const CuData &options) {
    d->options = options;
}

void CuTaDb::setTag(const CuData &tag) {
    d->tag = tag;
}

void CuTaDb::onProgress(int step, int total, const CuData &data) {
    (void) step; // unused
    (void) total;
    (void) data;
}

void CuTaDb::onResult(const CuData &data)
{
    const std::string& src = data.s("src");
    // do not update configuration data if exit
    std::list <CuDataListener *> listeners = d->listeners;
    std::list<CuDataListener *>::iterator it;
    for(it = listeners.begin(); it != listeners.end(); ++it) {
        (*it)->onUpdate(data);
    }
    d->cumbia_t->removeAction(d->tsrc.getName(), getType());
    d->cumbia_t->unregisterActivity(d->activity);
    d->listeners.clear();
    delete this;
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

void CuTaDb::start() {
    const std::string& tt = d->options.containsKey("thread_token") ? d->options.s("thread_token") : "cutadb";
    d->activity = new CuTaDbActivity(d->tsrc, d->options, d->tag);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, tt, fi, bf);
}

/*! \brief calls unregisterActivity on Cumbia in order to enter the exit state
 *
 * - sets the exiting flag to true
 * - calls Cumbia::unregisterActivity
 */
void CuTaDb::stop() {
        d->listeners.clear();
        d->cumbia_t->unregisterActivity(d->activity);
}
