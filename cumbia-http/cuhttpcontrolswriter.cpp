#include "cuhttpcontrolswriter.h"
#include "cuhttpwritehelper.h"
#include "cuhttpactionfactories.h"
#include "qureplacewildcards_i.h"

#include <cumbiahttp.h>
#include <cudatalistener.h>
#include <cuhttptangosrc.h>
#include <QCoreApplication>
#include <cumacros.h>
#include <assert.h>

/*! @private
 */
class CuHttpWriterFactoryPrivate
{
public:
    CuData w_options, w_conf;
    CuVariant wv;
};

/*! the class constructor
 */
CuHttpControlsWriterFactory::CuHttpControlsWriterFactory()
{
    d = new CuHttpWriterFactoryPrivate;
}

/*! \brief the class destructor
 *
 */
CuHttpControlsWriterFactory::~CuHttpControlsWriterFactory() {
    delete d;
}

/*! \brief set options through a CuData bundle. They are then passed to the
 *         CuHttpWriter instantiated and returned by the create method
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
void CuHttpControlsWriterFactory::setOptions(const CuData &o) {
    d->w_options = o;
}

/** \brief creates and returns a new CuHttpWriter, configured with the given
 *         Cumbia and CuDataListener.
 *
 * Call setOptions before create to configure the writer.
 * See setOptions for a list of predefined keys.
 *
 * @param c a pointer to the CumbiaHttp object
 * @param l a pointer to an object implementing the CuDataListener interface
 *
 * @return a CuHttpWriter, an implementation of CuControlsWriterA abstract class
 */
CuControlsWriterA *CuHttpControlsWriterFactory::create(Cumbia *c, CuDataListener *l) const {
    CuHttpControlsWriter *wf = new CuHttpControlsWriter(c, l);
    wf->setOptions(d->w_options);
    return wf;
}

/** \brief clones the writer factory, allocating a new one with the same options
 *
 * @return a new CuHttpWriterFactory with options copied from this object
 */
CuControlsWriterFactoryI *CuHttpControlsWriterFactory::clone() const {
    CuHttpControlsWriterFactory *wf = new CuHttpControlsWriterFactory();
    wf->d->w_options = d->w_options;
    return wf;
}

/*! @private
 */
class CuHttpWriterPrivate {
public:
    QString target;
    CumbiaHttp *cu_http;
    CuDataListener *tlistener;
    CuData w_options;
};

/*! \brief class constructor that's configured with CumbiaHttp and CuDataListener instances
 *
 * @param cu_http a pointer to a CumbiaHttp instance
 * @param CuDataListener a pointer to an object implementing the CuDataListener interface
 */
CuHttpControlsWriter::CuHttpControlsWriter(Cumbia *cu_http, CuDataListener *tl)
 : CuControlsWriterA(cu_http, tl) {
    assert(cu_http->getType() == CumbiaHttp::CumbiaHTTPType);
    d = new CuHttpWriterPrivate;
    d->cu_http = static_cast<CumbiaHttp *>(cu_http);
    d->tlistener = tl;
}

/*! \brief the class destructor calls clearTarget
 */
CuHttpControlsWriter::~CuHttpControlsWriter() {
    d->cu_http->unlinkListener(d->tlistener);
    delete d;
}

/*! \brief set the options on the writer (before setTarget)
 *
 * This is usually called by CuHttpWriterFactory::create immediately after CuHttpWriter is
 * instantiated.
 */
void CuHttpControlsWriter::setOptions(const CuData &o)
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
QString CuHttpControlsWriter::target() const
{
    return d->target;
}

/*! \brief remove the configured target, if any.
 *
 * \li unlinks the attribute configuration listener
 * \li unlinks the writer listener
 *
 */
void CuHttpControlsWriter::clearTarget() {
    d->target = QString();
}

/*! \brief execute the target configured with setTarget. The value to write is taken
 *         from the return value of getArgs.
 *
 * A CuTangoWriterFactory is used to start an activity managed by CumbiaHttp.
 * The activity deals with writing the value on an attribute or imparting a
 * command once.
 *
 */
void CuHttpControlsWriter::execute() {
    CuHTTPActionWriterFactory wtf;
    wtf.setWriteValue(getArgs());
    wtf.setConfiguration(getConfiguration());
    std::string t = d->target.toStdString();
    // remove placeholders from the target, f.e. a/b/c/d(&objectref) -> a/b/c/d
    t = t.substr(0, t.find("("));
    // no need for helpers because setTarget already saves an all-inclusive d->target
    d->cu_http->executeWrite(CuHTTPSrc(t, QList<CuHttpSrcHelper_I *>()), d->tlistener, wtf);
}

/*! \brief This is not implemented yet
 */
void CuHttpControlsWriter::sendData(const CuData & /* data */) {
    printf("CuHttpWriter.sendData: not implemented\n");
}

/*! \brief This is not implemented yet
 */
void CuHttpControlsWriter::getData(CuData & /*d_ino*/) const {
    printf("CuHttpWriter.getData: not implemented\n");
}

/*! \brief connect the writer to the Tango control system target
 *
 * @param s the name of the writer target
 */
void CuHttpControlsWriter::setTarget(const QString &s) {
    d->target = s;
    QList<QuReplaceWildcards_I *>rwis = d->cu_http->getReplaceWildcard_Ifaces();
    // d->source is equal to 's' if no replacement is made
    for(int i = 0; i < rwis.size() && d->target == s; i++) // leave loop if s != d->source (=replacement made)
        d->target = rwis[i]->replaceWildcards(s, qApp->arguments());
    // same CuHTTPActionConfFactory for readers and writers: same "conf" method
    // The service will use a CuTWriterConfFactory if "writer" option is set to true
    CuHTTPActionConfFactory cf;
    cf.setOptions(d->w_options);
    std::string t = d->target.toStdString(), a;
    t.find("(") != std::string::npos ? a =t.substr(t.find("("))  : a = ""; // save args (*)
    // remove placeholders from the target, f.e. a/b/c/d(&objectref) -> a/b/c/d
    t = t.substr(0, t.find("("));
    CuHTTPSrc ht(t, d->cu_http->getSrcHelpers());
    d->target = QString::fromStdString(ht.prepare() + a); // (*) restore args in d->target
    CuHttpTangoSrc ts(d->target.toStdString());
    if(ts.getType() == CuHttpTangoSrc::SrcAttr)
        cf.mergeOptions(CuData("read-value", true));
    d->cu_http->readEnqueue(ht, d->tlistener, cf);
}
