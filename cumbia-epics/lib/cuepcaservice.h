#ifndef DEVICEFACTORYSERVICE_H
#define DEVICEFACTORYSERVICE_H

#include <cuservicei.h>
#include <map>
#include <string>
#include <mutex>

class TDevice;

class CuEpCAService : public CuServiceI
{
public:
    enum Type { CuEpicsChannelAccessServiceType = CuServices::User + 30 };

    CuEpCAService();

    virtual ~CuEpCAService();

    int getResult() const;

    std::string getStatus() const;

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    int m_result;
};

#endif // DEVICEFACTORYSERVICE_H
