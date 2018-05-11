#include "cuput.h"
#include "cumbiaepics.h"
#include "cuputactivity.h"
#include "cuepics-world.h"
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

class CuEpWriterPrivate
{
public:
    std::list<CuDataListener *> listeners;
    EpSource ep_src;
    CumbiaEpics *cumbia_ep;
    CuActivity *activity;
    bool exit;
    CuConLogImpl li;
    CuLog log;
    CuVariant write_val;
};

CuPut::CuPut(const EpSource& src,
                     CumbiaEpics *ct)
{
    d = new CuEpWriterPrivate();
    d->ep_src = src;
    d->cumbia_ep = ct;
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
    cuprintf("CuPut.onResult: data received %s\n", data.toString().c_str());
    std::list<CuDataListener *>::iterator it;
    for(it = d->listeners.begin(); it != d->listeners.end(); ++it)
        (*it)->onUpdate(data);

    if(data["exit"].toBool())
    {
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_ep->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->ep_src.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuPut::getToken() const
{
    CuData da("source", d->ep_src.getName());
    da["type"] = std::string("writer");
    return da;
}

EpSource CuPut::getSource() const
{
    return d->ep_src;
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
            static_cast<CuEpCAService *>(d->cumbia_ep->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuEpCAService::CuEpicsChannelAccessServiceType)));
    CuData at("src", d->ep_src.getName()); /* activity token */
    at["ioc"] = d->ep_src.getIOC();
    at["pv"] = d->ep_src.getPV();
    at["activity"] = "writer";
    at["write_value"] = d->write_val;
    CuData tt("ioc", d->ep_src.getIOC()); /* thread token */
    d->activity = new CuPutActivity(at, df);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_ep->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_ep->getThreadFactoryImpl());
    d->cumbia_ep->registerActivity(d->activity, this, tt, fi, bf);
    cuprintf("> CuPut.start writer %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->activity);
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

