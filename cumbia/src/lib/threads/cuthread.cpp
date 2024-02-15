#include "cuthread.h"
#include "cutimer.h"
#include "cudata.h"
#include "cuthreadevents.h"
#include "cuactivity.h"
#include "cumacros.h"
#include "cuevent.h"
#include "cuactivityevent.h"
#include "cuthreadlistener.h"
#include "cutimerservice.h"
#include "cuserviceprovider.h"

#include <set>
#include <thread>
#include <queue>
#include <shared_mutex>
#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <vector>
#include <map>
#include <sys/time.h>
#include <chrono>
#include <assert.h>
#include <limits>

/*! @private
 *
 *  Private thread local methods and data.
 *  All these methods belong in CuThread's thread
 */
class CuThPP {
public:

    CuThPP(const CuServiceProvider *sp, CuThreadsEventBridge_I *_eb)
        : tmr_s(static_cast<CuTimerService *>(sp->get(CuServices::Timer))),
        eb(_eb) {
        mythread = pthread_self();
    }

    // timeout change:
    // 1. unregister and delete old_t
    // 2. create a new timer and start it with the required timeout
    CuTimer *m_a_new_timeout(CuActivity *a, int timeo, CuTimer* old_t, CuThread *th) {
        assert(mythread == pthread_self());
        m_tmr_remove(a); // remove the old activity - old timer entry
        if(m_activity_cnt(old_t) == 0) {
            tmr_s->unregisterListener(th, old_t->timeout());
        }
        CuTimer * t = tmr_s->registerListener(th, timeo); // may reuse timers
        m_tmr_registered(a, t);
        return t;
    };

    void activity_run(CuActivity *a, CuThread* t) {
        activity_set.insert(a);
        a->doInit();
        a->doExecute();
        if(a->repeat() > 0)  {
            CuTimer *ti = tmr_s->registerListener(t, a->repeat()); // checks for duplicates
            m_tmr_registered(a, ti);
        }
    };

    /*! @private */
    void mExitActivity(CuActivity *a, CuThread* t, bool on_quit) {
        std::set<CuActivity *>::iterator it = activity_set.find(a);
        if(it != activity_set.end()) {
            mRemoveActivityTimer(a, t);
            a->doOnExit();
            if(!on_quit)
                m_post_exit_event(a);
            activity_set.erase(it);
        }
    };

    /*! @private */
    void mRemoveActivityTimer(CuActivity *a, CuThread* th) {
        int timeo = -1; // timeout
        std::unordered_map<CuActivity *, CuTimer *>::iterator it = tmr_amap.find(a);
        bool u = it != tmr_amap.end(); // initialize u
        if(u) { // test u: end() iterator (valid, but not dereferenceable) cannot be used as key search.
            // get timeout from timer not from a, because a->repeat shall return -1 if exiting
            timeo = it->second->timeout();
            tmr_amap.erase(tmr_amap.find(a)); // removes if exists
        }
        for(std::unordered_map<CuActivity *, CuTimer *>::iterator it = tmr_amap.begin(); it != tmr_amap.end() && u; ++it)
            u &= it->second->timeout() != timeo; // no timers left with timeo timeout?
        if(u) {
            tmr_s->unregisterListener(th, timeo);
        }
    }

    /*! @private */
    const CuTimer *m_tmr_find(CuActivity *a) const {
        std::unordered_map<CuActivity *, CuTimer *>::const_iterator it = tmr_amap.find(a);
        if(it != tmr_amap.end())
            return it->second;
        return nullptr;
    }

    // inserts the pair (a,t) into d->timerActivityMap
    void m_tmr_registered(CuActivity *a, CuTimer *t) {
        assert(mythread == pthread_self());
        tmr_amap.insert(std::pair<CuActivity *, CuTimer *>(a, t));
    }

    void m_tmr_remove(CuTimer *t) {
        assert(mythread == pthread_self());
        std::unordered_map<CuActivity *, CuTimer *>::iterator it = tmr_amap.begin();
        while(it != tmr_amap.end()) {
            if(it->second == t) it = tmr_amap.erase(it);
            else   ++it;
        }
    }

