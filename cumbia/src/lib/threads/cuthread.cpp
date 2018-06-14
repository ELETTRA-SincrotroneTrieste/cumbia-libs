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

/*! \brief builds a new CuThread
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
    predtmp("~CuThread %p", this);
    if(d->thread)
    {
        perr("CuThread: thread destroyed while still running!\e[0m\n");
        delete d->thread;
    }
    delete d->eventBridge;
    delete d;
}

/*! \brief exit the thread loop gracefully
 *
 * an ExitThreadEvent is queued to the event queue to exit the thread
 */
void CuThread::exit()
{
    m_exit(false);
}

void CuThread::m_exit(bool auto_destroy)
{
    if(d->thread)
    {
        ThreadEvent *exitEvent = new ExitThreadEvent(auto_destroy);
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

/*! \brief unregister the activity passed as argument from this thread.
 *
 * @param l the CuActivity to unregister from this thread.
 *
 * An UnRegisterActivityEvent is queued to the thread event queue.
 * When processed, CuActivity::doOnExit is called which in turn calls
 * CuActivity::onExit (in the CuActivity background thread).
 * If the flag CuActivity::CuADeleteOnExit is true, the activity is
 * later deleted (back in the main thread)
 *
 * \par Called from
 * This method is called from the CuThread's thread *only if CuActivity::CuAUnregisterAfterExec is
 * set on the activity*.
 * Otherwise, activities must be unregistered through Cumbia::unregisterActivity (in that case, invocation
 * occurs from the *main* thread).
 */
void CuThread::unregisterActivity(CuActivity *l)
{
    pbblue("CuThread.unregisterActivity: \e[1;31munregister activity %p\e[0m (main or activity's) 0x%lx", l, pthread_self());
    ThreadEvent *unregisterEvent = new UnRegisterActivityEvent(l); // type ThreadEvent::UnregisterActivity defined in cuthreadevents.h
    /* need to protect event queue because this method is called from the main thread while
     * the queue is dequeued in the secondary thread
     */
    std::unique_lock<std::mutex> lk(d->mutex);
    d->eventQueue.push(unregisterEvent);
    d->conditionvar.notify_one();
}

/** \brief implements onEventPosted from CuThreadsEventBridgeListener interface. Invokes onProgress or
 *         onResult on the registered CuThreadListener objects.
 *
 * This method gets a reference to a CuActivityManager through the *service provider*.
 * \li if either a CuEventI::Result or CuEventI::Progress is received, then the
 *     addressed CuActivity is extracted by the CuResultEvent and the list of
 *     CuThreadListener objects is obtained through CuActivityManager::getThreadListeners.
 *     At last either CuThreadListener::onProgress or CuThreadListener::onResult is called.
 * \li if CuEventI::CuActivityExitEvent event type is received, CuThread becomes aware
 *     that a CuActivity has finished, and deletes it if its CuActivity::CuADeleteOnExit
 *     flag is set to true.
 *
 * \par note
 * The association between *activities*, *threads* and *CuThreadListener* objects is
 * defined by Cumbia::registerActivity. Please read Cumbia::registerActivity documentation
 * for more details.
 *
 */
void CuThread::onEventPosted(CuEventI *event)
{
    //    pbblue("CuThread.onEventPosted: thread (should be main!) 0x%lx event type %d\n", pthread_self(), event->getType());
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
            if(re->getType() == CuEventI::Progress)
                tl->onProgress(re->getStep(), re->getTotal(), re->getData());
            else if(re->isList()) {
                const std::vector<CuData> &vd_ref = *re->getDataList();
//                printf("\e[1;33mCuThread %p onEventPosted (cudata list version): post to CuThreadListener %p\e[0m\n", this,  tl);
                tl->onResult(vd_ref);
            }
            else {
//                printf("\e[1;33mCuThread %p onEventPosted (single data version): post to CuThreadListener %p data %s\e[0m\n", this,  tl, re->getData().toString().c_str());
                tl->onResult(re->getData());
            }
        }
    }

    else if(ty == CuEventI::CuActivityExitEvent)
    {
        pbblue("CuThread.onEventPosted: thread (should be main!) 0x%lx event type %d calling mOnActivityExit\n", pthread_self(), event->getType());
        mOnActivityExited(static_cast<CuActivityExitEvent *>(event)->getActivity());
    }
    else if(ty == CuEventI::ThreadAutoDestroy) {
        cuprintf("\e[0;35mCuThread:onEventPosted: auto destroy! joining... ");
        wait();
        CuThreadService *ts = static_cast<CuThreadService *> (d->serviceProvider->get(CuServices::Thread));
        ts->removeThread(this);
        cuprintf("\t [\e[1;32mjoined\e[0m]\n");
        delete this;
    }
}

