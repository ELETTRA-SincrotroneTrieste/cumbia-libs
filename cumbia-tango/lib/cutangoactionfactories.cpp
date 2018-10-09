#include "cutangoactionfactories.h"
#include "cutreader.h"
#include "cutwriter.h"
#include "cutconfiguration.h"
#include <cumacros.h>
#include <tango.h>

CuTangoReaderFactory::CuTangoReaderFactory()
{

}

/*! \brief set the options on the reader factory in order to configure the actual reader
 *  within create.
 *
 * @param o a CuData compiled with the following options:
 * \li "period" an integer, in milliseconds, for the polling period. Converted with: CuVariant::toInt
 * \li "refresh_mode": an integer defining the Tango refresh mode. Converted with: CuVariant::toInt
 *
 * \par note
 * Please use the CuTangoOptBuilder class rather than filling in the options manually.
 *
 * @see CuTangoOptBuilder
 */
void CuTangoReaderFactory::setOptions(const CuData &o)
{
    options = o;
}

CuTangoReaderFactory::~CuTangoReaderFactory()
{

}

/** \brief creates and returns a CuTReader, (that implements the CuTangoActionI interface)
 *
 * @param s a string with the name of the source
 * @param ct a pointer to CumbiaTango
 * @return a CuTReader, that implements the CuTangoActionI interface
 *
 * The source and the CumbiaTango reference are passed to the CuTReader
 * If options have been set, normally through CuTangoReaderFactory::create,
 * they are used to configure the CuTReader.
 *
 * \par options
 * The recognised options are:
 * \li "period" an integer, in milliseconds, for the polling period. Converted with: CuVariant::toInt
 * \li "refresh_mode": an integer defining the Tango refresh mode. Converted with: CuVariant::toInt
 *
 * \par note
 * Please use the CuTangoOptBuilder class rather than filling in the options manually.
 *
 * @see CuTangoOptBuilder
 */
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

// Configuration base class
//
CuTConfFactoryBase::~CuTConfFactoryBase() {

}

void CuTConfFactoryBase::setOptions(const CuData &o)
{
    m_options = o;
}

CuData CuTConfFactoryBase::options() const
{
    return m_options;
}

// Configuration: Reader
CuTangoActionI *CuTReaderConfFactory::create(const std::string &s, CumbiaTango *ct) const
{
    CuTConfiguration *w = new CuTConfiguration(s, ct, CuTangoActionI::ReaderConfig);
    const CuData& op = options();
    if(op.containsKey("fetch_props"))
        w->setDesiredAttributeProperties(op["fetch_props"].toStringVector());
    return w;
}

CuTangoActionI::Type CuTReaderConfFactory::getType() const
{
    return CuTangoActionI::ReaderConfig;
}

// Configuration: Writer
CuTangoActionI *CuTWriterConfFactory::create(const string &s, CumbiaTango *ct) const
{
    CuTConfiguration *w = new CuTConfiguration(s, ct, CuTangoActionI::WriterConfig);
    const CuData& op = options();
    if(op.containsKey("fetch_props"))
        w->setDesiredAttributeProperties(op["fetch_props"].toStringVector());
    return w;
}

CuTangoActionI::Type CuTWriterConfFactory::getType() const
{
    return CuTangoActionI::WriterConfig;
}