    size_t m_tmr_remove(CuActivity *a) {
        size_t e = 0;
        if(tmr_amap.find(a) != tmr_amap.end())
            e = tmr_amap.erase(a);
        return e;
    }

    size_t m_activity_cnt(CuTimer *t) const  {
        size_t s = 0;
        for(std::unordered_map<CuActivity *, CuTimer *>::const_iterator it = tmr_amap.begin(); it != tmr_amap.end(); ++it)
            if(it->second == t)
                s++;
        return s;
    }

    std::list<CuActivity *> m_activitiesForTimer(const CuTimer *t) const {
        std::list<CuActivity*> activities;
        std::unordered_map<CuActivity *, CuTimer *>::const_iterator it;
        for(it = tmr_amap.begin(); it != tmr_amap.end(); ++it) {
            if(it->second == t)
                activities.push_back(it->first);
        }
        return activities;
    }

    void m_post_exit_event(CuActivity *a)  {
        assert(mythread == pthread_self());
        eb->postEvent(new CuA_ExitEv(a));
    }

    std::unordered_map< CuActivity *, CuTimer *> tmr_amap;
    std::set<CuActivity *> activity_set;

    CuTimerService *tmr_s;
    CuThreadsEventBridge_I *eb;
    pthread_t mythread;
    CuData atok; // copy of activity token for thread local use

private:
};

/*! @private */
class CuThreadPrivate
{
public:
    CuThreadPrivate(const std::string &tk,
                    CuThreadsEventBridge_I *teb,
                    const CuServiceProvider *sp,
                    std::vector<CuThreadInterface *>* thv_p) :
        alimmap_locked(false), token(tk), eb(teb), se_p(sp), threads_p(thv_p),
        thpp(nullptr), thread(nullptr), exiting(false) {

    }

