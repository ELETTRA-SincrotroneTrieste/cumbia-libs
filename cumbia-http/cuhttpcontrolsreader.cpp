#include "cuhttpcontrolsreader.h"
#include "cumbiahttp.h"
#include "cuhttpcontrolsreader.h"
#include "cuhttpactionfactories.h"
#include <cudatalistener.h>
#include <qureplacewildcards_i.h>
#include <cudata.h>
#include <cumacros.h>
#include <QString>
#include <vector>
#include <string>
#include <QtDebug>
#include <QList>
#include <QCoreApplication>
#include <cumbiahttpworld.h>
#include <qustringlist.h>
#include <qregularexpression.h>

class CuHTTPReaderFactoryPrivate {
public:
    CuData options;

};

CuHTTPReaderFactory::CuHTTPReaderFactory()
{
    d = new CuHTTPReaderFactoryPrivate;
}

CuHTTPReaderFactory::~CuHTTPReaderFactory()
{
    delete d;
}

/** \brief creates and returns a new CuHttpControlsReader, configured with the given
 *         Cumbia and CuDataListener.
 *
 * Call setOptions before create to configure the reader.
 *
 * @param c a pointer to the Cumbia object
 * @param l a pointer to an object implementing the CuDataListener interface
 *
 * @return a CuHttpControlsReader, an implementation of CuControlsReaderA abstract class
 *
 * \par Important note
 * The options are reset (to an empty CuData) after  CuTControlsReader
 * creation and initialization. This avoids applying the same options to subsequent create calls
 * on the same factory
 *
 * \par Important reading
 * Please see also the unsetSource documentation.
 */
CuControlsReaderA *CuHTTPReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuHttpControlsReader *r = new CuHttpControlsReader(c, l);
    r->setOptions(d->options);
    d->options = CuData(); // reset options
    return r;
}

CuControlsReaderFactoryI *CuHTTPReaderFactory::clone() const
{
    CuHTTPReaderFactory *f = new CuHTTPReaderFactory();
    f->setOptions(d->options);
    return f;
}

void CuHTTPReaderFactory::setOptions(const CuData &options) {
    d->options = options;
}

/*! @private
 */
class CuHTTPReaderPrivate
{
public:
    QString s, method;
    CumbiaHttp *cu_http;
    CuDataListener *dlis;
    CuData o;
};

CuHttpControlsReader::CuHttpControlsReader(Cumbia *cumbia, CuDataListener *tl)
    : CuControlsReaderA(cumbia, tl)
{
    assert(cumbia->getType() == CumbiaHttp::CumbiaHTTPType);
    d = new CuHTTPReaderPrivate;
    d->cu_http = static_cast<CumbiaHttp *>(cumbia);
    d->dlis = tl;
}

CuHttpControlsReader::~CuHttpControlsReader() {
    pdelete("CuHttpControlsReader %p %s calling unlink listener without sending \"u\" \n", this, qstoc(d->s));
    // since 1.3.1 the destructor will not send an "unsubscribe" request to the server
    // assuming that ~CuHttpControlsReader is invoked upon application close, when CumbiaHttp
    // unregisters the client globally without sending an unsubscribe request for each reader
    d->cu_http->unlinkListener(CuHTTPSrc(d->s.toStdString(), d->cu_http->getSrcHelpers()), d->method.toStdString(), d->dlis);    delete d;
}

/*!
 * \brief Enqueue a request for either a *single shot* reading and an optional
 *        subscription for data updates through the channel
 *
 * \param s the name of the source
 *
 * The reader always performs a *synchronous* immediate reading.
 *
 * CuHttpControlsReader stores a *method* property that determines the behaviour of the reader and can either be:
 * - *read*, only one reading is performed "synchronously", that is, an http reply is sent in response to
 *   the request;
 * - *s*, subscribe mode: the single shot reading (as above) is followed by updates through the channel
 *
 * The single shot only *read* mode is activated by either *single-shot* or *manual* option set on the
 * CuHTTPReaderFactory.
 *
 * Moreover, if the options contain the *property* key set to *true*, the synchronous reply
 * shall contain the source configuration alongside its value. Otherwise, only the value will be
 * fetched.
 */
