#include "cumbia.h"
#include "services/cuserviceprovider.h"
#include "services/cuthreadservice.h"
#include "services/cutimerservice.h"
#include "cuthreadseventbridgefactory_i.h"
#include "services/cuservices.h"
#include "services/cuactivitymanager.h"
#include "cucontinuousactivity.h"
#include "cuthreadinterface.h"
#include "cumacros.h"
#include "cuactivityevent.h"
#include "cuactivity.h"
#include "cuthreadtokengeni.h"

/*! @private */
class CumbiaPrivate
{
public:
    CuServiceProvider *serviceProvider;
    CuThreadTokenGenI *threadTokenGenerator;
};

/*! \brief Cumbia class constructor
 *
 * \li instantiates a *cumbia service provider*, namely CuServiceProvider
 * \li registers an instance of the *cumbia thread service*, CuThreadService, to
 *     the service provider
 * \li registers an instance of the *cumbia activity manager*, CuActivityManager, to
 *     the service provider
 *
 * The cumbia *service provider* is a class member. A pointer to it can be obtained
 * with Cumbia::getServiceProvider. The *service provider* remains the same
 * throughout the whole application lifetime and is destroyed when the Cumbia object
 * is destroyed (Cumbia::finish)
 *
 * Please refer to CuServiceProvider, CuThreadService and CuActivityManager documentation
 * for further information.
 */
Cumbia::Cumbia()
{
    d = new CumbiaPrivate();
    d->serviceProvider = new CuServiceProvider();
    d->serviceProvider->registerService(CuServices::Thread, new CuThreadService());
    d->serviceProvider->registerService(CuServices::ActivityManager, new CuActivityManager());
    d->serviceProvider->registerService(CuServices::Timer, new CuTimerService());
    d->threadTokenGenerator = nullptr;
    printf("cumbia version \e[1;32m1.4.0\e[0m [ \e[1;33mTEST\e[0m ]\n");
}

/*! \brief called from the class destructor, cleans up the Cumbia object
 *
 * The method does the following:
 * \li through the CuThreadService, the list of threads is obtained and for every
 *     thread CuThreadInterface::exit and CuThread::wait are called.
 *     <strong>The thread is finally deleted</strong>.
 * \li for each thread, hrough the CuActivityManager, a list of activities
 *     linked to the thread is obtained and, for each CuActivity,
 *     CuActivityManager::removeConnection is called. This unbinds *threads from
 *     activities* and *thread listeners* (CuThreadListener) from *activities*
 *     (CuActivity). See CuActivityManager::removeConnection.
 *     <strong>At last, the activity is deleted</strong>.
 * \li each service from the list of CuServiceI fetched from CuServiceProvider
 *     is unregistered from the service provider through CuServiceProvider::unregisterService
 * \li if the service had not been registered with the *shared* option
 *     (see CuServiceProvider::registerService), the service is deleted.
 * \li the <strong>service provider is deleted</strong>.
 * \li deletes the token generator, if set
 */
void Cumbia::finish()
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadService *ts = static_cast<CuThreadService *> (d->serviceProvider->get(CuServices::Thread));
    const std::list<CuThreadInterface *> threads = ts->getThreads();
    std::list<CuThreadInterface *>::const_iterator it;
    for(it = threads.begin(); it != threads.end(); ++it)
    {
        CuThreadInterface *ti = *it;
        /* when thread leaves its loop, invokes onExit on all activities, ensuring onExit is called in
         * the still running thread.
         */
        ti->exit();
        ti->wait();
        /* we're now back in the main thread, whence activities are deleted */
        ts->removeThread(ti);
        delete ti;
        const std::vector<CuActivity *> activities = activityManager->activitiesForThread(ti);
        for(size_t ai = 0; ai < activities.size(); ai++)
        {
            activityManager->removeConnection(activities[ai]);
            delete activities[ai];
        }
    }

    std::list<CuServiceI *> services = d->serviceProvider->getServices();
    for(std::list<CuServiceI *>::const_iterator it = services.begin(); it != services.end(); ++it)
    {
        CuServiceI *s = (*it);
        d->serviceProvider->unregisterService(s->getType());
        if(!d->serviceProvider->isShared(s->getType()))
            delete s;
    }
    if(d->threadTokenGenerator)
        delete d->threadTokenGenerator;
    delete d->serviceProvider;
    delete d;
    d = NULL; /* avoid destroying twice if cleanup is called from a subclass destructor */
}

