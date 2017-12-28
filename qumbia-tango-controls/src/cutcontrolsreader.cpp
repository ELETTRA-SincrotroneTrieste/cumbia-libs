#include "cutcontrolsreader.h"
#include "cutcontrols-utils.h"
#include "cumbiatango.h"
#include "cutangoactionfactories.h"
#include "cutangoactioni.h"
#include "cudatalistener.h"

#include <assert.h>
#include <QCoreApplication>
#include <cumacros.h>

class CuTReaderFactoryPrivate
{
public:
    CuData r_options;
};

CuTReaderFactory::CuTReaderFactory()
{
    d = new CuTReaderFactoryPrivate();
}

CuTReaderFactory::~CuTReaderFactory()
{
    delete d;
}

void CuTReaderFactory::setOptions(const CuData &o)
{
    d->r_options = o;
}

CuData CuTReaderFactory::getOptions() const
{
    return d->r_options;
}

CuControlsReaderA *CuTReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuTControlsReader *r = new CuTControlsReader(c, l);
    r->setOptions(d->r_options);
    return r;
}

CuControlsReaderFactoryI *CuTReaderFactory::clone() const
{
    CuTReaderFactory *f = new CuTReaderFactory();
    f->d->r_options = d->r_options;
    return f;
}

class CuTControlsReaderPrivate
{
public:
    QString source;
    CumbiaTango *cumbia_tango;
    CuDataListener *tlistener;
    std::vector<std::string> attr_props;
    CuData ta_options;
};

CuTControlsReader::CuTControlsReader(Cumbia *cumbia_tango, CuDataListener *tl)
    : CuControlsReaderA(cumbia_tango, tl)
{
    assert(cumbia_tango->getType() == CumbiaTango::CumbiaTangoType);
    d = new CuTControlsReaderPrivate;
    d->cumbia_tango = static_cast<CumbiaTango *>(cumbia_tango);
    d->tlistener = tl;
}

CuTControlsReader::~CuTControlsReader()
{
    pdelete("~CuTControlsReader %p", this);
    d->tlistener->invalidate();
    unsetSource();
    delete d;
}

QString CuTControlsReader::source() const
{
    return d->source;
}

/** \brief Stops the reader.
 *
 * This must be called from the main thread.
 *
 * \par Developer note
 *      Calls CumbiaTango::unlinkListener for  CuTangoActionI::AttConfig and CuTangoActionI::Reader action types
 *      in a row.
 */
void CuTControlsReader::unsetSource()
{
    d->cumbia_tango->unlinkListener(d->source.toStdString(), CuTangoActionI::AttConfig, d->tlistener);
    d->cumbia_tango->unlinkListener(d->source.toStdString(), CuTangoActionI::Reader, d->tlistener);
    d->source = QString();
}

void CuTControlsReader::setOptions(const CuData &o)
{
    d->ta_options = o;
}

void CuTControlsReader::sendData(const CuData &data)
{
    CuTangoActionI *a = d->cumbia_tango->findAction(d->source.toStdString(), CuTangoActionI::Reader);
    if(a)
        a->sendData(data);
}

void CuTControlsReader::getData(CuData &d_ino) const
{
    CuTangoActionI *a = d->cumbia_tango->findAction(d->source.toStdString(), CuTangoActionI::Reader);
    if(a)
        a->getData(d_ino);
}

void CuTControlsReader::setSource(const QString &s)
{
    CuEpControlsUtils tcu;
    CuTangoAttConfFactory acf;
    CuTangoReaderFactory readf;
    acf.setOptions(d->ta_options);
    readf.setOptions(d->ta_options);
    d->source = tcu.replaceWildcards(s, qApp->arguments());
    d->cumbia_tango->addAction(d->source.toStdString(), d->tlistener, acf);
    d->cumbia_tango->addAction(d->source.toStdString(), d->tlistener, readf);
}
