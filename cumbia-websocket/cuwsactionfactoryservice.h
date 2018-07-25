#ifndef ACTIONFACTORYSERVICE_H
#define ACTIONFACTORYSERVICE_H

#include <cuservicei.h>
#include <cuwsactioni.h>
#include <vector>

class CuWSActionFactoryI;
class CumbiaWebSocket;
class CuDataListener;

class CuWSActionFactoryServicePrivate;

/*! \brief implements CuServiceI interface and provides a service to register, unregister
 *         and find *Websocket actions*
 *
 * \note This service is internally used by CumbiaWebSocket and clients of the library should
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
 * Once the service is registered, new CuWSActionI (CuTReader, CuTWriter) can be added through
 * registerAction, removed with unregisterAction and searched with find and findActive.
 * The count method returns the number of registered actions.
 *
 * Tango sources (for readers) and targets (for writers) with the same name and type, share the same
 * CuWSActionI. See the registerAction documentation for further details.
 *
 */
class CuWSActionFactoryService : public CuServiceI
{
public:
    enum Type { CuWSActionFactoryServiceType = CuServices::User + 25 };

    CuWSActionFactoryService();

    virtual ~CuWSActionFactoryService();

    CuWSActionI* registerAction(const std::string& src, const CuWSActionFactoryI& f, CumbiaWebSocket *ct);

    CuWSActionI *findActive(const std::string &name, CuWSActionI::Type at);

    std::vector<CuWSActionI *> find(const std::string &name, CuWSActionI::Type at);

    size_t count() const;

    void unregisterAction(const std::string& src, CuWSActionI::Type at);

    void cleanup();

    // CuServiceI interface
public:
    std::string getName() const;

    CuServices::Type getType() const;

private:
    CuWSActionFactoryServicePrivate *d;
};

#endif // DEVICEFACTORYSERVICE_H
