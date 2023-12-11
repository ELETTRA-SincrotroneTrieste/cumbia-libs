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
    bool exiting;
    CuConLogImpl li;
    CuLog log;
    CuVariant write_val;
    CuData property_d, value_d;
};

CuPut::CuPut(const EpSource& src,
                     CumbiaEpics *ct)
{
    d = new CuEpWriterPrivate();
    d->ep_src = src;
    d->cumbia_ep = ct;
    d->exiting = false;
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
    d->exiting = data[CuDType::Exit].toBool();  // data["exit"]
    std::list<CuDataListener *>::iterator it;
    for(it = d->listeners.begin(); it != d->listeners.end(); ++it)
        (*it)->onUpdate(data);


    if(d->exiting)
    {
        CuEpicsActionFactoryService * af = static_cast<CuEpicsActionFactoryService *>(d->cumbia_ep->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuEpicsActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->ep_src.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuPut::getToken() const
{
    CuData da("source", d->ep_src.getName());
    da[CuDType::Type] = std::string("writer");  // da["type"]
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
    CuData at(CuDType::Src, d->ep_src.getName()); /* activity token */  // CuData at("src", d->ep_src.getName()
    at[CuDType::Pv] = d->ep_src.getPV();  // at["pv"]
    at[CuDType::Activity] = "writer";  // at["activity"]
    at["write_value"] = d->write_val;
    std::string tt("pv" + d->ep_src.getPV()); /* thread token */
    d->activity = new CuPutActivity(at, df);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_ep->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_ep->getThreadFactoryImpl());
    d->cumbia_ep->registerActivity(d->activity, this, tt, fi, bf);
    cuprintf("> CuPut.start writer %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->activity);
}

void CuPut::stop()
{
    d->exiting = true;
}

void CuPut::sendData(const CuData& )
{

}

void CuPut::getData(CuData &d_inout) const
{
   d_inout = CuData();
}

bool CuPut::exiting() const
{
    return d->exiting;
}


void CuPut::onResult(const std::vector<CuData> &datalist)
{
}
