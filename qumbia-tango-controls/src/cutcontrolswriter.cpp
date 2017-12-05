#include "cutcontrolswriter.h"
#include "cutcontrols-utils.h"

#include <cumbiatango.h>
#include <cutangoactionfactories.h>
#include <QCoreApplication>
#include <cumacros.h>
#include <assert.h>

class CuTWriterFactoryPrivate
{
public:
    CuData w_options;
};

CuTWriterFactory::CuTWriterFactory()
{
    d = new CuTWriterFactoryPrivate;
}

CuTWriterFactory::~CuTWriterFactory()
{
    delete d;
}

void CuTWriterFactory::setOptions(const CuData &o)
{
    d->w_options = o;
}

CuControlsWriterA *CuTWriterFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuTControlsWriter *wf = new CuTControlsWriter(c, l);
    wf->setOptions(d->w_options);
    return wf;
}

CuControlsWriterFactoryI *CuTWriterFactory::clone() const
{
    CuTWriterFactory *wf = new CuTWriterFactory();
    wf->d->w_options = d->w_options;
    return wf;
}

class CuTangoControlsWriterPrivate
{
public:
    QString targets;
    CumbiaTango *cumbia_tango;
    CuDataListener *tlistener;
    std::vector<std::string> attr_props;
    CuData w_options;
};

CuTControlsWriter::CuTControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl)
 : CuControlsWriterA(cumbia_tango, tl)
{
    assert(cumbia_tango->getType() == CumbiaTango::CumbiaTangoType);
    d = new CuTangoControlsWriterPrivate;
    d->cumbia_tango = static_cast<CumbiaTango *>(cumbia_tango);
    d->tlistener = tl;
}

CuTControlsWriter::~CuTControlsWriter()
{
    delete d;
}

void CuTControlsWriter::setOptions(const CuData &o)
{
    d->w_options = o;
}

QString CuTControlsWriter::targets() const
{
    return d->targets;
}

void CuTControlsWriter::clearTargets()
{
    d->cumbia_tango->unlinkListener(d->targets.toStdString(), CuTangoActionI::AttConfig, d->tlistener);
    d->cumbia_tango->unlinkListener(d->targets.toStdString(), CuTangoActionI::Writer, d->tlistener);
    d->targets = QString();
}

void CuTControlsWriter::execute()
{
    CuTangoWriterFactory wtf;
    wtf.setWriteValue(getArgs());
    d->cumbia_tango->addAction(d->targets.toStdString(), d->tlistener, wtf);
}

void CuTControlsWriter::sendData(const CuData & /* data */)
{
    printf("CuTControlsWriter.sendData: not implemented\n");
}

void CuTControlsWriter::getData(CuData & /*d_ino*/) const
{
    printf("CuTControlsWriter.getData: not implemented\n");
}

void CuTControlsWriter::setTargets(const QString &s)
{
    CuEpControlsUtils tcu;
    CuTangoAttConfFactory att_conf_factory;
    CuData options;
    options["fetch_props"] = d->attr_props;
    att_conf_factory.setOptions(options);
    d->targets = tcu.replaceWildcards(s, qApp->arguments());
    cuprintf("CuTControlsWriter::setTargets: targets is: %s\n", qstoc(d->targets));
    d->cumbia_tango->addAction(d->targets.toStdString(), d->tlistener, att_conf_factory);
}
