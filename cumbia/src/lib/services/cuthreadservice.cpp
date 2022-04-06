#include "cuthreadservice.h"
#include "cuthreadfactoryimpl_i.h"
#include "cuthreadinterface.h"
#include "cuthreadseventbridgefactory_i.h"
#include "cudata.h"
#include "cuthread.h"
#include "cumacros.h"
#include <algorithm>
#include <assert.h>

class CuThreadServicePrivate {
public:
    std::vector<CuThreadInterface *> threads;
    pthread_t mythread;
};

/*! \brief class constructor
 *
 * Class constructor.
 *
 * Within the library, the service is created by the Cumbia constructor, where
 * it's also registered to the CuServiceProvider.
 */
CuThreadService::CuThreadService()
{
    d = new CuThreadServicePrivate();
    d->mythread = pthread_self();
}

CuThreadService::~CuThreadService() {
    assert(d->mythread == pthread_self());
    delete d;
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

int thcnt = 0;

CuThreadInterface *CuThreadService::getThread(const std::string& token,
                                              const CuThreadsEventBridgeFactory_I &eventsBridgeFactory,
                                              const CuServiceProvider *service_provider,
                                              const CuThreadFactoryImplI &thread_factory_impl,
                                              std::vector<CuThreadInterface *> *th_p) {
    assert(d->mythread == pthread_self());
    CuThreadInterface *thread;
    for(size_t i = 0; i < d->threads.size(); i++) {
        if(d->threads[i]->matches(token)) {
            pretty_pri("\e[1;32mREUSING THREAD for %s: \e[0mthreads size in service %ld", token.c_str(), d->threads.size());
            return d->threads[i];
        }
    }
    thread = thread_factory_impl.createThread(token, eventsBridgeFactory.createEventBridge(service_provider), service_provider, th_p);
    d->threads.push_back(thread);
    pretty_pri("\e[1;31mHAD TO CREATE THREAD FOR %s\e[0m: threads size in service %ld", token.c_str(), d->threads.size());
    return thread;
}

/*! \brief returns the number of threads recorded by the service
 *
 * @return the number of threads the service is currently aware of.
 */
int CuThreadService::count() {
    assert(d->mythread == pthread_self());
    return d->threads.size();
}

/*! \brief remove the specified thread from the service *without deleting it*
 *
 * @param thread the CuThread to remove from the service
 *
 */
void CuThreadService::removeThread(CuThreadInterface *thread) {
    assert(d->mythread == pthread_self());
    std::vector<CuThreadInterface *>::iterator it = std::find(d->threads.begin(), d->threads.end(), thread);
    if(it != d->threads.end())
        d->threads.erase(it);
}

/*! \brief returns the list of CuThreadInterface recorded in the service
 *
 * @return a std::vector of CuThreadInterface containing the pointers to the
 *         threads currently managed by the service
 *
 * Called by Cumbia::finish
 */
std::vector<CuThreadInterface *> CuThreadService::getThreads() const {
    assert(d->mythread == pthread_self());
    return d->threads;
}

/*! \brief returns the name of this service
 *
 * @return the const std::string "CuThreadService"
 *
 * @see getType
 */
std::string CuThreadService::getName() const
{
   assert(d->mythread == pthread_self());
    return std::string("CuThreadService");
}

/*! \brief returns the type of this service
 *
 * @return the value CuServices::Thread defined in CuServices
 */
CuServices::Type CuThreadService::getType() const {
   assert(d->mythread == pthread_self());
    return CuServices::Thread;
}
