#include "cuepactionfactories.h"
#include "cumonitor.h"
#include "cuput.h"
#include "cuepconfiguration.h"
#include <cumacros.h>
#include <cadef.h>
#include <cudata.h>
#include <cudatatypes_ex.h>

CuEpicsReaderFactory::CuEpicsReaderFactory()
{
    options[CuXDType::Period] = 1000;
    options[CuXDType::RefreshMode] = CuMonitor::MonitorRefresh;
}

void CuEpicsReaderFactory::setOptions(const CuData &o)
{
    options = o;
}

CuEpicsReaderFactory::~CuEpicsReaderFactory()
{

}

CuEpicsActionI *CuEpicsReaderFactory::create(const std::string &s, CumbiaEpics *ct) const
{
    CuMonitor* monitor = new CuMonitor(s, ct);
    monitor->setPeriod(options[CuXDType::Period].toInt());
    monitor->setRefreshMode(static_cast<CuMonitor::RefreshMode>(options[CuXDType::RefreshMode].toInt()));
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

CuEpicsPropertyFactory::CuEpicsPropertyFactory()
{
    m_fetchAttHistory = false;
}

void CuEpicsPropertyFactory::setDesiredPVProperties(const std::vector<std::__cxx11::string> props)
{
    m_props = props;
}

CuEpicsActionI *CuEpicsPropertyFactory::create(const std::string &s, CumbiaEpics *ce) const
{
    CuEpConfiguration *w = new CuEpConfiguration(s, ce);
    w->setDesiredPVProperties(this->m_props);
    return w;
}

CuEpicsActionI::Type CuEpicsPropertyFactory::getType() const
{
    return CuEpicsActionI::PropConfig;
}

