#ifndef CUTHREADFACTORY_H
#define CUTHREADFACTORY_H

#include <cuthreadfactory_i.h>

class CuData;
class CuServiceProvider;
class CuThreadFactoryImplI;

class CuThreadFactory : public CuThreadFactoryI
{
public:
    CuThreadFactory();

    // CuThreadFactoryI interface
public:
    CuThreadInterface *createThread(const CuData &data,
                                    CuThreadsEventBridge_I *eventsBridge,
                                    const CuServiceProvider *serviceProvider,
                                    const CuThreadFactoryImplI &threadFactoryImpl) const;
};

#endif // CUTHREADFACTORY_H