/*! \brief returns the Cumbia::CumbiaBaseType type
 *
 * @return Cumbia::CumbiaBaseTyp
 */
int Cumbia::getType() const
{
    return CumbiaBaseType;
}
/*! \brief the class destructor
 *
 * Cleanup is delegated to the Cumbia::finish method
 */
Cumbia::~Cumbia()
{
    pdelete("~Cumbia %p\n", this);
    if(d)
        finish();
}

CuServiceProvider *Cumbia::getServiceProvider() const
{
    return d->serviceProvider;
}

/*!
 * \brief registers (adds) an activity to cumbia, links it to
 *        a data listener, assigns its execution to an existing thread or
 *        a new one according to a *thread token*
 *
 * \param activity the new activity to register
 * \param dataListener an implementation of CuThreadListener that will receive
 *        updates through the CuThreadListener::onProgress and
 *        CuThreadListener::onResult callbacks
 * \param thread_token a CuData used to decide whether the thread where the
 *        CuActivity is executed is a new one or it is picked from an existing
 *        one with the same token
 * \param thread_factory_impl a const reference to a CuThreadFactoryImplI implementation,
 *        such as CuThreadFactoryImpl, that creates CuThread instances.
 * \param eventsBridgeFactoryImpl a const reference to an implementation either of
 *        CuThreadsEventBridgeFactory_I, such as Qt's QThreadsEventBridgeFactory,
 *        that has to be used in Qt applications (couples CuThread with Qt
 *        event loop in QApplication), or CuThreadsEventBridgeFactory, that
 *        instantiates CuThreadsEventBridge bridges (for other cumbia, non Qt
 *        applications).
 *
 * This method exploits the CuThreadService to get a thread for the given thread_token.
 * If the thread_token matches the token of a *running thread*, that thread is used
 * for the activity. Otherwise, CuThreadService will return a *new thread* that
 * is instantiated through the CuThreadFactoryImplI factory. The new thread will
 * be bridged to the *event loop* (Qt's or cumbia CuEventLoopService) through the
 * *threads event bridge* instantiated by the CuThreadsEventBridgeFactory_I
 * passed to registerActivity. Please refer to CuThreadService::getThread documentation
 * for further reading.
 *
 * The CuActivityManager, another cumbia service (implements CuServiceI and is
 * registered to the CuServiceProvider), will bind together the *thread*, the
 * *activity* and the *CuThreadListener*, by means of CuActivityManager::addConnection.
 *
 * At last, the thread is started (if not reused and already running) and the new
 * activity is registered to the thread itself (through CuThread::registerActivity).
 * CuThread::registerActivity starts immediately the activity in the background
 * thread.
 *
 * see Cumbia::unregisterActivity
 * see CuActivity::Flags
 *
 */
void Cumbia::registerActivity(CuActivity *activity,
                              CuThreadListener *dataListener,
                              const CuData& thread_token,
                              const CuThreadFactoryImplI& thread_factory_impl,
                              const CuThreadsEventBridgeFactory_I &eventsBridgeFactoryImpl)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadService *thread_service = static_cast<CuThreadService *>(d->serviceProvider->get(CuServices::Thread));
    CuThreadInterface *thread = NULL;
    thread = thread_service->getThread(threadToken(thread_token), eventsBridgeFactoryImpl, d->serviceProvider, thread_factory_impl);
    activityManager->addConnection(thread, activity, dataListener);
    activity->setActivityManager(activityManager);
    if(!thread->isRunning())
        thread->start();
    thread->registerActivity(activity);
}

/*! \brief unregister an activity from cumbia
 *
 * @param activity the CuActivity to unregister
 *
 * Through the activity manager service, the thread associated to the activity is
 * fetched and CuThread::unregisterActivity is called.
 *
 * A CuActivity can automatically unregister after execution and be deleted
 * if appropriate flags are set (CuActivity::CuAUnregisterAfterExec and
 * CuActivity::CuADeleteOnExit)
 *
 * See CuThread::unregisterActivity for further reading.
 * See also the CuActivity and CuIsolatedActivity documentation.
 *
 */
void Cumbia::unregisterActivity(CuActivity *activity)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(activity));
    /* CuActivityManager.removeConnection is invoked by the thread in order to ensure all scheduled events are processed */
    if(thread) {
        thread->unregisterActivity(activity);
    }
}

