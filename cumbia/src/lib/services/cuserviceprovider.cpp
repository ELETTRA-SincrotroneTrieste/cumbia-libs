#include "cuserviceprovider.h"
#include "cuservicei.h"

CuServiceProvider::CuServiceProvider()
{

}

void CuServiceProvider::registerService(CuServices::Type name, CuServiceI *service)
{
    mServicesMap[name] = service;
}

void CuServiceProvider::unregisterService(CuServices::Type name)
{
    if(mServicesMap.count(name) > 0)
        mServicesMap.erase(name);
}

CuServiceI* CuServiceProvider::get(CuServices::Type name) const
{
    CuServiceI *service = NULL;
    std::map<CuServices::Type, CuServiceI *>::const_iterator it = mServicesMap.find(name);
    if(it != mServicesMap.end())
        service = it->second;
    return service;
}

std::list<CuServiceI *> CuServiceProvider::getServices() const
{
    std::list< CuServiceI *> l;
    for(std::map<CuServices::Type, CuServiceI *>::const_iterator it = mServicesMap.begin(); it != mServicesMap.end(); ++it)
        l.push_back(it->second);
    return l;
}
