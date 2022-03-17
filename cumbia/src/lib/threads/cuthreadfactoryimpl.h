#ifndef CUTHREADFACTORYIMPL_H
#define CUTHREADFACTORYIMPL_H

#include <cuthreadfactoryimpl_i.h>

class CuServiceProvider;
class CuThreadsEventBridge_I;

/*! \brief implementation of CuThreadFactoryImplI used to instantiate
 *         and return a new CuThread
 *
 * Factory producing CuThread threads (see createThread)
 *
 * @implements CuThreadFactoryImplI
 */
class CuThreadFactoryImpl : public CuThreadFactoryImplI
{
    // CuThreadFactoryImplI interface
public:
    CuThreadInterface *createThread(const std::string &thread_token,
                                    CuThreadsEventBridge_I *eventsBridge,
                                    const CuServiceProvider *service_provider,
                                    std::vector<CuThreadInterface *> *threads) const;
};

#endif // CUTHREADFACTORYIMPL_H
