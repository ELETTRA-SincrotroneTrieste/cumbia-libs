#include "cuepcontrolswriter.h"
#include "cuepcontrols-utils.h"

#include <cumbiaepics.h>
#include <cuepactionfactories.h>
#include <QCoreApplication>
#include <cumacros.h>
#include <assert.h>

CuControlsWriterA *CuEpWriterFactory::create(Cumbia *c, CuDataListener *l) const
{
    return new CuEpControlsWriter(c, l);
}

CuControlsWriterFactoryI *CuEpWriterFactory::clone() const
{
    return new CuEpWriterFactory();
}

class CuEpControlsWriterPrivate
{
public:
    QString targets;
    CumbiaEpics *cumbia_epics;
    CuDataListener *tlistener;
    std::vector<std::string> attr_props;
};

CuEpControlsWriter::CuEpControlsWriter(Cumbia *cumbia_epics, CuDataListener *tl)
 : CuControlsWriterA(cumbia_epics, tl)
{
    assert(cumbia_epics->getType() == CumbiaEpics::CumbiaEpicsType);
    d = new CuEpControlsWriterPrivate;
    d->cumbia_epics = static_cast<CumbiaEpics *>(cumbia_epics);
    d->tlistener = tl;
}

CuEpControlsWriter::~CuEpControlsWriter()
{
    delete d;
}

QString CuEpControlsWriter::targets() const
{
    return d->targets;
}

void CuEpControlsWriter::clearTargets()
{
    d->cumbia_epics->unlinkListener(d->targets.toStdString(), CuEpicsActionI::AttConfig, d->tlistener);
    d->cumbia_epics->unlinkListener(d->targets.toStdString(), CuEpicsActionI::Writer, d->tlistener);
    d->targets = QString();
}

void CuEpControlsWriter::requestProperties(const QStringList &props)
{
    std::vector<std::string> vs;
    foreach(QString p, props)
        vs.push_back(p.toStdString());
    d->attr_props = vs;
}

void CuEpControlsWriter::execute()
{
    CuEpicsWriterFactory wtf;
    wtf.setWriteValue(getArgs());
    d->cumbia_epics->addAction(d->targets.toStdString(), d->tlistener, wtf);
}

void CuEpControlsWriter::setTargets(const QString &s)
{
    CuEpControlsUtils tcu;
    CuEpicsAttConfFactory att_conf_factory;
    att_conf_factory.setDesiredAttributeProperties(d->attr_props);
    d->targets = tcu.replaceWildcards(s, qApp->arguments());
    cuprintf("CuEpControlsWriter::setTargets: targets is: %s\n", qstoc(d->targets));
    d->cumbia_epics->addAction(d->targets.toStdString(), d->tlistener, att_conf_factory);
}
