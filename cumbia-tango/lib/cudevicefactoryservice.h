#ifndef DEVICEFACTORYSERVICE_H
#define DEVICEFACTORYSERVICE_H

#include <cuservicei.h>
#include <map>
#include <string>
#include <mutex>

class TDevice;

class CuDeviceFactoryService : public CuServiceI
{
public:
    enum Type { CuDeviceFactoryServiceType = CuServices::User + 20 };

    CuDeviceFactoryService();

    virtual ~CuDeviceFactoryService();

    TDevice *getDevice(const std::string &name);

    TDevice *findDevice(const std::string &name);

    void removeDevice(const std::string& name);

    // CuServiceI interface
public:
    std::__cxx11::string getName() const;
    CuServices::Type getType() const;

private:
    std::map<std::string, TDevice*> m_devmap;
    std::mutex m_mutex;
};

#endif // DEVICEFACTORYSERVICE_H
