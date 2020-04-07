#include "cuwscontrolswriter.h"
#include "cuwsactionwriter.h"
#include "cuwsactionfactories.h"
#include "qureplacewildcards_i.h"

#include <cumbiawebsocket.h>
#include <cudatalistener.h>
#include <QCoreApplication>
#include <cumacros.h>
#include <assert.h>

/*! @private
 */
class CuWsWriterFactoryPrivate
{
public:
    CuData w_options, w_conf;
    CuVariant wv;
};

/*! the class constructor
 */
CuWsControlsWriterFactory::CuWsControlsWriterFactory()
{
    d = new CuWsWriterFactoryPrivate;
}

/*! \brief the class destructor
 *
 */
CuWsControlsWriterFactory::~CuWsControlsWriterFactory() {
    delete d;
}

/*! \brief set options through a CuData bundle. They are then passed to the
 *         CuWsWriter instantiated and returned by the create method
 *
 * @param o a CuData bundle of key/value pairs
 *
 * \par Key/value pairs
 * \li *no-properties* [bool, default: false]. If true, skip attribute configuration in setTarget.
 * \li *fetch_props* [std::vector<std::string>, default: empty] list of attribute properties to
 *     fetch from the database during configuration. The *no-properties* key must be left to false
 *     (default).
 *
 * @see create
 */
void CuWsControlsWriterFactory::setOptions(const CuData &o) {
    d->w_options = o;
}

/** \brief creates and returns a new CuWsWriter, configured with the given
 *         Cumbia and CuDataListener.
 *
 * Call setOptions before create to configure the writer.
 * See setOptions for a list of predefined keys.
 *
 * @param c a pointer to the CumbiaWebSocket object
 * @param l a pointer to an object implementing the CuDataListener interface
 *
 * @return a CuWsWriter, an implementation of CuControlsWriterA abstract class
 */
CuControlsWriterA *CuWsControlsWriterFactory::create(Cumbia *c, CuDataListener *l) const {
    CuWsControlsWriter *wf = new CuWsControlsWriter(c, l);
    wf->setOptions(d->w_options);
    return wf;
}

/** \brief clones the writer factory, allocating a new one with the same options
 *
 * @return a new CuWsWriterFactory with options copied from this object
 */
CuControlsWriterFactoryI *CuWsControlsWriterFactory::clone() const {
    CuWsControlsWriterFactory *wf = new CuWsControlsWriterFactory();
    wf->d->w_options = d->w_options;
    return wf;
}

/*! @private
 */
class CuWsWriterPrivate {
public:
    QString target;
    CumbiaWebSocket *cu_ws;
    CuDataListener *tlistener;
    CuData w_options;
};

/*! \brief class constructor that's configured with CumbiaWebSocket and CuDataListener instances
 *
 * @param cu_ws a pointer to a CumbiaWebSocket instance
 * @param CuDataListener a pointer to an object implementing the CuDataListener interface
 */
CuWsControlsWriter::CuWsControlsWriter(Cumbia *cu_ws, CuDataListener *tl)
 : CuControlsWriterA(cu_ws, tl) {
    assert(cu_ws->getType() == CumbiaWebSocket::CumbiaWSType);
    d = new CuWsWriterPrivate;
    d->cu_ws = static_cast<CumbiaWebSocket *>(cu_ws);
    d->tlistener = tl;
}

/*! \brief the class destructor.
 *
 * The class destructors sets the invalidate flag on the listener, so that it won't be
 * used later, and calls clearTarget
 */
CuWsControlsWriter::~CuWsControlsWriter()
{
    d->tlistener->invalidate();
    clearTarget();
    delete d;
}

/*! \brief set the options on the writer (before setTarget)
 *
 * This is usually called by CuWsWriterFactory::create immediately after CuWsWriter is
 * instantiated.
 */
void CuWsControlsWriter::setOptions(const CuData &o)
{
    d->w_options = o;
}

/*! \brief returns the configured target, or an empty string
 *
 * @return the configured target or an empty string if setTarget hasn't been
 * called yet.
 *
 * \par note
 * The returned string will contain the actual target, after wildcards have been
 * replaced.
 */
QString CuWsControlsWriter::target() const
{
    return d->target;
}

/*! \brief remove the configured target, if any.
 *
 * \li unlinks the attribute configuration listener
 * \li unlinks the writer listener
 *
 */
void CuWsControlsWriter::clearTarget()
{
    d->cu_ws->unlinkListener(d->target.toStdString(), CuWSActionI::WriterConfig, d->tlistener);
    d->cu_ws->unlinkListener(d->target.toStdString(), CuWSActionI::Writer, d->tlistener);
    d->target = QString();
}

/*! \brief execute the target configured with setTarget. The value to write is taken
 *         from the return value of getArgs.
 *
 * A CuTangoWriterFactory is used to start an activity managed by CumbiaWebSocket.
 * The activity deals with writing the value on an attribute or imparting a
 * command once.
 *
 */
void CuWsControlsWriter::execute() {
    qDebug() << __PRETTY_FUNCTION__  << d->target << getArgs().toString().c_str();
    CuWSActionWriterFactory wtf;
    wtf.setWriteValue(getArgs());
    wtf.setConfiguration(getConfiguration());
    d->cu_ws->addAction(d->target.toStdString(), d->tlistener, wtf);
}

/*! \brief This is not implemented yet
 */
void CuWsControlsWriter::sendData(const CuData & /* data */)
{
    printf("CuWsWriter.sendData: not implemented\n");
}

/*! \brief This is not implemented yet
 */
void CuWsControlsWriter::getData(CuData & /*d_ino*/) const
{
    printf("CuWsWriter.getData: not implemented\n");
}

/*! \brief connect the writer to the Tango control system target
 *
 * @param s the name of the writer target
 */
void CuWsControlsWriter::setTarget(const QString &s) {
    d->target = s;
    QList<QuReplaceWildcards_I *>rwis = d->cu_ws->getReplaceWildcard_Ifaces();
    // d->source is equal to 's' if no replacement is made
    for(int i = 0; i < rwis.size() && d->target == s; i++) // leave loop if s != d->source (=replacement made)
        d->target = rwis[i]->replaceWildcards(s, qApp->arguments());
    qDebug() << __PRETTY_FUNCTION__ << "replace wildcards size " << rwis.size() << "target" << d->target;
    CuWSActionWriterConfFactory wswconff;
    wswconff.setOptions(d->w_options);
    d->cu_ws->addAction(d->target.toStdString(), d->tlistener, wswconff);
}
