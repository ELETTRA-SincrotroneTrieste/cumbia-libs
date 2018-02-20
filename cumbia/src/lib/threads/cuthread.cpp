#include "cuthread.h"
#include "cutimer.h"
#include "cudata.h"
#include "cuthreadevents.h"
#include "cuactivitymanager.h"
#include "cuserviceprovider.h"
#include "cuthreadservice.h"
#include "cuactivity.h"
#include "cumacros.h"
#include "cuevent.h"
#include "cuactivityevent.h"
#include "cuthreadlistener.h"
#include "cuisolatedactivity.h"

#include <set>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <assert.h>
#include <limits>

/*! @private */
class CuThreadPrivate
{
public:

    std::queue <ThreadEvent *> eventQueue;

    CuData token;

    const CuServiceProvider *serviceProvider;

    CuEventLoopService *cuEventLoop;

    std::mutex mutex;

    std::condition_variable conditionvar;

    std::thread *thread;

    std::map< CuActivity *, CuTimer *> timerActivityMap;

    std::set<CuActivity *> activity_set;

    CuThreadsEventBridge_I *eventBridge;

};

/*! \brief returns a new CuThread
 *
 * @param thread_token the token associated to the thread
 * @param eventsBridge a CuThreadsEventBridge_I implementation, for example
 *        CuThreadsEventBridge (see CuThreadsEventBridgeFactory::createEventBridge)
 *        or QThreadsEventBridge, recommended for *Qt applications*
 *        (see QThreadsEventBridgeFactory::createEventBridge)
 * @param service_provider the CuServiceProvider of the application
 *
 * \par Thread token
 * The thread token is used as an *id* for the thread. When Cumbia::registerActivity
 * is called, the thread token passed as argument is compared to all tokens
 * of all the running CuThreadInterface threads. If two tokens match, the
 * thread with that token is reused for the new activity, otherwise a new
 * thread is dedicated to run the new activity.
 */
CuThread::CuThread(const CuData &token,
                   CuThreadsEventBridge_I *teb,
                   const CuServiceProvider *serviceProvider)
{
    d = new CuThreadPrivate();
    d->cuEventLoop = NULL;
    d->token = token;
    d->eventBridge = teb;
    d->serviceProvider = serviceProvider;
    d->thread = NULL;
    assert(d->eventBridge != NULL && d->serviceProvider != NULL);
    d->eventBridge->setCuThreadsEventBridgeListener(this);
}

/*! \brief the class destructor, deletes the thread and the event bridge
 *
 * The CuThread destructor deletes the thread (std::thread) and the
 * event bridge
 */
CuThread::~CuThread()
{
    pdelete("~CuThread %p", this);
    if(d->thread)
    {
        perr("CuThread: thread destroyed while still running!\e[0m\n");
        delete d->thread;
    }
    delete d->eventBridge;
    delete d;
}

/*! \brief exit the thread loop
 *
 * an ExitThreadEvent is queued to the event queue to exit the thread
 */
void CuThread::exit()
{
    if (d->thread)
    {
        ThreadEvent *exitEvent = new ExitThreadEvent();
        std::unique_lock<std::mutex> lk(d->mutex);
        d->eventQueue.push(exitEvent);
        d->conditionvar.notify_one();
    }
}

/** \brief Register a new activity on this thread.
 *
 * This is invoked from the main thread.
 * As soon as registerActivity is called, a RegisterActivityEvent is added to the
 * thread event queue and the CuActivity will be soon initialised and executed in
 * the background. (CuActivity::init and CuActivity::execute)
 *
 * @param l a CuActivity that is the worker, whose methods are invoked in this background thread.
 */
void CuThread::registerActivity(CuActivity *l)
{
    ThreadEvent *registerEvent = new RegisterActivityEvent(l);
    /* need to protect event queue because this method is called from the main thread while
     * the queue is dequeued in the secondary thread
     */
    std::unique_lock<std::mutex> lk(d->mutex);
    d->eventQueue.push(registerEvent);
    d->conditionvar.notify_one();
}

/*! \brief unregister the activity passed as argument
 *
 * @param l the CuActivity to unregister
 *
 * An UnRegisterActivityEvent is queued to the thread event queue.
 * When processed, CuActivity::doOnExit is called which in turn calls
 * CuActivity::onExit (in the CuActivity background thread).
 * If the flag CuActivity::CuADeleteOnExit is true, the activity is
 * later deleted (back in the main thread)
 */
