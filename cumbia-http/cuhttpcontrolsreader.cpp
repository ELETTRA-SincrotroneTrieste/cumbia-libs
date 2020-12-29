#include "cuhttpcontrolsreader.h"
#include "cumbiahttp.h"
#include "cuhttpcontrolsreader.h"
#include "cuhttpactionreader.h"
#include "cuhttpactionconf.h"
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
    pdelete("CuHttpControlsReader %p %s", this, qstoc(d->s));
    unsetSource();
    delete d;
}

void CuHttpControlsReader::setSource(const QString &s) {
    d->s = s;
    if(!s.isEmpty() && CumbiaHTTPWorld().source_valid(s.toStdString())) {
        QList<QuReplaceWildcards_I *>rwis = d->cu_http->getReplaceWildcard_Ifaces();
        // d->source is equal to 's' if no replacement is made
        for(int i = 0; i < rwis.size() && d->s == s; i++) // leave loop if s != d->source (=replacement made)
            d->s = rwis[i]->replaceWildcards(s, qApp->arguments());
        d->o.value("single-shot").toBool() || d->o.value("manual").toBool() ? d->method = "read" : d->method = "s";
        CuHTTPActionReaderFactory httprf(d->method == "read");
        httprf.mergeOptions(d->o);
        const CuHTTPSrc hs(d->s.toStdString(), d->cu_http->getSrcHelpers());
        // d->s must store the complete src, including tango host
        d->s = QString::fromStdString(hs.prepare());
        d->cu_http->readEnqueue(hs, d->dlis, httprf);
    }
}

QString CuHttpControlsReader::source() const {
    return d->s;
}

void CuHttpControlsReader::unsetSource() {
    d->cu_http->unlinkListener(CuHTTPSrc(d->s.toStdString(), d->cu_http->getSrcHelpers()), d->method.toStdString(), d->dlis);
    d->s = QString();
}

void CuHttpControlsReader::setOptions(const CuData &o) {
    d->o = o;
}

CuData CuHttpControlsReader::getOptions() const {
    return d->o;
}

void CuHttpControlsReader::sendData(const CuData &data) {
    if(data.containsKey("read") && d->o["manual"].toBool()) {
        printf("\e[1;32mCuHttpControlsReader::sendData in MANUAL MODE\e[0m\n");
        QString s(d->s);
        CuHTTPActionReaderFactory httprf(true); // single shot
        if(data.containsKey("args")) {
            QuStringList vs(data["args"]);
            s.replace(QRegularExpression("\\(.*\\)"), "(" + vs.join(','));
        }
        setSource(s);
    }
    else if(data.containsKey("read") || data.containsKey("args")) {
        printf("\e[1;32mCuHttpControlsReader::sendData \e[1;35m NOT IN MANUAL MODE\e[0m\n");
        CuHTTPActionEditFactory httpeditf;
        if(data.containsKey("args"))
            httpeditf.setOptions(CuData("args", data["args"]));
        else if(data.containsKey("read"))
            httpeditf.setOptions(CuData("read", data["read"]));
        d->cu_http->readEnqueue(CuHTTPSrc(d->s.toStdString(), d->cu_http->getSrcHelpers()), d->dlis, httpeditf);
    }
}

void CuHttpControlsReader::getData(CuData &d_ino) const {
    Q_UNUSED(d_ino);
}

