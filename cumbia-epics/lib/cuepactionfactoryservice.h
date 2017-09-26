#ifndef ACTIONFACTORYSERVICE_H
#define ACTIONFACTORYSERVICE_H

#include <cuservicei.h>
#include <cuepactioni.h>
#include <vector>

class TDevice;
class CuEpicsActionI;
class CuEpicsActionFactoryI;
class CumbiaEpics;
class CuDataListener;

class CuActionFactoryServicePrivate;

class CuActionFactoryService : public CuServiceI
{
public:
    enum Type { CuActionFactoryServiceType = CuServices::User + 21 };

    CuActionFactoryService();

    virtual ~CuActionFactoryService();

    CuEpicsActionI* registerAction(const std::string& src, const CuEpicsActionFactoryI& f, CumbiaEpics *ct);

    CuEpicsActionI *findAction(const std::string &name, CuEpicsActionI::Type at);

    CuEpicsActionI *findAction(const std::string& src, CuEpicsActionI::Type at, CuDataListener *l);

    std::string getLastError() const;

    void unregisterAction(const std::string& src, CuEpicsActionI::Type at);

    void deleteActions();

    // CuServiceI interface
public:
    std::string getName() const;

    CuServices::Type getType() const;

private:
    CuActionFactoryServicePrivate *d;
};

#endif // DEVICEFACTORYSERVICE_H
