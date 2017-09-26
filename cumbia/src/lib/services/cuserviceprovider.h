#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include <map>
#include <cuservices.h>
#include <list>

class CuServiceI;


class CuServiceProvider
{
public:
    CuServiceProvider();

    void registerService(CuServices::Type name, CuServiceI *service);

    void unregisterService(CuServices::Type name);

    CuServiceI *get(CuServices::Type name) const;

    std::list<CuServiceI *>getServices() const;

private:
    std::map<CuServices::Type, CuServiceI *> mServicesMap;
};

#endif // SERVICEPROVIDER_H