void CuThread::unregisterActivity(CuActivity *l)
{
    pbblue("CuThread.unregisterActivity: \e[1;31munregister activity %p\e[0m (main or activity's) 0x%lx", l, pthread_self());
    ThreadEvent *unregisterEvent = new UnRegisterActivityEvent(l);
    /* need to protect event queue because this method is called from the main thread while
     * the queue is dequeued in the secondary thread
     */
    std::unique_lock<std::mutex> lk(d->mutex);
    d->eventQueue.push(unregisterEvent);
    d->conditionvar.notify_one();
}

/** \brief implements onEventPosted from CuThreadsEventBridgeListener interface and invokes onProgress or
 *         onResult on the registered CuThreadListener objects.
 *
 */
void CuThread::onEventPosted(CuEventI *event)
{
    pbblue("CuThread.onEventPosted: thread (should be main!) 0x%lx event type %d\n", pthread_self(), event->getType());
    CuActivityManager *activity_manager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    const CuEventI::CuEventType ty = event->getType();
    if(ty == CuEventI::Result || ty == CuEventI::Progress)
    {
        CuResultEvent *re = static_cast<CuResultEvent *>(event);
        const CuActivity *a = re->getActivity();
        const std::vector<CuThreadListener *>& threadListeners = activity_manager->getThreadListeners(a);
        for(size_t i = 0; i < threadListeners.size(); i++)
        {
            CuThreadListener *tl = threadListeners.at(i);
            const CuData& data = re->getData();
            if(re->getType() == CuEventI::Progress)
                tl->onProgress(re->getStep(), re->getTotal(), data);
            else
                tl->onResult(data);
        }
    }
    else if(ty == CuEventI::CuActivityExitEvent)
    {
        pbblue("CuThread.onEventPosted: thread (should be main!) 0x%lx event type %d calling mOnActivityExit\n", pthread_self(), event->getType());
        mOnActivityExited(static_cast<CuActivityExitEvent *>(event)->getActivity());
    }
}

/* invoked in CuThread's thread, posts an event to main thread
 */
void CuThread::publishProgress(const CuActivity* activity, int step, int total, const CuData &data)
{
    pbblue("CuThread.publishProgress: thread (should be CuThread's!) 0x%lx", pthread_self());
    d->eventBridge->postEvent(new CuResultEvent(activity, step, total, data));
}

/* invoked in CuThread's thread, posts an event to main thread
 */
void CuThread::publishResult(const CuActivity* a,  const CuData &da)
{
    pbblue("CuThread.publishResult: thread (should be CuThread's!) 0x%lx data \e[0m%s", pthread_self(), da.toString().c_str());
    d->eventBridge->postEvent(new CuResultEvent(a, da));
}

/*! \brief  invoked in CuThread's thread, posts an event to main thread
 *
 * Delivers an *exit event* to the main thread from the background, using
 * CuThreadsEventBridge_I::postEvent with a CuActivityExitEvent
 */
void CuThread::publishExitEvent(CuActivity *a)
{
    pbblue("CuThread.publishExitEvent activity %p", a);
    d->eventBridge->postEvent(new CuActivityExitEvent(a));
}

/*! \brief returns true if this thread token is equal to other_thread_token
 *
 * @param other_thread_token a CuData that's the token of another thread
 *
 * @return true if this thread was created with a token that equals
 *         other_thread_token, false otherwise.
 *
 * \note
 * The CuData::operator== is used to perform the comparison between the tokens
 *
 * \par Usage
 * CuThreadService::getThread calls this method to decide wheter to reuse
 * the current thread (if the tokens are *equivalent*) or create a new one
 * to run a new activity (if this method returns false).
 *
 * When Cumbia::registerActivity is used to execute a new activity, the
 * *thread token* passed as input argument is relevant to decide whether the
 * new registered activity must be run in a running thread (if
 * one is found with the same token) or in a new one (no threads found with
 * the token given to CuActivity::registerActivity).
 *
 * See also getToken
 */
bool CuThread::isEquivalent(const CuData &other_thread_token) const
{
    return this->d->token == other_thread_token;
}

/*! \brief returns the thread token that was specified at construction time
 *
 * @return the CuData specified in the class constructor at creation time
 *
 * @see CuThread::CuThread
 */
CuData CuThread::getToken() const
{
    return d->token;
}

/*! @private
 * does nothing
 */
void CuThread::cleanup()
{

}

/*! \brief returns 0
 *
 * @return 0
 */
int CuThread::type() const
{
    return 0;
}

/*! @private */
void CuThread::start()
{
    d->thread = new std::thread(&CuThread::run, this);
}

/*! @private
 * Thread loop
 */
