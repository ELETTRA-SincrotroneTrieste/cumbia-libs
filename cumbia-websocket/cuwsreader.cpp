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
    return r;
}

CuControlsReaderFactoryI *CuWSReaderFactory::clone() const
{
    CuWSReaderFactory *f = new CuWSReaderFactory();
    return f;
}

void CuWSReaderFactory::setOptions(const CuData &options)
{
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
    pdelete("~CuWSReader %p", this);
    d->tlistener->invalidate();
    unsetSource();
    delete d;
}

void CuWSReader::setSource(const QString &s)
{
    d->source = s;
    CuWSActionReaderFactory wsrf;
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
    d->source = QString();
}


void CuWSReader::sendData(const CuData &d)
{
}

void CuWSReader::getData(CuData &d_ino) const
{
}
