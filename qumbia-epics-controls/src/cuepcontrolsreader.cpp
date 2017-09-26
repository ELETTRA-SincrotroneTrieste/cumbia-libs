#include "cuepcontrolsreader.h"
#include "cuepcontrols-utils.h"
#include "cumbiaepics.h"
#include "cuepactionfactories.h"
#include "cuepactioni.h"

#include <assert.h>
#include <QCoreApplication>
#include <cumacros.h>

class CuEpReaderFactoryPrivate
{
public:
    CuEpicsReadOptions r_options;
};

CuEpReaderFactory::CuEpReaderFactory()
{
    d = new CuEpReaderFactoryPrivate();
}

CuEpReaderFactory::~CuEpReaderFactory()
{
    delete d;
}

void CuEpReaderFactory::setReadOptions(const CuEpicsReadOptions &o)
{
    d->r_options = o;
}

CuControlsReaderA *CuEpReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    printf("creating with cumbia %p listener %p this %p\n", c, l, this);
    CuEpControlsReader *r = new CuEpControlsReader(c, l);
    r->setOptions(d->r_options);
    return r;
}

CuControlsReaderFactoryI *CuEpReaderFactory::clone() const
{
    CuEpReaderFactory *f = new CuEpReaderFactory();
    f->d->r_options = d->r_options;
    return f;
}

class CuEpControlsReaderPrivate
{
public:
    QString source;
    CumbiaEpics *cumbia_ep;
    CuDataListener *tlistener;
    std::vector<std::string> attr_props;
    CuEpicsReadOptions read_options;
};

CuEpControlsReader::CuEpControlsReader(Cumbia *cumbia_epics, CuDataListener *tl)
    : CuControlsReaderA(cumbia_epics, tl)
{
    assert(cumbia_epics->getType() == CumbiaEpics::CumbiaEpicsType);
    d = new CuEpControlsReaderPrivate;
    d->cumbia_ep = static_cast<CumbiaEpics *>(cumbia_epics);
    d->tlistener = tl;
}

CuEpControlsReader::~CuEpControlsReader()
{
    unsetSource();
    delete d;
}

QString CuEpControlsReader::source() const
{
    return d->source;
}

void CuEpControlsReader::unsetSource()
{
    d->cumbia_ep->unlinkListener(d->source.toStdString(), CuEpicsActionI::AttConfig, d->tlistener);
    d->cumbia_ep->unlinkListener(d->source.toStdString(), CuEpicsActionI::Reader, d->tlistener);
    d->source = QString();
}

void CuEpControlsReader::requestProperties(const QStringList &props)
{
    std::vector<std::string> vs;
    foreach(QString p, props)
        vs.push_back(p.toStdString());
    d->attr_props = vs;
}

void CuEpControlsReader::setOptions(const CuEpicsReadOptions &o)
{
    d->read_options = o;
}

void CuEpControlsReader::sendData(const CuData &data)
{
    CuEpicsActionI *a = d->cumbia_ep->findAction(d->source.toStdString(), CuEpicsActionI::Reader);
    if(a)
        a->sendData(data);
}

void CuEpControlsReader::getData(CuData &d_ino) const
{
    CuEpicsActionI *a = d->cumbia_ep->findAction(d->source.toStdString(), CuEpicsActionI::Reader);
    if(a)
        a->getData(d_ino);
}

void CuEpControlsReader::setSource(const QString &s)
{
    CuEpControlsUtils tcu;
 //   CuEpicsAttConfFactory acf;
    CuEpicsReaderFactory readf;
    readf.setOptions(d->read_options);
  //  acf.setDesiredAttributeProperties(d->attr_props);
    d->source = tcu.replaceWildcards(s, qApp->arguments());
 //   d->cumbia_ep->addAction(d->source.toStdString(), d->tlistener, acf);
    d->cumbia_ep->addAction(d->source.toStdString(), d->tlistener, readf);
}
