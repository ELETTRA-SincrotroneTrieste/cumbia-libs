#include "cuserviceprovider.h"
#include "cuservicei.h"
#include <set>

class CuServiceProviderPrivate {
public:
    std::map<CuServices::Type, CuServiceI *> services_map;
    std::set<CuServices::Type> shared_srvcs;
};

/*! \brief the class constructor
 *
 * no parameters needed.
 */
CuServiceProvider::CuServiceProvider() {
    d = new CuServiceProviderPrivate;
}

CuServiceProvider::~CuServiceProvider() {
    delete d;
}

/*! \brief register a service to the *service provider*
 *
 * @param name the service name as *CuServices::Type*
 * @param service an instance of a CuServiceI implementation
 *
 * The service can be retrieved with CuServiceProvider::get
 * The service can be unregistered with  CuServiceProvider::unregisterService
 *
 * The service is deleted automatically by Cumbia when the latter is deleted.
 * If a service *is shared across multiple Cumbia instances*, like CuEventLoop,
 * then registerSharedService must be called instead.
 */
void CuServiceProvider::registerService(CuServices::Type name, CuServiceI *service) {
    d->services_map[name] = service;
}

/*!
 * \brief Same as registerService, but mark the service as potentially shared across multiple
 *        Cumbia instances, thus preventing *service* from being automatically deleted.
 * \param name the service name, as CuServices::Type
 * \param service the service to register as a *shared service*.
 *
 * \note The object ownership is left to the caller, and *service* must be manually deleted.
 */
void CuServiceProvider::registerSharedService(CuServices::Type name, CuServiceI *service) {
    d->services_map[name] = service;
    d->shared_srvcs.insert(name);
}

/*! \brief unregister a service from the *service provider*
 *
 * @param name the name of the service as a value from CuServices::Type
 *
 * The service with the given name is removed from the service provider
 */
void CuServiceProvider::unregisterService(CuServices::Type name) {
    if(d->services_map.count(name) > 0)
        d->services_map.erase(name);
}

bool CuServiceProvider::isShared(CuServices::Type t) const {
    return d->shared_srvcs.count(t) > 0;
}

void CuServiceProvider::setShared(CuServices::Type t, bool shared) {
    if(shared) d->shared_srvcs.insert(t); else d->shared_srvcs.erase(t);
}

/*! \brief returns the service with the given name, if registered
 *
 * @param name the name of the service as a value from CuServices::Type
 * @return the instance of the service with that name, if registered, NULL otherwise
 *
 * \par warning
 * The method can return NULL if no service with the given name is registered.
 */
CuServiceI* CuServiceProvider::get(CuServices::Type name) const
{
    CuServiceI *service = NULL;
    std::map<CuServices::Type, CuServiceI *>::const_iterator it = d->services_map.find(name);
    if(it != d->services_map.end())
        service = it->second;
    return service;
}

/*! \brief returns the list of registered services
 *
 * @return the list of registered services
 */
std::list<CuServiceI *> CuServiceProvider::getServices() const
{
    std::list< CuServiceI *> l;
    for(std::map<CuServices::Type, CuServiceI *>::const_iterator it = d->services_map.begin(); it != d->services_map.end(); ++it)
        l.push_back(it->second);
    return l;
}
