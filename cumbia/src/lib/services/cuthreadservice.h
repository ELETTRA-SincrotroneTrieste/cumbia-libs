#ifndef CUTHREADSERVICE_H
#define CUTHREADSERVICE_H

#include <cuservices.h>
#include <cuservicei.h>
#include <vector>

class CuThreadFactoryImplI;
class CuThreadInterface;
class CuData;
class CuThread;
class CuActivityI;
class CuServiceProvider;
class CuThreadsEventBridgeFactory_I;

/*! \brief *cumbia service* that creates threads and keeps references of the threads
 *         running within the application
 *
 * \par cuthread_service The *cumbia thread service*
 *
 * The most important function in this service is CuThreadService::getThread.
 * It returns a pointer to a CuThreadInterface implementation as instantiated
 * by the CuThreadFactoryImplI class factory specified in the last input argument.
 * This happens when the *token* passed to getThread does not match any token
 * of any other running thread. In this case, a thread with a matching token is used (or
 * an *equivalent* thread, since the CuThread::isEquivalent method is used for comparison).
 * It is *client responsibility to delete the thread returned by getThread*:
 * CuThreadService::removeThread erases the reference to the object from the internal
 * list without deleting it.
 *
 * The count method returns the number of threads currently managed by the service
 *
 * The removeThread function removes the thread from the list *without deleting it*
 * CuThreadService::removeThread is currently called from Cumbia::finish (Cumbia's
 * class destructor).
 *
 * The getThreads method returns the list of all the threads currently managed by
 * the service. It is called by Cumbia::finish to ask every thread to exit
 * (calls CuThread::exit) and then let the background thread *join* the main one
 * (calling CuThread::wait)
 *
 * @see Cumbia::registerActivity
 * @see Cumbia::finish
 *
 */
class CuThreadService : public CuServiceI
{
public:
    CuThreadService();

    CuThreadInterface *getThread(const CuData& token,
                                 const CuThreadsEventBridgeFactory_I &eventsBridgeFactory,
                                 const CuServiceProvider *service_provider,
                                 const CuThreadFactoryImplI &thread_factory_impl);

    virtual ~CuThreadService();

    int count() const;

    void removeThread(CuThreadInterface *thread);

    std::vector<CuThreadInterface *> getThreads() const;

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    std::vector<CuThreadInterface *> mThreads;
};

#endif // THREADSERVICE_H
