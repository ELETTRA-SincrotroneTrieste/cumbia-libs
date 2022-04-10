#include "cucontext.h"

#include <cucontrolsreader_abs.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolswriter_abs.h>
#include <cucontrolsfactorypool.h>
#include <cudata.h>
#include <cumbiapool.h>
#include "culinkstats.h"

#include <QtDebug>

class CuContextPrivate
{
  public:
    QList<CuControlsReaderA* > readers;
    QList<CuControlsWriterA* > writers;
    CuData options;
    CuLinkStats *link_stats;
    Cumbia* cu;
    CumbiaPool *cu_pool;
    CuControlsReaderFactoryI *r_factory;
    CuControlsWriterFactoryI *w_factory;
    CuControlsFactoryPool ctrl_factory_pool;
};

CuContext::CuContext(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac)
{
    d = new CuContextPrivate;
    d->link_stats = new CuLinkStats(10);
    d->cu = cumbia;
    d->r_factory = r_fac.clone();
    d->w_factory = NULL;
    d->cu_pool = NULL;
}

CuContext::CuContext(Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
{
    d = new CuContextPrivate;
    d->link_stats = new CuLinkStats(10);
    d->cu = cumbia;
    d->w_factory = w_fac.clone();
    d->r_factory = NULL;
    d->cu_pool = NULL;
}

CuContext::CuContext(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
{
    d = new CuContextPrivate;
    d->link_stats = new CuLinkStats(10);
    d->ctrl_factory_pool = fpool;
    d->cu_pool = cumbia_pool;
    d->cu = NULL;
    d->r_factory = NULL;
    d->w_factory = NULL;
}

CuContext::~CuContext()
{
    disposeReader();
    disposeWriter();

    if(d->r_factory)
        delete d->r_factory;
    if(d->w_factory)
        delete d->w_factory;

    delete d->link_stats;
    delete d;
}

bool CuContext::isAuto() const
{
    return d->cu_pool != NULL;
}

CuControlsReaderA *CuContext::m_make_reader(const std::string &s, CuDataListener *datal) const
{
    Cumbia *cumbia = NULL;
    CuControlsReaderFactoryI *r_fa;

    if(d->cu_pool && !d->ctrl_factory_pool.isEmpty()) /* pick Cumbia impl */
    {
        // pick a cumbia and reader factory implementation from the pool
        cumbia = d->cu_pool->getBySrc(s);
        r_fa = d->ctrl_factory_pool.getRFactoryBySrc(s);
    }
    else
    {
        // use specific cumbia and reader factory implementations
        cumbia = d->cu;
        r_fa = d->r_factory;
    }

    if(!cumbia || !r_fa)
        return NULL;
    if(!d->options.isEmpty()) { // otherwise use options from r_factory
        r_fa->setOptions(d->options);
    }
    return r_fa->create(cumbia, datal);
}

CuControlsWriterA *CuContext::m_make_writer(const std::string &s, CuDataListener *datal) const
{
    CuControlsWriterA *writer = NULL;
    Cumbia *cumbia = NULL;
    CuControlsWriterFactoryI *w_fa;

    if(d->cu_pool && !d->ctrl_factory_pool.isEmpty()) /* pick Cumbia impl */
    {
        // pick a cumbia and reader factory implementation from the pool
        cumbia = d->cu_pool->getBySrc(s);
        w_fa = d->ctrl_factory_pool.getWFactoryBySrc(s);
    }
    else
    {
        // use specific cumbia and reader factory implementations
        cumbia = d->cu;
        w_fa = d->w_factory;
    }

    if(!cumbia || !w_fa)
        return NULL;
    if(!d->options.isEmpty())
        w_fa->setOptions(d->options);
    writer = w_fa->create(cumbia, datal);

    return writer;
}

CuControlsReaderA *CuContext::replace_reader(const std::string &s, CuDataListener* datal)
{
    disposeReader(std::string()); // all
    CuControlsReaderA *reader = m_make_reader(s, datal);
    if(reader)
        d->readers.append(reader);
    return reader;
}

CuControlsWriterA *CuContext::replace_writer(const std::string &s, CuDataListener *datal)
{
    // replace
    disposeWriter(std::string());
    CuControlsWriterA *writer = m_make_writer(s, datal);
    // store the new writer
    if(writer)
        d->writers.append(writer);
    return writer;
}

CuControlsReaderA *CuContext::add_reader(const std::string &s, CuDataListener *datal)
{
    disposeReader(s);
    CuControlsReaderA *r = m_make_reader(s, datal);
    if(r)
        d->readers.append(r);
    return r;
}

CuControlsWriterA *CuContext::add_writer(const std::string &s, CuDataListener *datal)
{
    CuControlsWriterA *w = m_make_writer(s, datal);
    if(w)
        d->writers.append(w);
    return w;
}

CuControlsPropertyReaderA *CuContext::get_property_reader(const std::string &, CuDataListener *) {
    return nullptr;
}

void CuContext::disposeReader(const std::string &src)
{
    CuControlsReaderA *removed = NULL;
    foreach(CuControlsReaderA *r, d->readers)
    {
        if(r->source().toStdString() == src || src == std::string())
        {
            removed = r;
            printf("CuContext::disposeReader: deleting reader for %s\n", src.c_str());
            r->unsetSource();
            delete r;
        }
    }
    if(src == std::string())
        d->readers.clear();
    else if(removed)
        d->readers.removeAll(removed);
}

void CuContext::disposeWriter(const std::string &src)
{
    CuControlsWriterA *removed = NULL;
    foreach(CuControlsWriterA *w, d->writers)
    {
        if(w->target().toStdString() == src || src == std::string())
        {
            removed = w;
            delete w;
        }
    }
    if(src == std::string())
        d->writers.clear();
    else if(removed)
        d->writers.removeAll(removed);
}

/** Set the options bundle on the context.
 *
 * @param options a CuData bundle with key/value pairs of options.
 *
 * Saves the options locally. Options are used to configure the readers and writers
 * instantiated by CuContext.
 *
 * \par Note
 * You may want to call setOptions before replace_reader or replace_writer, so that they affect
 * the link configuration. The CuContext introductory documentation lists the known options.
 *
 * @see sendData
 * @see options
 */
void CuContext::setOptions(const CuData &options)
{
    d->options = options;
}

/** \brief Send options bundle to the link, if active, and save them locally.
 *
 * If the link (reader or writer) is active (either setSource or setTarget has been called)
 * the options are sent to either CuControlsReaderA or CuControlsWriterA (hereafter named "<em>link</em>")
 * through CuControlsReaderA::sendData or CuControlsWriterA::sendData
 * CuData options bundle is also saved locally and can be fetched with the QuContext::options method.
 *
 * \par Note
 * Call setOptions on CuContext <em>before</em> setSource/setTargets on the qumbia-qtcontrols object to set up
 * the link with the desired options. Please remind that the options that can be applied are
 * CuControlsReaderA/CuControlsWriterA (named <em>link implementation</em> hereafter) implementation dependent.
 * (For example, read mode - polling or event driven - polling period and so on).
 *
 * \par Observations
 * \li options bundle contents apply to the link implementation.
 * \li depending on the link implementation, it may be necessary to call setSource or setTargets
 *     (for example, Tango attribute properties are fetched only at setup, so options concerning
 *      initial configuration require setSource after setOptions). See the <em>qumbia-tango-controls</em>
 *      <strong>qucontext</strong> example.
 *
 *     See the specific link implementation (e.g. CuTReader/CuTWriter for Tango and CuEpReader/CuEpWriter
 *     for Epics) to determine what are the available options and what is done
 *     by CuControlsReaderA::sendData.
 *
 * @see CuTReader::sendData
 *
 * \since 1.2.3
 * \par Argument editing
 * The "args" key can be used to update the readers' arguments (Tango, also through cumbia-http).
 * \par Example
 * \code
 * myreader->getContext()->sendData(CuData("args", std::vector<std::string> {"1", "0" }));
 * \endcode
 */
void CuContext::sendData(const CuData &data)
{
    foreach(CuControlsReaderA *r, d->readers)
        r->sendData(data);
    foreach(CuControlsWriterA *w, d->writers)
        w->sendData(data);
    if(d->readers.size() == 0 && d->writers.size() == 0)
        perr("QuContext.sendData: link is not initialised");
}

/** Multiple link version of sendData(const CuData& data).
 *
 * @param data  list of CuData options bundle to send to the links with the names provided.
 *        Every element of the list must have a "src" key with the name of the source (target)
 *        of the link to which the data must be sent to.
 *
 * @see getData(QList<CuData> &in_datalist)
 */
void CuContext::sendData(const QList<CuData> &data)
{
    CuControlsReaderA *r;
    CuControlsWriterA *w;
    foreach(CuData d, data)
    {
        if(d.containsKey("src") && (r = findReader(d["src"].toString())))
            r->sendData(d);
        if(d.containsKey("src") && (w = findWriter(d["src"].toString())))
            w->sendData(d);
    }
}

/** \brief Send to the link a bundle with input arguments (keys) and get back the same bundle with values,
 *         if applicable.
 *
 * @param d_inout a CuData bundle with input arguments for the CuControlsReaderA/CuControlsWriter in use.
 *
 * @see setOptions
 * @see getData(QList<CuData> &in_datalist)
 *
 * \note How CuControlsReaderA::sendData deals with data is up to the CuControlsReaderA specific
 * implementation. The same goes for CuControlsWriterA.
 * See CuTReader::getData/CuTWriter::getData for further details and a list of supported key/values in the
 * CuData bundle for the Tango control system.
 *
 * \note If the context contains more readers (writers), use the getData(QList<CuData> &in_datalist) version
 * of the method.
 */
void CuContext::getData(CuData &d_inout)
{
    if(d->readers.size() > 0)
        d->readers.first()->getData(d_inout);
    if(d->writers.size() > 0)
        d->writers.first()->getData(d_inout);
    if(d->readers.size() == 0 && d->writers.size() == 0)
        perr("QuContext.sendData: link is not initialised");
}

/** \brief Get data from multiple readers (writers).
 *
 * @param in_datalist a list of CuData containing the keys you want to be associated to values.
 *        Each in_datalist element must contain a "src" key with the name of the source to search
 *        for in the readers (writers) list.
 *
 * \par Example. Get the period property of each curve in the plot.
 * Suppose the plot has two curves with different refresh period. Get the period of each one.
 * \li curve 1 has source current:a
 * \li curve 2 has source current:b
 *
 * \code
 * CuData p1, p2;
 * p1["src"]  = "current:a";
 * p1["period"] = -1; // will be set by getData
 * p2["src"] = "current:b";
 * p2["period"] = -1;
 * QList<CuData> in_dlist = QList<CuData>() << p1 << p2;
 * // suppose we have a QuTrendPlot named myTrendPlot...
 * myTrendPlot->getContext()->getData(in_dlist);
 *
 * printf("Period a: %d[ms] period b: %d[ms]\n", p1["period"].toInt(), p2["period"].toInt() );
 *
 * \endcode
 *
 */
void CuContext::getData(QList<CuData> &in_datalist)
{
    CuControlsReaderA *r;
    CuControlsWriterA *w;
    for(int i = 0; i < in_datalist.size(); i++)
    {
        CuData &d = in_datalist[i];
        if(d.containsKey("src") && (r = findReader(d["src"].toString())))
            r->getData(d);
        if(d.containsKey("src") && (w = findWriter(d["src"].toString())))
            w->getData(d);
    }
}

/** \brief Get the options currently set on the context.
 *
 * @see setOptions
 */
CuData CuContext::options() const
{
    return d->options;
}

CuControlsReaderA *CuContext::getReader() const
{
    if(d->readers.size()  == 1)
        return d->readers.first();
    return NULL;
}

CuControlsWriterA *CuContext::getWriter() const
{
    if(d->writers.size()  == 1)
        return d->writers.first();
    return NULL;
}

CuControlsReaderA *CuContext::findReader(const std::string &srcnam)
{
    foreach(CuControlsReaderA* r, d->readers)
        if(r->source().toStdString() == srcnam)
            return r;
    return NULL;
}

CuLinkStats *CuContext::getLinkStats() const
{
    return d->link_stats;
}

QList<CuControlsReaderA *> CuContext::readers() const
{
    return d->readers;
}

QList<CuControlsWriterA *> CuContext::writers() const
{
    return d->writers;
}

Cumbia *CuContext::cumbia() const
{
    return d->cu;
}

CumbiaPool *CuContext::cumbiaPool() const
{
    return d->cu_pool;
}

CuControlsWriterA *CuContext::findWriter(const std::string &targetnam)
{
    foreach(CuControlsWriterA* w, d->writers)
        if(w->target().toStdString() == targetnam)
            return w;
    return NULL;
}

/*! \brief returns a pointer to the CuControlsReaderFactoryI or NULL if the context is not a reader
 *         context
 *
 * @return a pointer to CuControlsReaderFactoryI * if the context refers to a reader, NULL otherwise
 *
 */
CuControlsReaderFactoryI *CuContext::getReaderFactoryI() const
{
    return d->r_factory;
}

/*! \brief returns a pointer to the CuControlsWriterFactoryI or NULL if the context is not a writer
 *         context
 *
 * @return a pointer to CuControlsWriterFactoryI * if the context refers to a writer, NULL otherwise
 *
 */
CuControlsWriterFactoryI *CuContext::getWriterFactoryI() const
{
    return d->w_factory;
}

/*! \brief returns a copy of the CuControlsFactoryPool, which will be valid if the context
 *         is configured with a CumbiaPool object.
 *
 * @return a copy of the CuControlsFactoryPool used by the pool to create readers or writers, if
 *         the CuContext is configured with the CumbiaPool/CuControlsFactoryPool pair.
 */
CuControlsFactoryPool CuContext::getControlsFactoryPool() const
{
    return d->ctrl_factory_pool;
}

