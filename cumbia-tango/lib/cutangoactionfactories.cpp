#include "cutangoactionfactories.h"
#include "cutreader.h"
#include "cutwriter.h"
#include "cutattconfiguration.h"
#include "cutangoopt_builder.h"
#include <cumacros.h>
#include <tango.h>
#include <cudatatypes_ex.h>

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
    if(options.containsKey(CuXDType::Period) && options[CuXDType::Period].toInt() > 0)
        reader->setPeriod(options[CuXDType::Period].toInt());
    if(options.containsKey(CuXDType::RefreshMode))
        reader->setRefreshMode(static_cast<CuTReader::RefreshMode>(options[CuXDType::RefreshMode].toInt()));
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
    if(options.containsKey(CuXDType::FetchProperties))
        w->setDesiredAttributeProperties(options[CuXDType::FetchProperties].toStringVector());
    return w;
}

CuTangoActionI::Type CuTangoAttConfFactory::getType() const
{
    return CuTangoActionI::AttConfig;
}