/*! \brief invoked in CuThread's thread, posts a *progress event* to the main thread
 *
 * @param activity: the addressee of the event
 * @param step the current step of the progress
 * @param total the total number of steps making up the whole work
 * @param data the data to be delivered with the progress event
 *
 * CuResultEvent is used in conjunction with CuThreadsEventBridge_I::postEvent
 */
void CuThread::publishProgress(const CuActivity* activity, int step, int total, const CuData &data)
{
    //    pbblue("CuThread.publishProgress: thread (should be CuThread's!) 0x%lx", pthread_self());
    d->eventBridge->postEvent(new CuResultEvent(activity, step, total, data));
}

/*! \brief invoked in CuThread's thread, posts a *result event* to main thread
 *
 * @param a: the addressee of the event
 * @param data the data to be delivered with the result
 *
 * CuResultEvent is used in conjunction with CuThreadsEventBridge_I::postEvent
 */
void CuThread::publishResult(const CuActivity* a,  const CuData &da)
{
    //    pbblue("CuThread.publishResult: thread (should be CuThread's!) 0x%lx data \e[0m%s", pthread_self(), da.toString().c_str());
    d->eventBridge->postEvent(new CuResultEvent(a, da));
}

void CuThread::publishResult(const CuActivity *a, const std::vector<CuData> *dalist)
{
    d->eventBridge->postEvent(new CuResultEvent(a, dalist));
}

/*! \brief  invoked in CuThread's thread, posts an *activity exit event*
 *          to the main thread
 *
 * \note used internally
 *
 * Called from CuActivity::doOnExit (background thread), delivers an *exit
 * event* to the main thread from the background, using
 * CuThreadsEventBridge_I::postEvent with a CuActivityExitEvent as parameter.
 * When the event is received and processed back in the *main thread* (in
 * CuThread::onEventPosted) the activity is deleted if the CuActivity::CuADeleteOnExit
 * flag is enabled.
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

/*! \brief internally used, allocates a new std::thread
 *
 * \note used internally
 *
 * return a new instance of std::thread
 */
void CuThread::start()
{
    d->thread = new std::thread(&CuThread::run, this);
}

/*! @private
 * Thread loop
 */
