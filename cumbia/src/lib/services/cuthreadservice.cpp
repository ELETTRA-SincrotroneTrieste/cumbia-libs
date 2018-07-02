#include "cuthreadservice.h"
#include "cuthreadfactoryimpl_i.h"
#include "cuthreadinterface.h"
#include "cuthreadseventbridgefactory_i.h"
#include "cudata.h"
#include "cuthread.h"
#include "cumacros.h"
#include <algorithm>

/*! \brief class constructor
 *
 * Class constructor.
 *
 * Within the library, the service is created by the Cumbia constructor, where
 * it's also registered to the CuServiceProvider.
 */
CuThreadService::CuThreadService()
{
}

/**
 * @brief returns a new thread if the token passed as argument does not match any token
 *        from any other recorded thread in the service, an existing  thread if a
 *        match is found
 *
 * @param token a CuData used to determine whether a new activity is run in a new thread
 *        or an existing thread is reused. If a new thread is created, the given
 *        *token* will be the new thread's token
 * @param thread_factory an implementation of the CuThreadFactoryImplI interface that
 *        creates specific instances of CuThreadInterface, normally CuThread.
 * @return a new thread or an existing one, according to the contents in the *token*
 *         discussed above.
 *
 * \note if token is empty, a new thread is returned.
 *
 * \par Reusing threads
 * An existing thread is reused if the *token* parameter to getThread is *equivalent* to
 * the existing thread's token. Whether threads (or *tokens*) are equivalent or not is
 * decided by CuThread::isEquivalent.
 *
 * The *service provider* passed as parameter to getThread is directly handed to the
 * CuThreadFactoryImplI::createThread. If the CuThreadFactoryImplI implementation is
 * CuThreadFactoryImpl (that creates CuThread objects), the *service provider* is
 * forwarded to the CuThread's constructor.
 *
 * The CuThreadsEventBridgeFactory_I parameter is used to pass to CuThreadFactoryImplI::createThread
 * an instance of CuThreadsEventBridge_I, that is a CuThreadsEventBridge for a basic
 * cumbia *thread event bridge*, or a QThreadsEventBridge (cumbia-qtcontrols module) for
 * Qt applications.
 *
 */
CuThreadInterface *CuThreadService::getThread(const CuData& token,
                                              const CuThreadsEventBridgeFactory_I &eventsBridgeFactory,
                                              const CuServiceProvider *service_provider,
                                              const CuThreadFactoryImplI &thread_factory_impl)
{
    CuThreadInterface *thread;
    std::list<CuThreadInterface *>::const_iterator it;
    for(it = mThreads.begin(); it != mThreads.end(); ++it)
    {
        if((*it)->isEquivalent(token))
            return (*it);
    }
    thread = thread_factory_impl.createThread(token, eventsBridgeFactory.createEventBridge(), service_provider);
    std::lock_guard<std::mutex> lock(m_mutex);
    mThreads.push_back(thread);
    return thread;
}

/*! \brief the class destructor
 *
 * class destructor, empties the list of threads
 */
CuThreadService::~CuThreadService()
{
    pdelete("~CuThreadService %p", this);
    mThreads.clear();
}

/*! \brief returns the number of threads recorded by the service
 *
 * @return the number of threads the service is currently aware of.
 */
int CuThreadService::count() const
{
    return mThreads.size();
}

/*! \brief remove the specified thread from the service *without deleting it*
 *
 * @param thread the CuThread to remove from the service
 *
 * \note the thread is *not deleted*. Responsibility for threads created by
 * CuThreadService::getThread is handed to the client of the service.
 *
 */
void CuThreadService::removeThread(CuThreadInterface *thread)
{
    // this method is accessed from the run method of different threads
    std::lock_guard<std::mutex> lock(m_mutex);
    std::list<CuThreadInterface *>::iterator it = std::find(mThreads.begin(), mThreads.end(), thread);
    if(it != mThreads.end())
        mThreads.erase(it);
}

/*! \brief returns the list of CuThreadInterface recorded in the service
 *
 * @return a std::vector of CuThreadInterface containing the pointers to the
 *         threads currently managed by the service
 *
 * Called by Cumbia::finish
 */
std::list<CuThreadInterface *> CuThreadService::getThreads() const
{
    return mThreads;
}

/*! \brief returns the name of this service
 *
 * @return the const std::string "CuThreadService"
 *
 * @see getType
 */
std::string CuThreadService::getName() const
{
    return std::string("CuThreadService");
}

/*! \brief returns the type of this service
 *
 * @return the value CuServices::Thread defined in CuServices
 */
CuServices::Type CuThreadService::getType() const
{
    return CuServices::Thread;
}
