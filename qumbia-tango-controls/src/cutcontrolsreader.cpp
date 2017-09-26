#include "cutcontrolsreader.h"
#include "cutcontrols-utils.h"
#include "cumbiatango.h"
#include "cutangoactionfactories.h"
#include "cutangoactioni.h"

#include <assert.h>
#include <QCoreApplication>
#include <cumacros.h>

class CuTReaderFactoryPrivate
{
public:
    CuTangoReadOptions r_options;
};

CuTReaderFactory::CuTReaderFactory()
{
    d = new CuTReaderFactoryPrivate();
}

CuTReaderFactory::~CuTReaderFactory()
{
    delete d;
}

void CuTReaderFactory::setReadOptions(const CuTangoReadOptions &o)
{
    d->r_options = o;
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
    CuTangoReadOptions read_options;
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
    unsetSource();
    delete d;
    printf("~CuTControlsReader %p out\n", this);
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
    printf("\e[1;36;2mCuTControlsReader.unsetSource on %s listener %p\e[0m\n", qstoc(d->source), d->tlistener);
    d->cumbia_tango->unlinkListener(d->source.toStdString(), CuTangoActionI::AttConfig, d->tlistener);
    d->cumbia_tango->unlinkListener(d->source.toStdString(), CuTangoActionI::Reader, d->tlistener);
    d->source = QString();
}

void CuTControlsReader::requestProperties(const QStringList &props)
{
    std::vector<std::string> vs;
    foreach(QString p, props)
        vs.push_back(p.toStdString());
    d->attr_props = vs;
}

void CuTControlsReader::setOptions(const CuTangoReadOptions &o)
{
    d->read_options = o;
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
    readf.setOptions(d->read_options);
    acf.setDesiredAttributeProperties(d->attr_props);
    d->source = tcu.replaceWildcards(s, qApp->arguments());
    d->cumbia_tango->addAction(d->source.toStdString(), d->tlistener, acf);
    d->cumbia_tango->addAction(d->source.toStdString(), d->tlistener, readf);
}
