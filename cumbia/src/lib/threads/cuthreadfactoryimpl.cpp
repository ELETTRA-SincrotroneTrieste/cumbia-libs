#include "cuthreadfactoryimpl.h"
#include "cuthread.h"

CuThreadFactoryImpl::CuThreadFactoryImpl()
{

}

CuThreadInterface *CuThreadFactoryImpl::createThread(const CuData &data ,
                                                     CuThreadsEventBridge_I *eventsBridge,
                                                     const CuServiceProvider *service_provider) const
{
    return new CuThread(data, eventsBridge, service_provider);
}