void CuThread::run()
{
    pbgreen("CuThread.run 0x%lx", pthread_self());
    ThreadEvent *te = NULL;
    while(1)
    {
        te = NULL;
        {
            std::unique_lock<std::mutex> lk(d->mutex);
            cuprintf("\e[1;33mCuThread.run: EVENT QUEUE SIZE %lu this thread  0x%lx this %p\e[0m\n", d->eventQueue.size(), pthread_self(), this);
            std::queue<ThreadEvent *> qcopy = d->eventQueue;
            for(size_t i = 0; i < d->eventQueue.size(); i++)
            {
                cuprintf("%ld - %d activity\n", i+1, qcopy.front()->getType());
                qcopy.pop();
            }
            while(d->eventQueue.empty())
                d->conditionvar.wait(lk);

            if(d->eventQueue.empty())
            {
                cuprintf("\e[1;33m**** QUEUE IS ZERO! Continueinggggg\e[0m\n");
                continue;
            }
            te = d->eventQueue.front();
            d->eventQueue.pop();
        }
        if(te->getType() == ThreadEvent::RegisterActivity)
        {
            pbgreen("CuThread.run pthread 0x%lx: registerActivity event", pthread_self());
            RegisterActivityEvent *rae = static_cast<RegisterActivityEvent *>(te);
            mActivityInit(rae->activity);
        }
        else if(te->getType() == ThreadEvent::UnregisterActivity)
        {
            pbgreen("CuThread.run pthread 0x%lx: unregister activity event", pthread_self());
            UnRegisterActivityEvent *rae = static_cast<UnRegisterActivityEvent *>(te);
            mExitActivity(rae->activity, false);
        }
        else if(te->getType() == ThreadEvent::TimerExpired)
        {
            CuTimerEvent *tev = static_cast<CuTimerEvent *>(te);
            CuTimer *timer = tev->getTimer();
            if(mFindActivity(timer) != NULL)
            {
                int repeat_ms = tev->activity->repeat();
                /* repeat_ms > 0: schedule timer; repeat_ms = 0 pause; repeat_ms < 0 exit */
                if(repeat_ms > 0)
                {
                    tev->activity->doExecute();
                    timer->start(repeat_ms);
                }
                else if(repeat_ms < 0 && tev->activity->getFlags() & CuActivity::CuAUnregisterAfterExec)
                    unregisterActivity(tev->activity);
            }
        }
        else if(te->getType() == ThreadEvent::PostEventToActivity)
        {
            CuPostEventToActivity *tce = static_cast<CuPostEventToActivity *>(te);
            CuActivity *a = tce->getActivity();
            CuActivityEvent* ae = tce->getEvent();
            CuTimer *t = mFindTimer(a);
            if(t && ae->getType() == CuActivityEvent::Pause)
                t->pause();
            else if(t && ae->getType() == CuActivityEvent::Resume)
                t->resume();
            else if(t && ae->getType() == CuActivityEvent::TimeoutChange)
                t->setTimeout(static_cast<CuTimeoutChangeEvent *>(ae)->getTimeout());
            a->event(ae);

            delete ae;
        }
        else if(te->getType() == ThreadEvent::ThreadExit)
        {
            pbgreen("CuThread.run pthread 0x%lx: \e[1;31mThreadExit\e[0m  event", pthread_self());
            delete te;
            break;
        }
        if(te)
            delete te;
    }
    /* on thread exit */
    /* empty and delete queued events */
    while(!d->eventQueue.empty())
    {
        ThreadEvent *qte = d->eventQueue.front();
        d->eventQueue.pop();
        delete qte;
    }
    std::unique_lock<std::mutex> lk(d->mutex);
    CuActivityManager *am = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    std::vector<CuActivity *>  myActivities = am->activitiesForThread(this);
    std::vector<CuActivity *>::iterator i;
    for(i = myActivities.begin(); i != myActivities.end(); ++i)
        mExitActivity(*i, true);
    pbred2("CuThread.run loop exit\n");
}

/*! \brief returns true if the thread is running
 *
 * @return true if the thread is running
 */
bool CuThread::isRunning()
{
    return d->thread != NULL;
}

/*! @private
 * called from CuThread::run()
*/
void CuThread::mActivityInit(CuActivity *a)
{
    int repeat_timeout;
    d->activity_set.insert(a);

    a->doInit();
    a->doExecute();

    repeat_timeout = a->repeat();
    CuTimer *timer = NULL;
    if(repeat_timeout > 0)
    {
        timer = new CuTimer(this);
        timer->start(repeat_timeout);
        std::pair<CuActivity *, CuTimer *> p(a, timer);
        d->timerActivityMap.insert(p);
    }
    else if(a->getFlags() & CuActivity::CuAUnregisterAfterExec)
        unregisterActivity(a); /* will enqueue and Unregister event */
}

/*! @private
 * invoked from "main" thread, by onEventPosted
 */
void CuThread::mOnActivityExited(CuActivity *a)
{
    pr_thread();
    static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager))->removeConnection(a);
    if(a->getFlags() & CuActivity::CuADeleteOnExit)
        delete a;
}

