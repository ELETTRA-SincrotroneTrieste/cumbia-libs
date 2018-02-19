#include "cuthreadfactoryimpl.h"
#include "cuthread.h"

/*! \brief returns a new CuThread
 *
 * @param thread_token the token associated to the thread
 * @param eventsBridge a CuThreadsEventBridge_I implementation
 * @param service_provider, the CuServiceProvider of the application
 * @return a new CuThread with the given token using the given event bridge
 *
 * For a detailed description of the parameters, please refer to the
 * documentation of the CuThread constructor, CuThread::CuThread.
 *
 *
 */
CuThreadInterface *CuThreadFactoryImpl::createThread(const CuData &thread_token ,
                                                     CuThreadsEventBridge_I *eventsBridge,
                                                     const CuServiceProvider *service_provider) const
{
    return new CuThread(thread_token, eventsBridge, service_provider);
}
