#include "cuwscontrolsreader.h"
#include "cumbiawebsocket.h"
#include "cuwscontrolsreader.h"
#include "cuwsactionreader.h"
#include "cuwsactionfactories.h"
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
    CuWsControlsReader *r = new CuWsControlsReader(c, l);
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

CuWsControlsReader::CuWsControlsReader(Cumbia *cumbia, CuDataListener *tl)
    : CuControlsReaderA(cumbia, tl)
{
    assert(cumbia->getType() == CumbiaWebSocket::CumbiaWSType);
    d = new CuWSReaderPrivate;
    d->cumbia_ws = static_cast<CumbiaWebSocket *>(cumbia);
    d->tlistener = tl;
}

CuWsControlsReader::~CuWsControlsReader()
{
    d->tlistener->invalidate();
    unsetSource();
    delete d;
}

void CuWsControlsReader::setSource(const QString &s) {
    d->source = s;
    QList<QuReplaceWildcards_I *>rwis = d->cumbia_ws->getReplaceWildcard_Ifaces();
    // d->source is equal to 's' if no replacement is made
    for(int i = 0; i < rwis.size() && d->source == s; i++) // leave loop if s != d->source (=replacement made)
        d->source = rwis[i]->replaceWildcards(s, qApp->arguments());

    CuWSActionConfFactory wswconff;
    wswconff.setOptions(d->options);
    d->cumbia_ws->addAction(d->source.toStdString(), d->tlistener, wswconff);

    CuWSActionReaderFactory wsrf;
    wsrf.setOptions(d->options);
    d->cumbia_ws->addAction(d->source.toStdString(), d->tlistener, wsrf);
}

QString CuWsControlsReader::source() const
{
    return d->source;
}

void CuWsControlsReader::unsetSource()
{
    d->cumbia_ws->unlinkListener(d->source.toStdString(), CuWSActionI::Reader, d->tlistener);
    d->source = QString();
}

void CuWsControlsReader::setOptions(const CuData &o)
{
    d->options = o;
}

CuData CuWsControlsReader::getOptions() const
{
    return d->options;
}

void CuWsControlsReader::sendData(const CuData &)
{

}

void CuWsControlsReader::getData(CuData &d_ino) const
{
    Q_UNUSED(d_ino);
}

