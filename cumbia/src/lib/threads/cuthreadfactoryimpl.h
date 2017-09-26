#ifndef CUTHREADFACTORYIMPL_H
#define CUTHREADFACTORYIMPL_H

#include <cuthreadfactoryimpl_i.h>

class CuServiceProvider;
class CuThreadsEventBridge_I;

class CuThreadFactoryImpl : public CuThreadFactoryImplI
{
public:
    CuThreadFactoryImpl();

    // CuThreadFactoryImplI interface
public:
    CuThreadInterface *createThread(const CuData &data,
                                    CuThreadsEventBridge_I *eventsBridge,
                                    const CuServiceProvider *service_provider) const;
};

#endif // CUTHREADFACTORYIMPL_H
