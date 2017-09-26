#include "cuepactionfactories.h"
#include "cumonitor.h"
#include "cuput.h"
#include "cuepconfiguration.h"
#include <cumacros.h>
#include <cadef.h>

CuEpicsReaderFactory::CuEpicsReaderFactory()
{
    options = new CuEpicsReadOptions(1000, CuMonitor::MonitorRefresh);
}

void CuEpicsReaderFactory::setOptions(const CuEpicsReadOptions &o)
{
    *options = o;
}

CuEpicsReaderFactory::~CuEpicsReaderFactory()
{
    delete options;
}

CuEpicsActionI *CuEpicsReaderFactory::create(const std::string &s, CumbiaEpics *ct) const
{
    CuMonitor* monitor = new CuMonitor(s, ct);
    monitor->setPeriod(options->period);
    monitor->setRefreshMode(options->mode);
    return monitor;
}

CuEpicsActionI::Type CuEpicsReaderFactory::getType() const
{
    return CuEpicsActionI::Reader;
}

CuEpicsWriterFactory::CuEpicsWriterFactory()
{

}

void CuEpicsWriterFactory::setWriteValue(const CuVariant &write_val)
{
    m_write_val = write_val;
}

CuEpicsActionI *CuEpicsWriterFactory::create(const std::string &s, CumbiaEpics *ct) const
{
    CuPut *w = new CuPut(s, ct);
    w->setWriteValue(m_write_val);
    return w;
}

CuEpicsActionI::Type CuEpicsWriterFactory::getType() const
{
    return CuEpicsActionI::Writer;
}

CuEpicsAttConfFactory::CuEpicsAttConfFactory()
{
    m_fetchAttHistory = false;
}

void CuEpicsAttConfFactory::fetchAttributeHistory(bool fetch)
{
    m_fetchAttHistory = fetch;
}

void CuEpicsAttConfFactory::setDesiredAttributeProperties(const std::vector<std::__cxx11::string> props)
{
    m_props = props;
}

CuEpicsActionI *CuEpicsAttConfFactory::create(const std::string &s, CumbiaEpics *ct) const
{
    CuEpConfiguration *w = new CuEpConfiguration(s, ct);
    w->setDesiredAttributeProperties(this->m_props);
    return w;
}

CuEpicsActionI::Type CuEpicsAttConfFactory::getType() const
{
    return CuEpicsActionI::AttConfig;
}

