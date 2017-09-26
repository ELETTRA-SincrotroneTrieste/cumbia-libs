#include "cuput.h"
#include "cumbiaepics.h"
#include "cuputactivity.h"
#include "cuepactionfactoryservice.h"
#include "cuepcaservice.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <list>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <culog.h>

#include <cadef.h>

class EpSource;

class CuTWriterPrivate
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
};

CuPut::CuPut(const EpSource& src,
                     CumbiaEpics *ct)
{
    d = new CuTWriterPrivate();
    d->tsrc = src;
    d->cumbia_t = ct;
    d->exit = false;
    d->log = CuLog(&d->li);
}

CuPut::~CuPut()
{
    delete d;
}

void CuPut::setWriteValue(const CuVariant &write_val)
{
    d->write_val = write_val;
}

void CuPut::onProgress(int step, int total, const CuData &data)
{
    (void) step; // unused
    (void) total;
    (void) data;
}

void CuPut::onResult(const CuData &data)
{
    cuprintf("CuTWriter.onResult: data received %s\n", data.toString().c_str());
    std::list<CuDataListener *>::iterator it;
    for(it = d->listeners.begin(); it != d->listeners.end(); ++it)
        (*it)->onUpdate(data);

    if(data["exit"].toBool())
    {
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuPut::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("writer");
    return da;
}

EpSource CuPut::getSource() const
{
    return d->tsrc;
}

CuEpicsActionI::Type CuPut::getType() const
{
    return CuEpicsActionI::Writer;
}


void CuPut::addDataListener(CuDataListener *l)
{
    std::list<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuPut::removeDataListener(CuDataListener *l)
{
    d->listeners.remove(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuPut::dataListenersCount()
{
    return d->listeners.size();
}

void CuPut::start()
{
    CuEpCAService *df =
            static_cast<CuEpCAService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuEpCAService::CuEpicsChannelAccessServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["ioc"] = d->tsrc.getIOC();
    at["pv"] = d->tsrc.getPV();
    at["activity"] = "writer";
    at["write_value"] = d->write_val;
    at["pv"] = (d->tsrc.getType() == EpSource::PV);
    CuData tt("ioc", d->tsrc.getIOC()); /* thread token */
    d->activity = new CuWriteActivity(at, df);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, tt, fi, bf);
    cuprintf("> CuTWriter.start writer %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->activity);
}

void CuPut::stop()
{

}

void CuPut::sendData(const CuData& )
{

}

void CuPut::getData(CuData &d_inout) const
{

}