/** \brief Finds an activity with the given token.
 *
 * @token CuData used to find an activity with the token
 *
 * @return a *non disposable* CuActivity whose token matches token
 *
 * \par Note
 * This is a convenience shortcut to CuActivityManager::findMatching
 */
CuActivity *Cumbia::findActivity(const CuData &token) const
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    return  activityManager->find(token);
}

/*! \brief if a timer runs within an activity, change the timeout
 *
 * @param a the CuActivity which timer has to be changed
 * @param timeout the number of milliseconds of the new timeout
 *
 * \par Implementation
 * Through the activity manager service, the thread associated to the activity is
 * fetched and a CuTimeoutChangeEvent is posted.
 */
void Cumbia::setActivityPeriod(CuActivity *a, int timeout)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    thread->postEvent(a, new CuTimeoutChangeEvent(timeout));
}

/*! \brief if a timer runs within an activity, get the timeout
 *
 * @param a the CuActivity which timer timeout has to be obtained
 * @return the number of milliseconds of the current timeout
 */
unsigned long Cumbia::getActivityPeriod(CuActivity *a) const
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    return thread->getActivityTimerPeriod(a);
}

/*! \brief if a timer runs within an activity, pause it
 *
 * @param a the CuActivity which timer timeout has to be paused
 */
void Cumbia::pauseActivity(CuActivity *a)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    thread->postEvent(a, new CuPauseEvent());
}

/*! \brief if a timer runs within an activity, resume it
 *
 * @param a the CuActivity which timer timeout has to be resumed
 */
void Cumbia::resumeActivity(CuActivity *a)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    thread->postEvent(a, new CuResumeEvent());
}

/*! \brief post an event to an activity
 *
 * @param a a pointer to a CuActivity where the event is dispatched
 * @param e a generic CuActivityEvent to be delivered to the activity
 *
 * \par implementation
 * Through the activity manager service, the thread associated to the activity is
 * fetched and CuThread::postEvent is called
 */
void Cumbia::postEvent(CuActivity *a, CuActivityEvent *e)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    thread->postEvent(a, e);
}

/*!
 * \brief Cumbia::setThreadTokenGenerator install athread token generator that deals
 *        with grouping activities into separate threads according to the specific engine implementation
 *
 * \param tg an implementation of the CuThreadTokenGenI interface
 *
 * \note
 * Once you install a thread token generator with this method, *the ownership of the object is handed
 * to Cumbia*. The generator will either be destroyed by Cumbia or by the removeThreadTokenGenerator
 * method. Subsequent calls to setThreadTokenGenerator will also delete a previously installed generator.
 */
void Cumbia::setThreadTokenGenerator(CuThreadTokenGenI *tg) {
    removeThreadTokenGenerator();
    d->threadTokenGenerator = tg;
}

/*!
 * \brief Cumbia::removeThreadTokenGenerator remove the currently installed generator, if any.
 *
 * \note The currently installed generator is deleted
 */
void Cumbia::removeThreadTokenGenerator()
{
    if(d->threadTokenGenerator)
        delete d->threadTokenGenerator;
    d->threadTokenGenerator = nullptr;
}

/*!
 * \brief Cumbia::getThreadTokenGenerator returns the installed token generator or nullptr if none is installed
 *
 * @see setThreadTokenGenerator
 *
 * \return the currently installed CuThreadTokenGenI implementation, null if none is installed.
 *
 * @see setThreadTokenGenerator
 */
CuThreadTokenGenI *Cumbia::getThreadTokenGenerator() const {
    return d->threadTokenGenerator;
}

/*!
 * \brief Cumbia::threadToken returns a thread token that can be used to group threads together
 *
 * If a thread token generator (CuThreadTokenGenI) implementation has been registered with setThreadTokenGenerator,
 * it is used to provide the thread token.
 *
 * If the option contains the *thread_token* with a given value, that value will be used to group
 * threads with the same token
 *
 * If there is no *thread_token* key in the input options, the return value is the same as the input
 * argument.
 *
 * \param options configuration options to customize thread grouping
 *
 * \return a thread token (CuData)
 */
CuData Cumbia::threadToken(const CuData &options) const
{
    CuData ret;
    if(d->threadTokenGenerator)
        ret = d->threadTokenGenerator->generate(options);
    else if(!options.containsKey("thread_token"))
        ret = options;
    else
        ret =  CuData("thread_token", options["thread_token"]);
    return ret;
}
