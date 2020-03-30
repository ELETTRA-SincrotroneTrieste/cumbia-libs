#include "cuwsactionfactories.h"
#include "cuwsactionreader.h"
#include "cumbiawebsocket.h"

class CuWSClient;

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
 */
CuWSActionI *CuWSActionReaderFactory::create(const std::string &s, CuWSClient *cli, const QString &http_addr) const
{
    CuWSActionReader* reader = new CuWSActionReader(s, cli, http_addr);
    // no refresh mode options, no period for websocket
    return reader;
}

CuWSActionI::Type CuWSActionReaderFactory::getType() const
{
    return CuWSActionI::Reader;
}


