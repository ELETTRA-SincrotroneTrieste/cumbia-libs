#include "cuwsactionfactories.h"
#include "cuwsactionreader.h"

class CuWSReaderFactoryPrivate {
    public:

};

CuWSActionReaderFactory::CuWSActionReaderFactory()
{

}

void CuWSActionReaderFactory::setOptions(const CuData &o)
{
    options = o;
}

CuWSActionReaderFactory::~CuWSActionReaderFactory()
{

}

/** \brief creates and returns a CuWSActionReader, (that implements the CuWSActionI interface)
 *
 * @param s a string with the name of the source
 * @param ct a pointer to CumbiaWebSocket
 * @return a CuWSActionReader, that implements the CuWSActionI interface
 *
 * The source and the CumbiaWebSocket reference are passed to the CuWSActionReader
 * If options have been set, normally through CuWSActionReaderFactory::create,
 * they are used to configure the CuWSActionReader.
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
CuWSActionI *CuWSActionReaderFactory::create(const std::string &s, CumbiaWebSocket *c_ws) const
{
    CuWSActionReader* reader = new CuWSActionReader(s, c_ws);
    if(options.containsKey("period") && options["period"].toInt() > 0)
        reader->setPeriod(options["period"].toInt());
    if(options.containsKey("refresh_mode"))
        reader->setRefreshMode(static_cast<CuWSActionReader::RefreshMode>(options["refresh_mode"].toInt()));
    return reader;
}

CuWSActionI::Type CuWSActionReaderFactory::getType() const
{
    return CuWSActionI::Reader;
}


