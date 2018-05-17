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

class CuEpicsActionFactoryServicePrivate;

class CuEpicsActionFactoryService : public CuServiceI
{
public:
    enum Type { CuActionFactoryServiceType = CuServices::User + 21 };

    CuEpicsActionFactoryService();

    virtual ~CuEpicsActionFactoryService();

    CuEpicsActionI* registerAction(const std::string& src, const CuEpicsActionFactoryI& f, CumbiaEpics *ce);

    CuEpicsActionI *find(const std::string &name, CuEpicsActionI::Type at);

    CuEpicsActionI *findActive(const std::string &src, CuEpicsActionI::Type at);

    std::string getLastError() const;

    void unregisterAction(const std::string& src, CuEpicsActionI::Type at);

    void deleteActions();

    // CuServiceI interface
public:
    std::string getName() const;

    CuServices::Type getType() const;

private:
    CuEpicsActionFactoryServicePrivate *d;
};

#endif // DEVICEFACTORYSERVICE_H