void CuThread::run()
{
    bool destroy = false;
    ThreadEvent *te = NULL;
    while(1)
    {
        te = NULL;
        {
            std::unique_lock<std::mutex> lk(d->mutex);
            std::queue<ThreadEvent *> qcopy = d->eventQueue;
            for(size_t i = 0; i < d->eventQueue.size(); i++)
            {
                qcopy.pop();
            }
            while(d->eventQueue.empty())
                d->conditionvar.wait(lk);

            if(d->eventQueue.empty())
            {
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
            // prevent event delivery to an already deleted action
            if(d->activity_set.find(a) != d->activity_set.end())
                a->event(ae);
            delete ae;
        }
        else if(te->getType() == ThreadEvent::ThreadExit) {
            // ExitThreadEvent enqueued by mOnActivityExited (foreground thread)
            destroy = static_cast<ExitThreadEvent *>(te)->autodestroy;
            delete te;
            break;
        }
        if(te)
            delete te;
    }
    /* on thread exit */
    /* empty and delete queued events */
    while(!d->eventQueue.empty()) {
        ThreadEvent *qte = d->eventQueue.front();
        d->eventQueue.pop();
        delete qte;
    }
    std::unique_lock<std::mutex> lk(d->mutex);
    CuActivityManager *am = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    std::vector<CuActivity *>  myActivities = am->activitiesForThread(this);
    pbred2("CuThread.run loop exit: auto destroy %d activities left %ld\n", destroy, myActivities.size());
    std::vector<CuActivity *>::iterator i;
    for(i = myActivities.begin(); i != myActivities.end(); ++i)
        mExitActivity(*i, true);
    printf("CuThread.run loop exit removing connections for this thread %p\n", this);
    am->removeConnections(this);
    CuThreadService *ts = static_cast<CuThreadService *> (d->serviceProvider->get(CuServices::Thread));
    ts->removeThread(this);
    // auto destroy when back in foreground thread. bridge: send event from bacgkround to fg
    if(destroy) {
        d->eventBridge->postEvent(new CuThreadAutoDestroyEvent());
    }
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
 * invoked from "main" thread, by onEventPosted.
 * If CuActivity::CuADeleteOnExit is set on a, a is deleted.
 * If this thread does not manage any activity, m_exit is called.
 */
void CuThread::mOnActivityExited(CuActivity *a)
{
    pr_thread();
    CuActivityManager *activityManager = static_cast<CuActivityManager *>(d->serviceProvider->get(CuServices::ActivityManager));
    activityManager->removeConnection(a);
    if(a->getFlags() & CuActivity::CuADeleteOnExit)
        delete a;
    if(activityManager->countActivitiesForThread(this) == 0) {
        m_exit(true);
    }
}

/*! @private
 * - called from within CuThread run (upon ThreadEvent::UnregisterActivity event)
 * - ThreadEvent::UnregisterActivity event is posted to CuThread from
 * - CuThread's thread
 *
 * When mExitActivity is called, the activity is asked to exit.
 * Whether it will be deleted or not depends on the CuActivity::CuADeleteOnExit flag on a (see
 * mOnActivityExited).
 *
 * mExitActivity is called *from the CuThread's thread*
 * The next function invoked in sequence in this class is mOnActivityExited, on the *main thread*.
 */
void CuThread::mExitActivity(CuActivity *a, bool onThreadQuit)
{
    std::set<CuActivity *>::iterator it = d->activity_set.find(a);
    if(it == d->activity_set.end())
        return;

    mRemoveActivityTimer(a);
    if(!(a->getStateFlags() & CuActivity::CuAStateOnExit)) {
        // if the activity is not already doing exit
        if(onThreadQuit)
            a->exitOnThreadQuit(); // will not call thread->publishExitEvent
        else
            a->doOnExit();

        d->activity_set.erase(it);
    }
    else
        perr("CuThread.mExitActivity: called twice on %p", a);

}

/*! @private */
void CuThread::mRemoveActivityTimer(CuActivity *a)
{
    std::map<CuActivity *, CuTimer *>::iterator it = d->timerActivityMap.begin();
    pbgreen("CuThread.mRemoveActivityTimer pthread 0x%lx Searching timer in map sized %lu for activity %p (type %d) \e[0m\e[1;36mAND UNREGISTERING/DELETING ACTIVITY\e[0m", pthread_self(),
            d->timerActivityMap.size(), a, a->getType());
    while(it != d->timerActivityMap.end()) {
        if(it->first == a)  {
            if(it->second != NULL) {
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

/*! \brief sends the event e to the activity a *from the main thread
 *         to the background*
 *
 * \note
 * This method is used to send an event *from the main thread to the
 * background* Events from the background are posted to the *main thread*
 * through the *thread event bridge* (CuThreadsEventBridge_I)
 *
 * @param a the CuActivity that sends the event
 * @param e the CuActivityEvent
 *
 * \par Examples
 * Cumbia calls CuThread::postEvent several times:
 *
 * \li Cumbia::setActivityPeriod sends a CuTimeoutChangeEvent event
 * \li Cumbia::pauseActivity sends a CuPauseEvent
 * \li Cumbia::resumeActivity sends a CuResumeEvent
 * \li Cumbia::postEvent barely forwards a *user defined* CuActivityEvent
 *     to the specified activity
 *
 * There can be situations where clients can call the above listed Cumbia methods.
 * On the other hand, CuThread::postEvent is not normally intended for direct use
 * by clients of this library.
 *
 */
void CuThread::postEvent(CuActivity *a, CuActivityEvent *e)
{
    //    pbblue("CuThread.postEvent: posting event to activity %p type %d. This thread 0x%lx\e[0m (should be Main's!)", a,
    //           e->getType(), pthread_self());

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
    //    pbblue("CuThread.onTimeout: thread (should be Timer's!) 0x%lx", pthread_self());
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
