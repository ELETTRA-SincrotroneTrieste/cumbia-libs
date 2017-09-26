#include "cutangoactionfactories.h"
#include "cutreader.h"
#include "cutwriter.h"
#include "cutattconfiguration.h"
#include <cumacros.h>
#include <tango.h>

CuTangoReaderFactory::CuTangoReaderFactory()
{
    options = new CuTangoReadOptions(1000, CuTReader::ChangeEventRefresh);
}

void CuTangoReaderFactory::setOptions(const CuTangoReadOptions &o)
{
    *options = o;
}

CuTangoReaderFactory::~CuTangoReaderFactory()
{
    delete options;
}

CuTangoActionI *CuTangoReaderFactory::create(const std::string &s, CumbiaTango *ct) const
{
    CuTReader* reader = new CuTReader(s, ct);
    reader->setPeriod(options->period);
    reader->setRefreshMode(options->mode);
    return reader;
}

CuTangoActionI::Type CuTangoReaderFactory::getType() const
{
    return CuTangoActionI::Reader;
}

CuTangoWriterFactory::CuTangoWriterFactory()
{

}

void CuTangoWriterFactory::setWriteValue(const CuVariant &write_val)
{
    m_write_val = write_val;
}

CuTangoActionI *CuTangoWriterFactory::create(const std::string &s, CumbiaTango *ct) const
{
    CuTWriter *w = new CuTWriter(s, ct);
    w->setWriteValue(m_write_val);
    return w;
}

CuTangoActionI::Type CuTangoWriterFactory::getType() const
{
    return CuTangoActionI::Writer;
}

CuTangoAttConfFactory::CuTangoAttConfFactory()
{
    m_fetchAttHistory = false;
}

void CuTangoAttConfFactory::fetchAttributeHistory(bool fetch)
{
    m_fetchAttHistory = fetch;
}

void CuTangoAttConfFactory::setDesiredAttributeProperties(const std::vector<std::__cxx11::string> props)
{
    m_props = props;
}

CuTangoActionI *CuTangoAttConfFactory::create(const std::string &s, CumbiaTango *ct) const
{
    CuTAttConfiguration *w = new CuTAttConfiguration(s, ct);
    w->setDesiredAttributeProperties(this->m_props);
    return w;
}

CuTangoActionI::Type CuTangoAttConfFactory::getType() const
{
    return CuTangoActionI::AttConfig;
}

