#include "curndactionfactories.h"
#include "curndactionreader.h"

class CuRNDReaderFactoryPrivate {
    public:

};

CuRNDActionReaderFactory::CuRNDActionReaderFactory()
{

}

void CuRNDActionReaderFactory::setOptions(const CuData &o)
{
    options = o;
}

CuRNDActionReaderFactory::~CuRNDActionReaderFactory()
{

}

/** \brief creates and returns a CuRNDActionReader, (that implements the CuRNDActionI interface)
 *
 * @param s a string with the name of the source
 * @param ct a pointer to CumbiaWebSocket
 * @return a CuRNDActionReader, that implements the CuRNDActionI interface
 *
 * The source and the CumbiaWebSocket reference are passed to the CuRNDActionReader
 * If options have been set, normally through CuRNDActionReaderFactory::create,
 * they are used to configure the CuRNDActionReader.
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
CuRNDActionI *CuRNDActionReaderFactory::create(const std::string &s, CumbiaWebSocket *c_rnd) const
{
    CuRNDActionReader* reader = new CuRNDActionReader(s, c_rnd);
    if(options.containsKey("period") && options["period"].toInt() > 0)
        reader->setPeriod(options["period"].toInt());
    if(options.containsKey("refresh_mode"))
        reader->setRefreshMode(static_cast<CuRNDActionReader::RefreshMode>(options["refresh_mode"].toInt()));
    return reader;
}

CuRNDActionI::Type CuRNDActionReaderFactory::getType() const
{
    return CuRNDActionI::Reader;
}


