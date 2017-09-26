#include "cuthreadfactory.h"
#include "cuthreadfactoryimpl_i.h"
#include "cuthreadservice.h"
#include "cuthreadinterface.h"

CuThreadFactory::CuThreadFactory()
{

}

CuThreadInterface *CuThreadFactory::createThread(const CuData &data,
                                                 CuThreadsEventBridge_I *eventsBridge,
                                                 const CuServiceProvider *serviceProvider,
                                                 const CuThreadFactoryImplI &threadFactoryImpl) const
{
    return threadFactoryImpl.createThread(data, eventsBridge, serviceProvider);
}
