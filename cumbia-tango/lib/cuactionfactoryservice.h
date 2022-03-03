#ifndef ACTIONFACTORYSERVICE_H
#define ACTIONFACTORYSERVICE_H

#include <cuservicei.h>
#include <cutangoactioni.h>
#include <vector>

class TDevice;
class CuTangoActionI;
class CuTangoActionFactoryI;
class CumbiaTango;
class CuDataListener;
class CuActionFactoryServiceImpl_Base; // defined in cuactionfactoryservice_impls.h

/*! \brief implements CuServiceI interface and provides a service to register, unregister
 *         and find *Tango actions* (CuTReader, CuTWriter - CuTangoActionI implementations)
 *
 * \note This service is internally used by CumbiaTango and clients of the library should
 *       rarely need to use it.
 *
 * \note Methods shall be called from the same thread.
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
 * Once the service is registered, new CuTangoActionI (CuTReader, CuTWriter) can be added through
 * registerAction, removed with unregisterAction and searched with find and findActive.
 * The count method returns the number of registered actions.
 *
 * Tango sources (for readers) and targets (for writers) with the same name and type, share the same
 * CuTangoActionI. See the registerAction documentation for further details.
 *
 */
class CuActionFactoryService : public CuServiceI
{
public:
    enum Type { CuActionFactoryServiceType = CuServices::User + 21 };

    CuActionFactoryService();
    virtual ~CuActionFactoryService();

    virtual CuTangoActionI* registerAction(const std::string& src, const CuTangoActionFactoryI& f, CumbiaTango *ct, bool *isnew);
    virtual CuTangoActionI *find(const std::string &name, CuTangoActionI::Type at);
    virtual size_t count() const;
    virtual void unregisterAction(const std::string& src, CuTangoActionI::Type at);
    virtual void cleanup();

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    CuActionFactoryServiceImpl_Base *impl;
};


#endif // DEVICEFACTORYSERVICE_H
