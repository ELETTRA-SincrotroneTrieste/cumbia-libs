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

class CuActionFactoryServicePrivate;

class CuActionFactoryService : public CuServiceI
{
public:
    enum Type { CuActionFactoryServiceType = CuServices::User + 21 };

    CuActionFactoryService();

    virtual ~CuActionFactoryService();

    CuTangoActionI* registerAction(const std::string& src, const CuTangoActionFactoryI& f, CumbiaTango *ct);

    CuTangoActionI *findActive(const std::string &name, CuTangoActionI::Type at);

    CuTangoActionI *find(const std::string &name, CuTangoActionI::Type at);

    size_t count() const;

//    CuTangoActionI *findAction(const std::string& src, CuTangoActionI::Type at, CuDataListener *l);

    std::string getLastError() const;

    void unregisterAction(const std::string& src, CuTangoActionI::Type at);

    void cleanup();

    // CuServiceI interface
public:
    std::string getName() const;

    CuServices::Type getType() const;

private:
    CuActionFactoryServicePrivate *d;
};

#endif // DEVICEFACTORYSERVICE_H
