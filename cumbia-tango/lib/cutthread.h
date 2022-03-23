#ifndef CUTTHREAD_H
#define CUTTHREAD_H

#include <cuthreadfactoryimpl_i.h>
#include <cuthread.h>

class CuServiceProvider;
class CuThreadsEventBridge_I;
class CuTThreadPrivate;
class CuDeviceFactory_I;

/*! \brief implementation of CuThreadFactoryImplI used to instantiate
 *         and return a new CuThread
 *
 * Factory producing CuThread threads (see createThread)
 *
 * @implements CuThreadFactoryImplI
 */
class CuTThreadFactoryImpl : public CuThreadFactoryImplI
{
    // CuThreadFactoryImplI interface
public:
    CuThreadInterface *createThread(const std::string &thread_token,
                                    CuThreadsEventBridge_I *eventsBridge,
                                    const CuServiceProvider *service_provider,
                                    std::vector<CuThreadInterface *> *threads) const;
};

class CuTThread : public CuThread {
public:
    enum Type { CuTThreadType =  12 };

    CuTThread(const std::string &token,
              CuThreadsEventBridge_I *threadEventsBridge,
              const CuServiceProvider *sp,
              std::vector<CuThreadInterface *> *thv_p);

    ~CuTThread();

    int type() const;

    CuDeviceFactory_I *device_factory() const;

private:
    CuTThreadPrivate *d;
};

#endif // CUTTHREAD_H
