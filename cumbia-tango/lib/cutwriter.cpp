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

/*! @private
 */
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
    CuData options;
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

/*! \brief set the value that will be written after start is called
 *
 * @param write_val a const reference to a CuVariant containing the value to write
 *        when start is called
 *
 * @see start
 */
void CuTWriter::setWriteValue(const CuVariant &write_val)
{
    d->write_val = write_val;
}

void CuTWriter::setConfiguration(const CuData& dbc) {
    d->options = dbc;
}

/*! this method is currently void
 *
 * \note
 * onProgress is executed in the main thread
 */
void CuTWriter::onProgress(int step, int total, const CuData &data)
{
    (void) step; // unused
    (void) total;
    (void) data;
}

/*! \brief unused. To comply with interface
 *
 * @see onResult(const CuData &data)
 */
void CuTWriter::onResult(const std::vector<CuData> &datalist)
{
    (void) datalist;
}

/*! \brief calls onUpdate on all listeners or deletes itself when the *exiting* flag is set
 *
 * @param data CuData with the result to be delivered to every CuDataListener
 *
 * The method calls delete this if the *exit* flag is true, after calling CuActionFactoryService::unregisterAction
 *
 * \note
 * onResult is executed in the main thread
 */
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

/*! \brief creates and registers a CuWriteActivity to trigger a write operation on the Tango control system
 *
 * \note
 * This function is internally used by the library. Clients shouldn't need to deal with it.
 *
 * This method
 * \li fills in a CuData called *activity token*, that will be passed to the CuWriteActivity
 * \li fills in a CuData called *thread token*, used to register the CuWriteActivity and make
 *     the activity shared between writers with the same target.
 * \li instantiates and registers (i.e. starts) a CuWriteActivity.
 *
 * \note
 * start is usually called by CumbiaTango::addAction, which in turn is called by qumbia-tango-controls
 * CuTControlsWriter::execute.
 *
 */
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
    CuData thtok = CuData("device", d->tsrc.getDeviceName()); /* thread token */
    if(d->options.containsKey("thread_token"))
        thtok["thread_token"] = d->options["thread_token"];
    d->activity = new CuWriteActivity(at, df, d->options);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, thtok, fi, bf);
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
    d_inout = CuData();
}

