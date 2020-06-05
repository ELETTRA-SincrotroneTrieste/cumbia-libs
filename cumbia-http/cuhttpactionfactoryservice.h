#ifndef HTTPACTIONFACTORYSERVICE_H
#define HTTPACTIONFACTORYSERVICE_H

#include <cuservicei.h>
#include <cuhttpactiona.h>
#include <vector>

class CuHTTPActionFactoryI;
class QNetworkAccessManager;
class CuDataListener;
class CuHttpAuthManager;
class CuHttpChannelReceiver;

class CuHTTPActionFactoryServicePrivate;

/*! \brief implements CuServiceI interface and provides a service to register, unregister
 *         and find *Http actions*
 *
 * \note This service is internally used by CumbiaHttp and clients of the library should
 *       rarely need to use it.
 *
 * This class is a cumbia *service* (implements getName and getType). It is registered by
 * CumbiaTango at construction time through the cumbia service provider, namely CuServiceProvider:
 *
 * \code
   getServiceProvider()->registerService(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType), new CuActionFactoryService());
 * \endcode
 *
 * A reference to the service can be obtained through the Cumbia::getServiceProvider method:
 *
 * \code
   CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
 * \endcode
 *
 * Through the service provider, the service can be unregistered with CuServiceProvider::unregisterService
 *
 * Once the service is registered, new CuHTTPActionI (CuTReader, CuTWriter) can be added through
 * registerAction, removed with unregisterAction and searched with find and findActive.
 * The count method returns the number of registered actions.
 *
 * Tango sources (for readers) and targets (for writers) with the same name and type, share the same
 * CuHTTPActionI. See the registerAction documentation for further details.
 *
 */
class CuHTTPActionFactoryService : public CuServiceI
{
public:
    enum Type { CuHTTPActionFactoryServiceType = CuServices::User + 26 };

    CuHTTPActionFactoryService();

    virtual ~CuHTTPActionFactoryService();

    CuHTTPActionA* registerAction(const std::string& src,
                                  const CuHTTPActionFactoryI& f,
                                  QNetworkAccessManager *qnam,
                                  const QString &url,
                                  CuHttpChannelReceiver *cr, CuHttpAuthManager *authman);

    CuHTTPActionA *findActive(const std::string &name, CuHTTPActionA::Type at);

    std::vector<CuHTTPActionA *> find(const std::string &name, CuHTTPActionA::Type at);

    size_t count() const;

    CuHTTPActionA* unregisterAction(const std::string& src, CuHTTPActionA::Type at);

    void cleanup();

    // CuServiceI interface
public:
    std::string getName() const;

    CuServices::Type getType() const;

private:
    CuHTTPActionFactoryServicePrivate *d;
};

#endif // HTTPACTIONFACTORYSERVICE_H
