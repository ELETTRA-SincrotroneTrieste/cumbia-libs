#ifndef CUTHREADFACTORY_I_H
#define CUTHREADFACTORY_I_H

class CuThreadInterface;
class CuData;
class CuServiceProvider;
class CuThreadFactoryImplI;
class CuThreadsEventBridge_I;

class CuThreadFactoryI
{
public:

    virtual CuThreadInterface *createThread(const CuData &data,
                                            CuThreadsEventBridge_I *eventsBridge,
                                            const CuServiceProvider *serviceProvider,
                                            const CuThreadFactoryImplI &threadFactoryImpl) const = 0;

};

#endif // CUTHREADFACTORYIMPL_H
