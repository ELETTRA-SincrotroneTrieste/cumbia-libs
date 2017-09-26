#include "cutcontrolswriter.h"
#include "cutcontrols-utils.h"

#include <cumbiatango.h>
#include <cutangoactionfactories.h>
#include <QCoreApplication>
#include <cumacros.h>
#include <assert.h>

CuControlsWriterA *CuTWriterFactory::create(Cumbia *c, CuDataListener *l) const
{
    return new CuTControlsWriter(c, l);
}

CuControlsWriterFactoryI *CuTWriterFactory::clone() const
{
    return new CuTWriterFactory();
}

class CuEpControlsWriterPrivate
{
public:
    QString targets;
    CumbiaTango *cumbia_tango;
    CuDataListener *tlistener;
    std::vector<std::string> attr_props;
};

CuTControlsWriter::CuTControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl)
 : CuControlsWriterA(cumbia_tango, tl)
{
    assert(cumbia_tango->getType() == CumbiaTango::CumbiaTangoType);
    d = new CuEpControlsWriterPrivate;
    d->cumbia_tango = static_cast<CumbiaTango *>(cumbia_tango);
    d->tlistener = tl;
}

CuTControlsWriter::~CuTControlsWriter()
{
    delete d;
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

void CuTControlsWriter::requestProperties(const QStringList &props)
{
    std::vector<std::string> vs;
    foreach(QString p, props)
        vs.push_back(p.toStdString());
    d->attr_props = vs;
}

void CuTControlsWriter::execute()
{
    CuTangoWriterFactory wtf;
    wtf.setWriteValue(getArgs());
    d->cumbia_tango->addAction(d->targets.toStdString(), d->tlistener, wtf);
}

void CuTControlsWriter::setTargets(const QString &s)
{
    CuEpControlsUtils tcu;
    CuTangoAttConfFactory att_conf_factory;
    att_conf_factory.setDesiredAttributeProperties(d->attr_props);
    d->targets = tcu.replaceWildcards(s, qApp->arguments());
    cuprintf("CuTControlsWriter::setTargets: targets is: %s\n", qstoc(d->targets));
    d->cumbia_tango->addAction(d->targets.toStdString(), d->tlistener, att_conf_factory);
}
