#ifndef CUTHREADFACTORYIMPL_INTERFACE_H
#define CUTHREADFACTORYIMPL_INTERFACE_H

class CuThreadInterface;
class CuData;
class CuServiceProvider;
class CuThreadsEventBridge_I;

class CuThreadFactoryImplI
{
public:

    virtual ~CuThreadFactoryImplI() {}

    virtual CuThreadInterface *createThread(const CuData& data,
                                            CuThreadsEventBridge_I *eventsBridge,
                                            const CuServiceProvider *serviceProvider) const = 0;
};

#endif // CUTHREADFACTORYIMPL_H
