#ifndef CUTHREADSERVICE_H
#define CUTHREADSERVICE_H

#include <cuservices.h>
#include <cuservicei.h>
#include <vector>

class CuThreadFactoryImplI;
class CuThreadFactoryI;
class CuThreadInterface;
class CuData;
class CuThread;
class CuActivityI;
class CuServiceProvider;
class CuThreadsEventBridgeFactory_I;

class CuThreadService : public CuServiceI
{
public:
    CuThreadService();

    CuThreadInterface *getThread(const CuData& token,
                                 const CuThreadsEventBridgeFactory_I &eventsBridgeFactory,
                                 const CuServiceProvider *service_provider,
                                 const CuThreadFactoryImplI &thread_factory_impl);

    virtual ~CuThreadService();

    int count() const;

    void removeThread(CuThreadInterface *thread);

    std::vector<CuThreadInterface *> getThreads() const;

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    std::vector<CuThreadInterface *> mThreads;
};

#endif // THREADSERVICE_H
