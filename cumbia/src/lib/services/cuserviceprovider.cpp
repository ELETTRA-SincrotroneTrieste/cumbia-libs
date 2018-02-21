#include "cuserviceprovider.h"
#include "cuservicei.h"

/*! \brief the class constructor
 *
 * no parameters needed.
 */
CuServiceProvider::CuServiceProvider()
{

}

/*! \brief register a service to the *service provider*
 *
 * @param name the service name as *CuServices::Type*
 * @param service an instance of a CuServiceI implementation
 *
 * The service can be retrieved with CuServiceProvider::get
 * The service can be unregistered with  CuServiceProvider::unregisterService
 */
void CuServiceProvider::registerService(CuServices::Type name, CuServiceI *service)
{
    mServicesMap[name] = service;
}

/*! \brief unregister a service from the *service provider*
 *
 * @param name the name of the service as a value from CuServices::Type
 *
 * The service with the given name is removed from the service provider
 */
void CuServiceProvider::unregisterService(CuServices::Type name)
{
    if(mServicesMap.count(name) > 0)
        mServicesMap.erase(name);
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
    std::map<CuServices::Type, CuServiceI *>::const_iterator it = mServicesMap.find(name);
    if(it != mServicesMap.end())
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
    for(std::map<CuServices::Type, CuServiceI *>::const_iterator it = mServicesMap.begin(); it != mServicesMap.end(); ++it)
        l.push_back(it->second);
    return l;
}
