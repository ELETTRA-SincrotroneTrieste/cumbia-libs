#include "cuwsreader.h"
#include "cumbiawebsocket.h"
#include "cuwsreader.h"
#include "cuwsactionreader.h"
#include "cuwsactionfactories.h"
#include <cudatalistener.h>
#include <cudata.h>
#include <cumacros.h>
#include <QString>
#include <vector>
#include <string>
#include <QtDebug>

class CuWSReaderFactoryPrivate {
public:
    CuData options;

};

CuWSReaderFactory::CuWSReaderFactory()
{
    d = new CuWSReaderFactoryPrivate;
}

CuWSReaderFactory::~CuWSReaderFactory()
{
    delete d;
}

CuControlsReaderA *CuWSReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuWSReader *r = new CuWSReader(c, l);
    r->setOptions(d->options);
    return r;
}

CuControlsReaderFactoryI *CuWSReaderFactory::clone() const
{
    CuWSReaderFactory *f = new CuWSReaderFactory();
    f->setOptions(d->options);
    return f;
}

void CuWSReaderFactory::setOptions(const CuData &options)
{
    d->options = options;
}

/*! @private
 */
class CuWSReaderPrivate
{
public:
    QString source;
    CumbiaWebSocket *cumbia_ws;
    CuDataListener *tlistener;
    CuData options;
};

CuWSReader::CuWSReader(Cumbia *cumbia_ws, CuDataListener *tl)
    : CuControlsReaderA(cumbia_ws, tl)
{
    printf("CuWSReader constructor!!!!!!!!!!\n");
    assert(cumbia_ws->getType() == CumbiaWebSocket::CumbiaWSType);
    d = new CuWSReaderPrivate;
    d->cumbia_ws = static_cast<CumbiaWebSocket *>(cumbia_ws);
    d->tlistener = tl;
}

CuWSReader::~CuWSReader()
{
    qDebug() << __FUNCTION__ << "deleting myself " <<this << source();
    d->tlistener->invalidate();
    unsetSource();
    delete d;
}

void CuWSReader::setSource(const QString &s)
{
    d->source = s;
    CuWSActionReaderFactory wsrf;
    wsrf.setOptions(d->options);
    printf("CuWSReader.setSource: %s\n", qstoc(s));
    qDebug() << __FUNCTION__ << "source" << s;
    d->cumbia_ws->addAction(s.toStdString(), d->tlistener, wsrf);
}

QString CuWSReader::source() const
{
    return d->source;
}

void CuWSReader::unsetSource()
{
    d->cumbia_ws->unlinkListener(d->source.toStdString(), CuWSActionI::Reader, d->tlistener);
    d->source = QString();
}

void CuWSReader::setOptions(const CuData &o)
{
    d->options = o;
}

CuData CuWSReader::getOptions() const
{
    return d->options;
}

void CuWSReader::sendData(const CuData &)
{

}

void CuWSReader::getData(CuData &d_ino) const
{
    Q_UNUSED(d_ino);
}

