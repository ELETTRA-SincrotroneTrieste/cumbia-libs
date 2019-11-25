#include "curndreader.h"
#include "cumbiarandom.h"
#include "curndreader.h"
#include "curndactionreader.h"
#include "curndactionfactories.h"
#include "rndsourcebuilder.h"
#include <cudatalistener.h>
#include <cudata.h>
#include <cumacros.h>
#include <QString>
#include <vector>
#include <string>
#include <QtDebug>

class CuRNDReaderFactoryPrivate {
public:
    CuData options;

};

CuRNDReaderFactory::CuRNDReaderFactory()
{
    d = new CuRNDReaderFactoryPrivate;
}

CuRNDReaderFactory::~CuRNDReaderFactory()
{
    delete d;
}

CuControlsReaderA *CuRNDReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuRNDReader *r = new CuRNDReader(c, l);
    r->setOptions(d->options);
    return r;
}

CuControlsReaderFactoryI *CuRNDReaderFactory::clone() const
{
    CuRNDReaderFactory *f = new CuRNDReaderFactory();
    f->setOptions(d->options);
    return f;
}

void CuRNDReaderFactory::setOptions(const CuData &options)
{
    d->options = options;
}

/*! @private
 */
class CuRNDReaderPrivate
{
public:
    QString source;
    CumbiaRandom *cumbia_rnd;
    CuDataListener *tlistener;
    CuData options;
};

CuRNDReader::CuRNDReader(Cumbia *cumbia_rnd, CuDataListener *tl)
    : CuControlsReaderA(cumbia_rnd, tl)
{
    printf("CuRNDReader constructor!!!!!!!!!!\n");
    assert(cumbia_rnd->getType() == CumbiaRandom::CumbiaRNDType);
    d = new CuRNDReaderPrivate;
    d->cumbia_rnd = static_cast<CumbiaRandom *>(cumbia_rnd);
    d->tlistener = tl;
}

CuRNDReader::~CuRNDReader()
{
    qDebug() << __FUNCTION__ << "deleting myself " <<this << source();
    d->tlistener->invalidate();
    unsetSource();
    delete d;
}

void CuRNDReader::setSource(const QString &s)
{
    d->source = s;
    RndSourceBuilder rndsb;
    rndsb.fromSource(s);
    CuRNDActionReaderFactory rndrf;
    rndrf.setOptions(rndsb.options);
    printf("CuRNDReader.setSource: %s --> %s options %s\n", qstoc(s), qstoc(rndsb.name),
           rndsb.options.toString().c_str());
    qDebug() << __FUNCTION__ << "source" << s;
    d->cumbia_rnd->addAction(s.toStdString(), d->tlistener, rndrf);
}

QString CuRNDReader::source() const
{
    return d->source;
}

void CuRNDReader::unsetSource()
{
    d->cumbia_rnd->unlinkListener(d->source.toStdString(), CuRNDActionI::Reader, d->tlistener);
    d->source = QString();
}

void CuRNDReader::setOptions(const CuData &o)
{
    d->options = o;
}

CuData CuRNDReader::getOptions() const
{
    return d->options;
}

void CuRNDReader::sendData(const CuData &da)
{
    CuRNDActionI *a = d->cumbia_rnd->findAction(d->source.toStdString(), CuRNDActionI::Reader);
    if(a)
        a->sendData(da);
}

void CuRNDReader::getData(CuData &d_ino) const
{
    CuRNDActionI *a = d->cumbia_rnd->findAction(d->source.toStdString(), CuRNDActionI::Reader);
    if(a)
        a->getData(d_ino);
}
