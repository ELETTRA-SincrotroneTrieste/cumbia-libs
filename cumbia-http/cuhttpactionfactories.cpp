#include "cuhttpactionfactories.h"
#include "cuhttpactionreader.h"
#include "cuhttpactionwriter.h"
#include "cuhtttpactionconf.h"
#include "cumbiahttp.h"
#include "cuhttpchannelreceiver.h"

#include <QNetworkAccessManager>

class CuHttpAuthManager;
class CuHTTPClient;

class CuHTTPReaderFactoryPrivate {
public:
    CuData options;
};


void CuHTTPActionReaderFactory::setOptions(const CuData &o) {
    d->options = o;
}

CuHTTPActionReaderFactory::CuHTTPActionReaderFactory() {
    d  = new CuHTTPReaderFactoryPrivate;
}

CuHTTPActionReaderFactory::~CuHTTPActionReaderFactory() {
    delete d;
}

/** \brief creates and returns a CuHTTPActionReader, (that implements the CuHTTPActionI interface)
 *
 * @param s a string with the name of the source
 * @param ct a pointer to CumbiaHttp
 * @return a CuHTTPActionReader, that implements the CuHTTPActionI interface
 */
CuHTTPActionA *CuHTTPActionReaderFactory::create(const CuHTTPSrc &s,
                                                 QNetworkAccessManager *nam,
                                                 const QString &http_addr,
                                                 CuHttpAuthManager *authman,
                                                 CuHttpChannelReceiver* cr) const {
    CuHTTPActionReader* reader = new CuHTTPActionReader(s, cr, nam, http_addr, authman);
    reader->setOptions(d->options);
    // no refresh mode options, no period for http
    return reader;
}

CuHTTPActionA::Type CuHTTPActionReaderFactory::getType() const {
    return CuHTTPActionA::Reader;
}

void CuHTTPActionWriterFactory::setConfiguration(const CuData &conf) {
    configuration = conf;
}

void CuHTTPActionWriterFactory::setWriteValue(const CuVariant &write_val) {
    m_write_val = write_val;
}

CuHTTPActionWriterFactory::~CuHTTPActionWriterFactory() {

}

CuHTTPActionA *CuHTTPActionWriterFactory::create(const CuHTTPSrc &s,
                                                 QNetworkAccessManager *qnam,
                                                 const QString &http_addr,
                                                 CuHttpAuthManager *aman,
                                                 CuHttpChannelReceiver* ) const
{
    CuHttpActionWriter *w = new CuHttpActionWriter(s, qnam, http_addr, aman);
    w->setWriteValue(m_write_val);
    w->setConfiguration(configuration);
    return w;
}

CuHTTPActionA::Type CuHTTPActionWriterFactory::getType() const {
    return CuHTTPActionA::Writer;
}

void CuHTTPActionConfFactory::setOptions(const CuData &o) {
    options = o;
}

CuHTTPActionConfFactory::~CuHTTPActionConfFactory() {

}

CuHTTPActionA *CuHTTPActionConfFactory::create(const CuHTTPSrc &s,
                                               QNetworkAccessManager *qnam,
                                               const QString &http_addr,
                                               CuHttpAuthManager *aman,
                                               CuHttpChannelReceiver* ) const {
    return new CuHttpActionConf(s, qnam, http_addr, aman);
}

CuHTTPActionA::Type CuHTTPActionConfFactory::getType() const {
    return CuHTTPActionA::Config;
}

