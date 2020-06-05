#include "cuepcontrolsreader.h"
#include "cuepcontrols-utils.h"
#include "cumbiaepics.h"
#include "cuepactionfactories.h"
#include "cuepactioni.h"

#include <cudatalistener.h>
#include <assert.h>
#include <QCoreApplication>
#include <cumacros.h>

class CuEpReaderFactoryPrivate
{
public:
    CuData r_options;
};

CuEpReaderFactory::CuEpReaderFactory()
{
    d = new CuEpReaderFactoryPrivate();
}

CuEpReaderFactory::~CuEpReaderFactory()
{
    delete d;
}

CuControlsReaderA *CuEpReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
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

void CuEpReaderFactory::setOptions(const CuData &options)
{
    d->r_options = options;
}

class CuEpControlsReaderPrivate
{
public:
    QString source;
    CumbiaEpics *cumbia_ep;
    CuDataListener *tlistener;
    std::vector<std::string> attr_props;
    CuData read_options;
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
    pdelete("CuEpControlsReader %p", this);
    unsetSource();
    delete d;
}

QString CuEpControlsReader::source() const
{
    return d->source;
}

void CuEpControlsReader::unsetSource()
{
    d->cumbia_ep->unlinkListener(d->source.toStdString(), CuEpicsActionI::Reader, d->tlistener);
    d->source = QString();
}

void CuEpControlsReader::setOptions(const CuData &o)
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
    // if needs_caget is true, a caget is called a CuEpicsPropertyFactory is used to perform an explicit read
    // for configuration purposes
    // needs_caget will be false if d->cumbia_ep->addAction with CuEpicsReaderFactory returns false
    // (= no action with the same source already monitored)
    bool needs_caget = true;
    CuEpControlsUtils tcu;
    CuEpicsPropertyFactory acf;
    d->source = tcu.replaceWildcards(s, qApp->arguments());
    if(!d->read_options.value("properties-only").toBool()) {
        CuEpicsReaderFactory readf;
        readf.setOptions(d->read_options);
        needs_caget = d->cumbia_ep->addAction(d->source.toStdString(), d->tlistener, readf);
    }
    if(needs_caget) {
        d->cumbia_ep->addAction(d->source.toStdString(), d->tlistener, acf);
    }
}
