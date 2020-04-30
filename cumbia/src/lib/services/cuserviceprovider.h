#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include <map>
#include <cuservices.h>
#include <list>

class CuServiceI;
class CuServiceProviderPrivate;

/*!
 * \brief The CuServiceProvider class is the *cumbia service provider* where
 *        *cumbia services* can be registered and have their references stored
 *        throughout their lifetime.
 *
 * *cumbia services* are classes that must implement the CuServiceI interface.
 * It forces them to provide a name as string and a type as CuServices::Type.
 * Services are registered to the *service provider* with the
 * CuServiceProvider::registerService, removed with the
 * CuServiceProvider::unregisterService and references to registered services
 * can be obtained by *type* (CuServices::Type) with the CuServiceProvider::get
 * method.
 *
 * The CuServiceProvider::getServices returns a std::list of the registered
 * services.
 *
 * Cumbia instantiates a CuServiceProvider in its class constructor (Cumbia::Cumbia)
 * and holds a reference to it throughout all Cumbia's lifetime (see Cumbia::finish).
 * Within Cumbia::finish, all services still registered (as returned by
 * CuServiceProvider::getServices) are first unregistered and then deleted, unless
 * the service had been registered with the *shared* option set to true
 * (see CuServiceProvider::registerService(CuServices::Type name, CuServiceI *service, bool shared) )
 *
 * Cumbia::getServiceProvider can be called to get a reference to the *service
 * provider*.
 *
 */
class CuServiceProvider
{
public:
    CuServiceProvider();

    virtual ~CuServiceProvider();

    void registerService(CuServices::Type name, CuServiceI *service);

    void registerSharedService(CuServices::Type name, CuServiceI *service);

    void unregisterService(CuServices::Type name);

    bool isShared(CuServices::Type t) const;

    void setShared(CuServices::Type t, bool shared);

    CuServiceI *get(CuServices::Type name) const;

    std::list<CuServiceI *>getServices() const;

private:
    CuServiceProviderPrivate *d;
};

#endif // SERVICEPROVIDER_H
