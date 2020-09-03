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

CuControlsReaderA *CuHTTPReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuHttpControlsReader *r = new CuHttpControlsReader(c, l);
    r->setOptions(d->options);
    return r;
}

CuControlsReaderFactoryI *CuHTTPReaderFactory::clone() const
{
    CuHTTPReaderFactory *f = new CuHTTPReaderFactory();
    f->setOptions(d->options);
    return f;
}

void CuHTTPReaderFactory::setOptions(const CuData &options)
{
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
    if(!s.isEmpty()) {
        QList<QuReplaceWildcards_I *>rwis = d->cu_http->getReplaceWildcard_Ifaces();
        // d->source is equal to 's' if no replacement is made
        for(int i = 0; i < rwis.size() && d->s == s; i++) // leave loop if s != d->source (=replacement made)
            d->s = rwis[i]->replaceWildcards(s, qApp->arguments());
        d->o.value("single-shot").toBool() || d->o.value("manual").toBool() ? d->method = "read" : d->method = "s";
        CuHTTPActionReaderFactory httprf(d->method == "read");
        httprf.mergeOptions(d->o);
        d->cu_http->readEnqueue(d->s.toStdString(), d->dlis, httprf);
    }
}

QString CuHttpControlsReader::source() const {
    return d->s;
}

void CuHttpControlsReader::unsetSource() {
    d->cu_http->unlinkListener(d->s.toStdString(), d->method.toStdString(), d->dlis);
    d->s = QString();
}

void CuHttpControlsReader::setOptions(const CuData &o) {
    d->o = o;
}

CuData CuHttpControlsReader::getOptions() const {
    return d->o;
}

void CuHttpControlsReader::sendData(const CuData &data) {
    printf("CuHttpControlsReader::sendData \e[1;32m%s\e[0m\n", datos(data));
    if(data.containsKey("read")) {
        CuHTTPActionReaderFactory httprf(true);
        httprf.mergeOptions(d->o);
        d->cu_http->readEnqueue(d->s.toStdString(), d->dlis, httprf);
    }
}

void CuHttpControlsReader::getData(CuData &d_ino) const {
    Q_UNUSED(d_ino);
}

