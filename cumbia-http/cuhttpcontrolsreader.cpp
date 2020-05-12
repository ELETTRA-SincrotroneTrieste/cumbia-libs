#include "cuhttpcontrolsreader.h"
#include "cumbiahttp.h"
#include "cuhttpcontrolsreader.h"
#include "cuhttpactionreader.h"
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
    QString source;
    CumbiaHttp *cumbia_http;
    CuDataListener *tlistener;
    CuData options;
};

CuHttpControlsReader::CuHttpControlsReader(Cumbia *cumbia, CuDataListener *tl)
    : CuControlsReaderA(cumbia, tl)
{
    assert(cumbia->getType() == CumbiaHttp::CumbiaHTTPType);
    d = new CuHTTPReaderPrivate;
    d->cumbia_http = static_cast<CumbiaHttp *>(cumbia);
    d->tlistener = tl;
}

CuHttpControlsReader::~CuHttpControlsReader()
{
    d->tlistener->invalidate();
    unsetSource();
    delete d;
}

void CuHttpControlsReader::setSource(const QString &s) {
    d->source = s;
    QList<QuReplaceWildcards_I *>rwis = d->cumbia_http->getReplaceWildcard_Ifaces();
    // d->source is equal to 's' if no replacement is made
    for(int i = 0; i < rwis.size() && d->source == s; i++) // leave loop if s != d->source (=replacement made)
        d->source = rwis[i]->replaceWildcards(s, qApp->arguments());

    CuHTTPActionConfFactory httpwconff;
    httpwconff.setOptions(d->options);
    d->cumbia_http->addAction(d->source.toStdString(), d->tlistener, httpwconff);
    CuHTTPActionReaderFactory httprf;
    httprf.setOptions(d->options);
    d->cumbia_http->addAction(d->source.toStdString(), d->tlistener, httprf);
}

QString CuHttpControlsReader::source() const
{
    return d->source;
}

void CuHttpControlsReader::unsetSource()
{
    d->cumbia_http->unlinkListener(d->source.toStdString(), CuHTTPActionA::Reader, d->tlistener);
    d->source = QString();
}

void CuHttpControlsReader::setOptions(const CuData &o)
{
    d->options = o;
}

CuData CuHttpControlsReader::getOptions() const
{
    return d->options;
}

void CuHttpControlsReader::sendData(const CuData &)
{

}

void CuHttpControlsReader::getData(CuData &d_ino) const
{
    Q_UNUSED(d_ino);
}

