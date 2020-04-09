#include "cuwsactionfactories.h"
#include "cuwsactionreader.h"
#include "cuwsactionwriter.h"
#include "cuwsactionconf.h"
#include "cumbiawebsocket.h"

class CuWSClient;

class CuWSReaderFactoryPrivate {
    public:
};


void CuWSActionReaderFactory::setOptions(const CuData &o) {
    options = o;
}

CuWSActionReaderFactory::~CuWSActionReaderFactory() {

}

/** \brief creates and returns a CuWSActionReader, (that implements the CuWSActionI interface)
 *
 * @param s a string with the name of the source
 * @param ct a pointer to CumbiaWebSocket
 * @return a CuWSActionReader, that implements the CuWSActionI interface
 */
CuWSActionI *CuWSActionReaderFactory::create(const std::string &s, CuWSClient *cli, const QString &http_addr) const {
    CuWSActionReader* reader = new CuWSActionReader(s, cli, http_addr);
    // no refresh mode options, no period for websocket
    return reader;
}

CuWSActionI::Type CuWSActionReaderFactory::getType() const {
    return CuWSActionI::Reader;
}

void CuWSActionWriterFactory::setConfiguration(const CuData &conf) {
    configuration = conf;
}

void CuWSActionWriterFactory::setWriteValue(const CuVariant &write_val) {
    m_write_val = write_val;
}

CuWSActionWriterFactory::~CuWSActionWriterFactory() {

}

CuWSActionI *CuWSActionWriterFactory::create(const string &s, CuWSClient *cli, const QString &http_addr) const
{
    CuWsActionWriter *w = new CuWsActionWriter(s, cli, http_addr);
    w->setWriteValue(m_write_val);
    w->setConfiguration(configuration);
    return w;
}

CuWSActionI::Type CuWSActionWriterFactory::getType() const {
    return CuWSActionI::Writer;
}

void CuWSActionConfFactory::setOptions(const CuData &o) {
    options = o;
}

CuWSActionConfFactory::~CuWSActionConfFactory() {

}

CuWSActionI *CuWSActionConfFactory::create(const string &s, CuWSClient *cli, const QString &http_addr) const {
    return new CuWsActionConf(s, cli, http_addr);
}

CuWSActionI::Type CuWSActionConfFactory::getType() const {
    return CuWSActionI::WriterConfig;
}