/*! @private
 * called from within CuThread run
 */
void CuThread::mExitActivity(CuActivity *a, bool onThreadQuit)
{
    std::set<CuActivity *>::iterator it = d->activity_set.find(a);
    if(it == d->activity_set.end())
        return;

    mRemoveActivityTimer(a);
    if(a->getStateFlags() & CuActivity::CuAStateOnExit)
    {
        printf("\e[1;35;4mwill not call exit again on activity %p\e[0m\n", a);
        return;
    }

    // if the activity is not already doing exit
    if(onThreadQuit)
        a->exitOnThreadQuit(); // will not call thread->publishExitEvent
    else
        a->doOnExit();

    d->activity_set.erase(it);
}

/*! @private */
void CuThread::mRemoveActivityTimer(CuActivity *a)
{
    std::map<CuActivity *, CuTimer *>::iterator it = d->timerActivityMap.begin();
    pbgreen("CuThread.mRemoveActivityTimer pthread 0x%lx Searching timer in map sized %lu for activity %p (type %d) \e[0m\e[1;36mAND UNREGISTERING/DELETING ACTIVITY\e[0m", pthread_self(),
            d->timerActivityMap.size(), a, a->getType());
    while(it != d->timerActivityMap.end())
    {
        if(it->first == a)
        {
            if(it->second != NULL)
            {
                it->second->stop();
                delete it->second;
            }
            it = d->timerActivityMap.erase(it);
        }
        else
            ++it;
    }
}

/*! @private */
CuTimer *CuThread::mFindTimer(CuActivity *a) const
{
    std::map<CuActivity *, CuTimer *>::iterator it;
    for(it = d->timerActivityMap.begin(); it != d->timerActivityMap.end(); ++it)
        if(it->first == a)
            return it->second;
    return NULL;
}

/*! @private */
CuActivity *CuThread::mFindActivity(CuTimer *t) const
{
    std::map<CuActivity *, CuTimer *>::iterator it;
    for(it = d->timerActivityMap.begin(); it != d->timerActivityMap.end(); ++it)
        if(it->second == t)
            return it->first;
    return NULL;
}

/*! \brief sends the event e to the activity a
 *
 * @param a the CuActivity that sends the event
 * @param e the CuActivityEvent
 *
 */
void CuThread::postEvent(CuActivity *a, CuActivityEvent *e)
{
    pbblue("CuThread.postEvent: posting event to activity %p type %d. This thread 0x%lx\e[0m (should be Main's!)", a,
           e->getType(), pthread_self());

    ThreadEvent *event = new CuPostEventToActivity(a, e);
    /* need to protect event queue because this method is called from the main thread while
         * the queue is dequeued in the secondary thread
         */
    std::unique_lock<std::mutex> lk(d->mutex);
    d->eventQueue.push(event);
    d->conditionvar.notify_one();
}

/*! \brief returns the period of the timer running for the given activity
 *
 * @param a the CuActivity which timer period you want to get
 * @return milliseconds of the timer running for that activity or -1 if
 *         no timer is associated to the given activity
 */
int CuThread::getActivityTimerPeriod(CuActivity *a) const
{
    pbblue("CuThread.getActivityPeriod: getting period of activity %p. This thread 0x%lx\e[0m (should be Main's!)", a, pthread_self());
    /* need to protect event queue because this method is called from the main thread while
     * the queue is dequeued in the secondary thread
     */
    std::unique_lock<std::mutex> lk(d->mutex);
    std::map<CuActivity *, CuTimer *>::const_iterator it;
    for(it = d->timerActivityMap.begin(); it != d->timerActivityMap.end(); ++it)
        if(it->first == a)
            return it->second->timeout();
    return -1;
}

/*! @private */
void CuThread::onTimeout(CuTimer *sender)
{
    pbblue("CuThread.onTimeout: thread (should be Timer's!) 0x%lx", pthread_self());
    std::map<CuActivity *, CuTimer *>::iterator it;
    for(it = d->timerActivityMap.begin(); it != d->timerActivityMap.end(); ++it)
    {
        if(it->second == sender)
        {
            CuTimerEvent *te = new CuTimerEvent(it->first, sender);
            std::unique_lock<std::mutex> lk(d->mutex);
            d->eventQueue.push(te);
            d->conditionvar.notify_one();
        }
    }
}

/*! @private */
void CuThread::wait()
{
    pbviolet("CuThread::wait: d->thread %p. Joining!", d->thread);
    if(d->thread)
    {
        d->thread->join();
        pbviolet("CuThread::wait: THREAD  %p JOINED", d->thread);
        delete d->thread;
        d->thread = NULL;
    }
}