    std::queue <ThreadEvent *> eq;
    // activity --> listeners multi map
    std::multimap<const CuActivity *, CuThreadListener *> alimmap;
    bool alimmap_locked; // lock alimmap (same thread)
    // activities to remove while alimmap_locked shall be placed here
    std::list<CuActivity *> arem_list;
    std::string token;
    std::mutex mu;
    std::condition_variable cv;
    CuThreadsEventBridge_I *eb;
    const CuServiceProvider *se_p;
    // cumbia private list of threads
    // used to track the full thread lifetime
    std::vector<CuThreadInterface *>* threads_p;
    CuThPP *thpp;
    std::thread *thread;
    pthread_t mythread;
    bool exiting;
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
CuThread::CuThread(const std::string &token,
                   CuThreadsEventBridge_I *teb,
                   const CuServiceProvider *sp,
                   std::vector<CuThreadInterface *>* thv_p)
{
    d = new CuThreadPrivate(token, teb, sp, thv_p);
    d->eb->setCuThreadsEventBridgeListener(this);
    d->mythread = pthread_self();
}

/*! \brief the class destructor, deletes the thread and the event bridge
 *
 * The CuThread destructor deletes the thread (std::thread) and the
 * event bridge
 */
CuThread::~CuThread() {
    assert(d->mythread == pthread_self());
    if(d->thread)
        perr("CuThread::~CuThread(): thread destroyed while still running!\e[0m\n");
    if(d->thread) delete d->thread;
    if(d->thpp) delete d->thpp;
    delete d->eb;
    delete d;
}

/*! \brief exit the thread loop gracefully
 *
 * an ExitThreadEvent is queued to the event queue to exit the thread
 */
void CuThread::exit() {
    assert(d->mythread == pthread_self());
    if(d->thpp) {
        ThreadEvent *exitEvent = new CuThreadExitEv; // no auto destroy
        std::unique_lock lk(d->mu);
        d->eq.push(exitEvent);
        d->cv.notify_one();
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
void CuThread::registerActivity(CuActivity *l, CuThreadListener *tl) {
    assert(d->mythread == pthread_self());
    l->setThreadToken(d->token);
    // add [another] listener to l
    d->alimmap.insert(std::pair<const CuActivity *, CuThreadListener *>(l, tl));
    ThreadEvent *registerEvent = new CuThRegisterA_Ev(l);
    /* need to protect event queue because this method is called from the main thread while
     * the queue is dequeued in the secondary thread
     */
    std::unique_lock lk(d->mu);
    d->eq.push(registerEvent);
    d->cv.notify_one();
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
 * Thread: main
 *
 */
void CuThread::unregisterActivity(CuActivity *l) {
    assert(d->mythread == pthread_self());
    if(!d->alimmap_locked)
        d->alimmap.erase(l);
    else {
        d->arem_list.push_back(l); // onEventPosted later removes from d->alimmap
//        printf("CuThread::unregisterActivity: \e[1;35mactivity / thread listeners map is currently locked\e[0m\n");
    }
    ThreadEvent *unregisterEvent = new CuThUnregisterA_Ev(l); // ThreadEvent::UnregisterActivity
    std::unique_lock lk(d->mu);
    d->eq.push(unregisterEvent);
    d->cv.notify_one();
}

/** \brief implements onEventPosted from CuThreadsEventBridgeListener interface. Invokes onProgress or
 *         onResult on the registered CuThreadListener objects.
 *
 * This method gets a reference to a CuActivityManager through the *service provider*.
 * \li if either a CuEventI::Result or CuEventI::Progress is received, then the
 *     addressed CuActivity is extracted by the CuResultEvent and the list of
 *     CuThreadListener objects is obtained through CuActivityManager::getThreadListeners.
 *     At last either CuThreadListener::onProgress or CuThreadListener::onResult is called.
 * \li if CuEventI::CuA_ExitEv event type is received, CuThread becomes aware
 *     that a CuActivity has finished, and deletes it if its CuActivity::CuADeleteOnExit
 *     flag is set to true.
 *
 * \par note
 * The association between *activities*, *threads* and *CuThreadListener* objects is
 * defined by Cumbia::registerActivity. Please read Cumbia::registerActivity documentation
 * for more details.
 *
 */
void CuThread::onEventPosted(CuEventI *event) {
    assert(d->mythread == pthread_self());
    const CuEventI::CuEventType ty = event->getType();
    if(ty == CuEventI::CuResultEv || ty == CuEventI::CuProgressEv) {
        CuResultEvent *re = static_cast<CuResultEvent *>(event);
        const CuActivity *a = re->getActivity();
        // do not iterate directly on d->alimmap
        // clients may register / unregister from within onResult (onProgress)
        d->alimmap_locked = true; // prevent onResult to call unregisterActivity
        const std::multimap<const CuActivity *,  CuThreadListener *>& m = (d->alimmap);
        std::pair<std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator,
                    std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator> eqr = m.equal_range(a);
        for(std::multimap<const CuActivity *,  CuThreadListener *>::const_iterator it = eqr.first; it != eqr.second; ++it)  {
            if(re->getType() == CuEventI::CuProgressEv) {
                it->second->onProgress(re->getStep(), re->getTotal(), re->data);
            }
            else if(re->isList()) { // vector will be deleted from within ~CuResultEventPrivate
                const std::vector<CuData> &vd_ref = re->datalist;
                it->second->onResult(vd_ref);
            }
            else if(re->u_data) {
                it->second->onResult(re->u_data);
            }
            else {
                it->second->onResult(re->data);
            }
        }
//        if(d->arem_list.size() > 0)
//            printf("CuThread::onEventPosted \e[1;32m erasing now %ld unregistered activities...\e[0m\n", d->arem_list.size());
        for(std::list<CuActivity *>::const_iterator it = d->arem_list.begin(); it != d->arem_list.end(); ++it)
            d->alimmap.erase(*it);
        d->arem_list.clear();
        d->alimmap_locked = false;
    }
    else if(ty == CuEventI::CuA_ExitEvent) {
        mOnActivityExited(static_cast<CuA_ExitEv *>(event)->getActivity());
    }
    else if(ty == CuEventI::CuA_UnregisterEv) {
        m_activity_disconnect(static_cast<CuA_UnregisterEv *>(event)->getActivity());
    }
    else if(ty == CuEventI::CuThAutoDestroyEv) {
        // remove this thread from the cumbia internal thread list
        std::vector<CuThreadInterface *>::iterator it = std::find(d->threads_p->begin(), d->threads_p->end(), this);
        if(it != d->threads_p->end())
            d->threads_p->erase(it);
        wait();
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
void CuThread::publishProgress(const CuActivity* activity, int step, int total, const CuData &data) {
    d->eb->postEvent(new CuResultEvent(activity, step, total, data));
}

/*! \brief invoked in CuThread's thread, posts a *result event* to main thread
 *
 * @param a: the recepient of the event
 * @param data the data to be delivered with the result
 *
 * CuResultEvent is used in conjunction with CuThreadsEventBridge_I::postEvent
 */
void CuThread::publishResult(const CuActivity* a,  const CuData &da) {
    // da will be *moved* into a thread local data before
    // being posted to the event loop's thread
    d->eb->postEvent(new CuResultEvent(a, da));
}

void CuThread::publishResult(const CuActivity *a, const std::vector<CuData> &dalist)
{
    d->eb->postEvent(new CuResultEvent(a, dalist));
}

void CuThread::publishResult(const CuActivity *a, const CuUserData *u) {
    d->eb->postEvent(new CuResultEvent(a, u));
}

unsigned int CuThread::activityCount() const {
    return d->alimmap.size();
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
bool CuThread::matches(const std::string &other_thtok) const {
    assert(d->mythread == pthread_self());
    return this->d->token == other_thtok;
}

/*! \brief returns the thread token that was specified at construction time
 *
 * @return the CuData specified in the class constructor at creation time
 *
 * @see CuThread::CuThread
 */
std::string CuThread::getToken() const {
    return d->token;
}

/*! @private
 * does nothing
 */
void CuThread::cleanup() { }

/*! \brief returns 0 */
int CuThread::type() const {
    return CuThreadType;
}

/*! \brief internally used, allocates a new std::thread
 *
 * \note used internally
 *
 * return a new instance of std::thread
 */
void CuThread::start() {
    assert(d->mythread == pthread_self());
    try {
        d->thread = new std::thread(&CuThread::run, this);
        d->threads_p->push_back(this);
    }
    catch(const std::system_error &se) {
        perr("CuThread.start: failed to allocate thread resource: %s", se.what());
        abort();
    }
}

/*! @private
 * Thread loop
 */
void CuThread::run() {
    bool destroy = false;
    ThreadEvent *te = NULL;
    d->thpp = new CuThPP(d->se_p, d->eb); // thread local private data and methods
    while(1)  {
        te = NULL; {
            // acquire lock while dequeueing
            std::unique_lock<std::mutex> condvar_lock(d->mu);
            while(d->eq.empty()) {
                d->cv.wait(condvar_lock);
            }
            if(d->eq.empty()) // spurious wake up
                continue;
            te = d->eq.front();
            d->eq.pop();
        }
        if(te->getType() == ThreadEvent::RegisterActivity)  {
            CuThRegisterA_Ev *rae = static_cast<CuThRegisterA_Ev *>(te);
            d->thpp->activity_run(rae->activity, this);
        }
        else if(te->getType() == ThreadEvent::UnregisterActivity) {
            CuThUnregisterA_Ev *rae = static_cast<CuThUnregisterA_Ev *>(te);
            d->thpp->mExitActivity(rae->activity, this, false);
        }
        else if(te->getType() == ThreadEvent::TimerExpired)
        {
            // if at least one activity needs the timer, the
            // service will restart it after execution.
            // tmr is single-shot and needs restart to prevent
            // queueing multiple timeout events caused by slow activities
            bool tmr_restart = false;
            CuThreadTimer_Ev *tev = static_cast<CuThreadTimer_Ev *>(te);
            CuTimer *timer = tev->getTimer();
            std::list<CuActivity *> a_for_t = d->thpp->m_activitiesForTimer(timer); // no locks
            for(CuActivity *a : a_for_t) {
                const int& repeat = a->repeat();
                if(repeat > 0) { // periodic activity
                    a->doExecute(); // first
                    if(repeat != timer->timeout()) { // reschedule with new timeout
                        d->thpp->m_a_new_timeout(a, repeat, timer, this);
                    }
                    else if(!tmr_restart) // reschedule the same timer
                        tmr_restart = true;
                }
            } // for activity iter
            if(tmr_restart) { // restart timer if at least one activity needs it
                d->thpp->tmr_s->restart(timer);
            }
        }
        else if(te->getType() == ThreadEvent::PostToActivity) {
            CuThRun_Ev *tce = static_cast<CuThRun_Ev *>(te);
            CuActivity *a = tce->getActivity();
            CuActivityEvent* ae = tce->getEvent();
            // timeout change: m_a_new_timeout:
            // 1. unregister and delete old timer (d->tmr_act_map.find(a))
            // 2. create a new timer and start it with the required timeout
            if(ae->getType() == CuActivityEvent::TimeoutChange && d->thpp->tmr_amap.find(a) != d->thpp->tmr_amap.end())
                d->thpp->m_a_new_timeout(a, static_cast<CuTimeoutChangeEvent *>(ae)->getTimeout(),  d->thpp->tmr_amap.find(a)->second, this);
            // prevent event delivery to an already deleted action
            if(d->thpp->activity_set.find(a) != d->thpp->activity_set.end())
                a->event(ae);
            delete ae;
        }
        else if(te->getType() == ThreadEvent::ThreadExit || te->getType() == ThreadEvent::ZeroActivities) {
            // ExitThreadEvent enqueued by mOnActivityExited (foreground thread)
            destroy = (te->getType()  == ThreadEvent::ZeroActivities);
            delete te;
            break;
        }
        if(te)
            delete te;
    }
    /* on thread exit */
    /* empty and delete queued events */
    while(!d->eq.empty()) {
        ThreadEvent *qte = d->eq.front();
        // this shall be checked in the future. do we need it?
        if(qte->getType() == ThreadEvent::RegisterActivity) {
            perr("thread %s exits with a still queued register event for activity %s", d->token.c_str(), datos(static_cast<CuThRegisterA_Ev *>(qte)->activity->getToken()));
        }
        d->eq.pop();
        delete qte;
    }
    std::set acopy(d->thpp->activity_set);
    std::set<CuActivity *>::iterator i;
    for(i = acopy.begin(); i != acopy.end(); ++i) {
        d->thpp->mExitActivity(*i, this, true);
    }
    // post destroy event so that it will be delivered in the *main thread*
    if(destroy) {
        d->eb->postEvent(new CuThreadAutoDestroyEvent());
    }
}

/*! \brief returns true if the thread is running
 *
 * @return true if the thread is running
 */
bool CuThread::isRunning() {
    return d->thread != NULL;
}


// see mExitActivity
/*! @private */
void CuThread::mOnActivityExited(CuActivity *a) {
    assert(d->mythread == pthread_self());
    if(a->getFlags() & CuActivity::CuADeleteOnExit)
        delete a;
    if(d->alimmap.size() == 0) {
        m_zero_activities();
    }
};

void CuThread::m_zero_activities() {
    assert(d->mythread == pthread_self());
    if(!d->exiting) {
        d->exiting = true;
        ThreadEvent *zeroa_e = new CuThZeroA_Ev; // auto destroys
        std::unique_lock lk(d->mu);
        d->eq.push(zeroa_e);
        d->cv.notify_one();
    }
};

/*! @private
 *  Replicate what Cumbia::unregisterActivity does.
 *  Thread: main
 */
void CuThread::m_activity_disconnect(CuActivity *a) {
    assert(d->mythread == pthread_self());
    d->alimmap.erase(a);
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
void CuThread::postEvent(CuActivity *a, CuActivityEvent *e) {
    ThreadEvent *event = new CuThRun_Ev(a, e);
    /* need to protect event queue because this method is called from the main thread while
     * the queue is dequeued in the secondary thread
     */
    std::unique_lock lk(d->mu);
    d->eq.push(event);
    d->cv.notify_one();
}

/*! @private */
void CuThread::onTimeout(CuTimer *sender) {
    // unique lock to push on the event queue
    std::unique_lock ulock(d->mu);
    CuThreadTimer_Ev *te = new CuThreadTimer_Ev(sender);
    d->eq.push(te);
    d->cv.notify_one();
}

/*! @private */
void CuThread::wait() {
    if(d->thread) {
        d->thread->join();
        delete d->thread;
        d->thread = NULL;
    }
}
