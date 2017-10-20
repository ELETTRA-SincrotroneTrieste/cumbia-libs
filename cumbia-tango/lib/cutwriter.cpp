#include "cutwriter.h"
#include "cumbiatango.h"
#include "tdevice.h"
#include "cuwriteactivity.h"
#include "cudevicefactoryservice.h"
#include "cuactionfactoryservice.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <list>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <culog.h>

#include <tango.h>

class TSource;

class CuTWriterPrivate
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
};

CuTWriter::CuTWriter(const TSource& src,
                     CumbiaTango *ct)
{
    d = new CuTWriterPrivate();
    d->tsrc = src;
    d->cumbia_t = ct;
    d->exit = false;
    d->log = CuLog(&d->li);
}

CuTWriter::~CuTWriter()
{
    delete d;
}

void CuTWriter::setWriteValue(const CuVariant &write_val)
{
    d->write_val = write_val;
}

void CuTWriter::onProgress(int step, int total, const CuData &data)
{
    (void) step; // unused
    (void) total;
    (void) data;
}

void CuTWriter::onResult(const CuData &data)
{
    cuprintf("CuTWriter.onResult: data received %s\n", data.toString().c_str());
    d->exit = data["exit"].toBool();
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::list<CuDataListener *> lis_copy = d->listeners;
    std::list<CuDataListener *>::iterator it;
    for(it = lis_copy.begin(); it != lis_copy.end(); ++it)
        (*it)->onUpdate(data);

    if(d->exit)
    {
        CuActionFactoryService * af = static_cast<CuActionFactoryService *>(d->cumbia_t->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuActionFactoryService::CuActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

CuData CuTWriter::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("writer");
    return da;
}

TSource CuTWriter::getSource() const
{
    return d->tsrc;
}

CuTangoActionI::Type CuTWriter::getType() const
{
    return CuTangoActionI::Writer;
}

void CuTWriter::addDataListener(CuDataListener *l)
{
    std::list<CuDataListener *>::iterator it = d->listeners.begin();
    l->setValid();
    d->listeners.insert(it, l);
}

void CuTWriter::removeDataListener(CuDataListener *l)
{
    if(l->invalid()) {
        d->listeners.remove(l);
        if(!d->listeners.size())
            stop();
    }
    else if(d->listeners.size() == 1)
        stop();
    else
        d->listeners.remove(l);
}

size_t CuTWriter::dataListenersCount()
{
    return d->listeners.size();
}

void CuTWriter::start()
{
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["device"] = d->tsrc.getDeviceName();
    at["point"] = d->tsrc.getPoint();
    at["activity"] = "writer";
    at["write_value"] = d->write_val;
    at["cmd"] = (d->tsrc.getType() == TSource::Cmd);
    CuData tt("device", d->tsrc.getDeviceName()); /* thread token */
    d->activity = new CuWriteActivity(at, df);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, tt, fi, bf);
    cuprintf("> CuTWriter.start writer %p thread 0x%lx ACTIVITY %p\n", this, pthread_self(), d->activity);
}

void CuTWriter::stop()
{
    d->exit = true;
}

bool CuTWriter::exiting() const
{
    return d->exit;
}

void CuTWriter::sendData(const CuData& )
{

}

void CuTWriter::getData(CuData &d_inout) const
{

}