void CuHttpControlsReader::setSource(const QString &s) {
    d->s = s;
    if(!s.isEmpty() && CumbiaHTTPWorld().source_valid(s.toStdString())) {
        QList<QuReplaceWildcards_I *>rwis = d->cu_http->getReplaceWildcard_Ifaces();
        // d->source is equal to 's' if no replacement is made
        for(int i = 0; i < rwis.size() && d->s == s; i++) // leave loop if s != d->source (=replacement made)
            d->s = rwis[i]->replaceWildcards(s, qApp->arguments());

        const CuHTTPSrc hs(d->s.toStdString(), d->cu_http->getSrcHelpers());
        // d->s must store the complete src, including tango host
        d->s = QString::fromStdString(hs.prepare());
        d->o.value("single-shot").toBool() || d->o.value("manual").toBool() ? d->method = "read" :  d->method = "s";
        CuHTTPActionReaderFactory httprf(d->method == "read");
        httprf.mergeOptions(d->o);
        // tell the service CuTReaderConfFactory can be safely used to get configuration *and* impart command_inout
        // if we are a reader. By default, to avoid unwanted command_inouts upon target (writer) configuration,
        // the service uses CuTWriterConfFactory if *read-value* is not in the options
        // NOTE: this safety measure is needed only with commands.
        httprf.mergeOptions(CuData("read-value", true));
        d->cu_http->readEnqueue(hs, d->dlis, httprf);
    }
}

QString CuHttpControlsReader::source() const {
    return d->s;
}


/*! \brief unlink the listener and, if the source is "subscribed", send an "unsubscribe" request
 *
 *  unsetSource will send an unsubscribe request if the method for this source is "s" (the source
 *  is monitored on a channel).
 *
 *  \par Important note
 *  If you are deleting the reader explicitly from the application, call unsetSource and then delete
 *  the reader.
 *  *Deleting the reader without calling unsetSource first will not send an unsubscribe request to the client*.
 *
 *  On the other hand, the class destructor alone will not send an unsubscribe request, to avoid flooding
 *  the server with unsubscribe requests at application shutdown. In this case, a *global unsubscribe* request
 *  is sent by CumbiaHttp for the entire client application
 *
 *  The implementation somehow differs from the other cumbia engines (for example *cumbia-tango*, where
 *  the destructor actually performs a thorough disconnection from the Tango device) but ensures a
 *  consistent and simple *unsetSource* interface across the engines.
 */
void CuHttpControlsReader::unsetSource() {
    const CuHTTPSrc s(d->s.toStdString(), d->cu_http->getSrcHelpers());
    d->cu_http->unlinkListener(s, d->method.toStdString(), d->dlis);
    if(d->method == "s") {
        d->cu_http->unsubscribeEnqueue(s, d->dlis);
    }
    d->s = QString();
}

void CuHttpControlsReader::setOptions(const CuData &o) {
    d->o = o;
}

CuData CuHttpControlsReader::getOptions() const {
    return d->o;
}

/*!
 * \brief Send data to the service managing this reader
 * \param data the data to send
 *
 * \list keys
 * \li *read* a read command
 * \li *args* a list of arguments to change on the source. This is valid for
 *     readings with input arguments only (e.g. Tango commands with argins)
 *
 * \par note
 * Potentially, the http service shares sources across different clients.
 * Changing the arguments (through the *args* key) of a shared source may lead to unwanted results for
 * the other clients reading the same data. In this case, an *edit* command is issued.
 * Issuing a *read* has only the side effect of potentially refreshing all clients connected to
 * the same source.
 *
 * On the other hand, if the "manual" option is enabled for the current reader,
 * then a new read is requested through http, which should be quite safe.
 */
void CuHttpControlsReader::sendData(const CuData &data) {
    bool a = data.containsKey("read") || data.containsKey("args");
    if(d->o["manual"].toBool() && a) {
        if(data.containsKey("args") && d->s.contains(QRegularExpression("\\(.*\\)"))) {
            d->s.replace(QRegularExpression("\\(.*\\)"), "(" + QuStringList(data["args"]).join(',') + ')');
        }
        if(data.containsKey("read"))
            setSource(d->s);
    }
    else if(a) {
        CuHTTPActionEditFactory httpeditf;
        httpeditf.setOptions(data);
        d->cu_http->readEnqueue(CuHTTPSrc(d->s.toStdString(), d->cu_http->getSrcHelpers()), d->dlis, httpeditf);
    }
}

void CuHttpControlsReader::getData(CuData &d_ino) const {
    Q_UNUSED(d_ino);
}

