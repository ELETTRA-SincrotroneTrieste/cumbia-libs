#include "cutangoactionfactories.h"
#include "cutreader.h"
#include "cutwriter.h"
#include "cutattconfiguration.h"
#include <cumacros.h>
#include <tango.h>

CuTangoReaderFactory::CuTangoReaderFactory()
{

}

void CuTangoReaderFactory::setOptions(const CuData &o)
{
    options = o;
}

CuTangoReaderFactory::~CuTangoReaderFactory()
{

}

CuTangoActionI *CuTangoReaderFactory::create(const std::string &s, CumbiaTango *ct) const
{
    CuTReader* reader = new CuTReader(s, ct);
    if(options.containsKey("period") && options["period"].toInt() > 0)
        reader->setPeriod(options["period"].toInt());
    if(options.containsKey("refresh_mode"))
        reader->setRefreshMode(static_cast<CuTReader::RefreshMode>(options["refresh_mode"].toInt()));
    return reader;
}

CuTangoActionI::Type CuTangoReaderFactory::getType() const
{
    return CuTangoActionI::Reader;
}

CuTangoWriterFactory::CuTangoWriterFactory()
{

}

CuTangoWriterFactory::~CuTangoWriterFactory()
{

}

void CuTangoWriterFactory::setOptions(const CuData &o)
{
    options = o;
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

}

CuTangoAttConfFactory::~CuTangoAttConfFactory()
{

}

void CuTangoAttConfFactory::setOptions(const CuData &o)
{
    options = o;
}

CuTangoActionI *CuTangoAttConfFactory::create(const std::string &s, CumbiaTango *ct) const
{
    CuTAttConfiguration *w = new CuTAttConfiguration(s, ct);
    if(options.containsKey("fetch_props"))
        w->setDesiredAttributeProperties(options["fetch_props"].toStringVector());
    return w;
}

CuTangoActionI::Type CuTangoAttConfFactory::getType() const
{
    return CuTangoActionI::AttConfig;
}

