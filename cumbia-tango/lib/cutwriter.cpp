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

#include <tango/tango.h>

class TSource;

/*! @private
 */
class CuTWriterPrivate
{
public:
    CuTWriterPrivate(const TSource& src,
                     CumbiaTango *ct,
                     const CuData &conf,
                     const CuData &opts,
                     const CuData &_tag)
        : tsrc(src), cumbia_t(ct), activity(nullptr), db_conf(conf), options(opts), tag(_tag) { }

    std::set<CuDataListener *> listeners;
    TSource tsrc;
    CumbiaTango *cumbia_t;
    CuActivity *activity;
    CuConLogImpl li;
    CuLog log;
    CuVariant write_val;
    CuData db_conf, options, tag;
};

CuTWriter::CuTWriter(const TSource& src,
                     CumbiaTango *ct, const CuData &conf, const CuData &options, const CuData &tag)
{
    d = new CuTWriterPrivate(src, ct, conf, options, tag);
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

/*!
 * \brief Set database configuration
 * \param db_conf data of type CuDType::Property obtained from a
 */
void CuTWriter::setConfiguration(const CuData& db_conf) {
    d->db_conf = db_conf;
}

void CuTWriter::setOptions(const CuData &options) {
    d->options = options;
}

void CuTWriter::setTag(const CuData &tag) {
    d->tag = tag;
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

/*! \brief calls onUpdate on all listeners and deletes itself afterwards
 *
 * @param data CuData with the result to be delivered to every CuDataListener
 *
 * \note
 * onResult is called just once from CuWriteActivity.execute (since v1.2.0)
 * This allows cleaning everything after updating listeners.
 *
 * \note
 * onResult is executed in the main thread
 */
void CuTWriter::onResult(const CuData &data)
{
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::set<CuDataListener *> set_copy = d->listeners;
    std::set<CuDataListener *>::iterator it;
    for(it = set_copy.begin(); it != set_copy.end(); ++it)
        (*it)->onUpdate(data);
    d->cumbia_t->removeAction(d->tsrc.getName(), getType());
    d->cumbia_t->unregisterActivity(d->activity);
    d->listeners.clear();
    delete this;
}

CuData CuTWriter::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da[CuDType::Type] = std::string("writer");  // da["type"]
    return da;
}

TSource CuTWriter::getSource() const {
    return d->tsrc;
}

CuTangoActionI::Type CuTWriter::getType() const {
    return CuTangoActionI::Writer;
}

void CuTWriter::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuTWriter::removeDataListener(CuDataListener *l) {
    d->listeners.erase(l);
    if(!d->listeners.size())
        stop();
}

size_t CuTWriter::dataListenersCount() {
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
void CuTWriter::start() {
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));
    CuData at(CuDType::Src, d->tsrc.getName()); /* activity token */
    at[CuDType::Device] = d->tsrc.getDeviceName();
    at[CuDType::Point] = d->tsrc.getPoint();
    at[CuDType::Activity] = "writer";  // at["activity"]
    at["write_value"] = d->write_val;
    at[CuDType::CmdName] = (d->tsrc.getType() == TSource::SrcCmd);  // at["cmd"]
    at.merge(d->options);
    const std::string & thtok = d->options.containsKey("thread_token") ? d->options.s("thread_token") : d->tsrc.getDeviceName(); /* thread token */
    d->activity = new CuWriteActivity(at, df, d->db_conf, d->tag);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
    d->cumbia_t->registerActivity(d->activity, this, thtok, fi, bf);
}

void CuTWriter::stop() {
}

void CuTWriter::sendData(const CuData& ) {

}

void CuTWriter::getData(CuData &d_inout) const {
    d_inout = CuData();
}

