#include "cumbia.h"
#include "services/cuserviceprovider.h"
#include "services/cuthreadservice.h"
#include "cuthreadseventbridgefactory_i.h"
#include "services/cuservices.h"
#include "services/cuactivitymanager.h"
#include "cucontinuousactivity.h"
#include "cuthreadinterface.h"
#include "cuthreadinterface.h"
#include "cumacros.h"
#include "cuactivityevent.h"
#include "cuactivity.h"

class CumbiaPrivate
{
public:
    CuServiceProvider *serviceProvider;
};

void Cumbia::finish()
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadService *ts = static_cast<CuThreadService *> (d->serviceProvider->get(CuServices::Thread));
    const std::vector<CuThreadInterface *> threads = ts->getThreads();
    for(size_t i = 0; i < threads.size(); i++)
    {
        CuThreadInterface *ti = threads[i];
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
        delete s;
    }
    delete d->serviceProvider;
    delete d;
    d = NULL; /* avoid destroying twice if cleanup is called from a subclass destructor */
}

int Cumbia::getType() const
{
    return CumbiaBaseType;
}

Cumbia::~Cumbia()
{
    pdelete("~Cumbia %p", this);
    if(d)
        finish();
}

Cumbia::Cumbia()
{
    d = new CumbiaPrivate();
    d->serviceProvider = new CuServiceProvider();
    d->serviceProvider->registerService(CuServices::Thread, new CuThreadService());
    d->serviceProvider->registerService(CuServices::ActivityManager, new CuActivityManager());
}

CuServiceProvider *Cumbia::getServiceProvider() const
{
    return d->serviceProvider;
}

void Cumbia::registerActivity(CuActivity *activity,
                              CuThreadListener *dataListener,
                              const CuData& thread_token,
                              const CuThreadFactoryImplI& thread_factory_impl,
                              const CuThreadsEventBridgeFactory_I &eventsBridgeFactoryImpl)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadService *thread_service = static_cast<CuThreadService *>(d->serviceProvider->get(CuServices::Thread));
    CuThreadInterface *thread = NULL;
    thread = thread_service->getThread(thread_token, eventsBridgeFactoryImpl, d->serviceProvider, thread_factory_impl);
    activityManager->addConnection(thread, activity, dataListener);
    activity->setActivityManager(activityManager);
    cuprintf("Cumbia::registerActivity: \e[1;32mgot thread \"0x%lx\", is running? [%d]\e[0m...\n", pthread_self(), thread->isRunning());
    if(!thread->isRunning())
        thread->start();
    thread->registerActivity(activity);
}

void Cumbia::unregisterActivity(CuActivity *activity)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(activity));
    /* CuActivityManager.removeConnection is invoked by the thread in order to ensure all scheduled events are processed */
    if(thread)
        thread->unregisterActivity(activity);
}

/** \brief Finds an activity with the given token.
 *
 * @token CuData used to find an activity with the token
 *
 * @return a CuActivity whose token matches token
 *
 * \par Note
 * This is a convenience shortcut to CuActivityManager::findMatching
 */
CuActivity *Cumbia::findActivity(const CuData &token) const
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    return  activityManager->findMatching(token);
}

void Cumbia::setActivityPeriod(CuActivity *a, int timeout)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    thread->postEvent(a, new CuTimeoutChangeEvent(timeout));
}

unsigned long Cumbia::getActivityPeriod(CuActivity *a) const
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    return thread->getActivityTimerPeriod(a);
}

void Cumbia::pauseActivity(CuActivity *a)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    thread->postEvent(a, new CuPauseEvent());
}

void Cumbia::resumeActivity(CuActivity *a)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    thread->postEvent(a, new CuResumeEvent());
}

void Cumbia::postEvent(CuActivity *a, CuActivityEvent *e)
{
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    CuThreadInterface *thread = static_cast<CuThreadInterface *>(activityManager->getThread(a));
    thread->postEvent(a, e);
}
