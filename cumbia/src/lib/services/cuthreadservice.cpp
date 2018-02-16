#include "cuthreadservice.h"
#include "cuthreadfactoryimpl_i.h"
#include "cuthreadinterface.h"
#include "cuthreadseventbridgefactory_i.h"
#include "cudata.h"
#include "cuthread.h"
#include "cumacros.h"
#include <algorithm>

CuThreadService::CuThreadService()
{
}

/**
 * @brief CuThreadService::getThread
 * @param token
 * @param thread_factory
 * @return
 *
 * \note if token is empty, a new thread is returned.
 *
 */
CuThreadInterface *CuThreadService::getThread(const CuData& token,
                                              const CuThreadsEventBridgeFactory_I &eventsBridgeFactory,
                                              const CuServiceProvider *service_provider,
                                              const CuThreadFactoryImplI &thread_factory_impl)
{
    CuThreadInterface *thread;
    for(size_t i = 0; i < mThreads.size(); i++)
    {
        thread = mThreads[i];
        if(thread->isEquivalent(token))
            return thread;
    }
    thread = thread_factory_impl.createThread(token, eventsBridgeFactory.createEventBridge(), service_provider);
    mThreads.push_back(thread);
    return thread;
}

CuThreadService::~CuThreadService()
{
    pdelete("~CuThreadService %p", this);
    mThreads.clear();
}

int CuThreadService::count() const
{
    return mThreads.size();
}

void CuThreadService::removeThread(CuThreadInterface *thread)
{
    std::vector<CuThreadInterface *>::iterator it = std::find(mThreads.begin(), mThreads.end(), thread);
    if(it != mThreads.end())
        mThreads.erase(it);
}

std::vector<CuThreadInterface *> CuThreadService::getThreads() const
{
    return mThreads;
}

std::string CuThreadService::getName() const
{
    return std::string("CuThreadService");
}

CuServices::Type CuThreadService::getType() const
{
    return CuServices::Thread;
}
