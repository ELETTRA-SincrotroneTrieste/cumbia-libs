#include "cutimerservice.h"
#include <map>
#include <vector>
#include <cutimerlistener.h>
#include <cutimer.h>
#include <cueventloop.h>
#include <algorithm>
#include <cumacros.h>
#include <shared_mutex>

class CuTimerServicePrivate {
public:
    // timeout --> [listeners (threads) --> timer map]
    std::map<int, CuTimer *> ti_map;
    // for fast listener --> timer search
    std::map<const CuTimerListener *, CuTimer *> ti_cache;
    std::shared_mutex shared_mutex;
    std::list<CuTimerListener *>restart_queue;
    int timer_max_count;
};

/*!
 * \brief Class constructor
 * Initializes timerMaxCount to 25
 */
CuTimerService::CuTimerService() {
    d = new CuTimerServicePrivate;
    d->timer_max_count = 25;
}

CuTimerService::~CuTimerService()
{
    m_stopAll();
    d->ti_map.clear();
    d->ti_cache.clear();
    delete d;
}

/*!
 * \brief Register a new CuTimerListener with a given timeout. Reuse existing timers if possible. Listeners shall be
 *        notified in the event loop thread if the method is called with a non null reference to CuEventLoopService,
 *        in the CuTimer's thread otherwise.
 *
 * \param timer_listener a CuTimerListener (e.g. CuThread)
 * \param timeout the desired timeout
 * \param loop_service: if specified (not null) then the timeout callback is executed in the same thread
 * as the event loop thread (usually, the main thread). \since 1.3.0
 * \return a new CuTimer or a CuTimer already in use
 *
 * \par Timer registration
 *
 * A timer with the same timeout is reused and the listener and loop service are added to the instance.
 *
 * \note
 * If the same listener is registered with another loo_s, the new one replaces the old one.
 */
CuTimer *CuTimerService::registerListener(CuTimerListener *tl,
                                          int timeout,
                                          CuEventLoopService *loo_s)
{
    std::unique_lock lock(d->shared_mutex);
    CuTimer *timer = m_tmr_find(timeout);
    printf("\e[0;31mCuTimerService.registerListener: lis %p timeout %d found %s\n", tl, timeout, timer ? "YES" : "NO");
    if(!timer) {
            timer = new CuTimer(loo_s);
            timer->setTimeout(timeout);
            pgreen("CuTimerService::registerListener timers count for timeout %d is %ld > max timers %d: \e[1;32mcreating NEW timer %p\e[0m] that has now %ld listeners\e[0m\n",
                   timeout, d->ti_map.count(timeout), d->timer_max_count, timer, timer->listenersMap().size());
            timer->start(timeout);
            d->ti_map[timeout] = timer; // timeout -> timer  map
            d->ti_cache[tl] = timer;// listeners -> timer cache
            printf("CuTimerService.registerListener: \e[1;34mthe timer created %p has loop_service %p and timer listener %p\e[0m\n", timer, loo_s, tl);
    }
    timer->addListener(tl, loo_s);
    return timer;
}

/*!
 * \brief Unregister a listener with the given timeout from the service
 * \param tl the CuTimerListner to unregister
 * \param timeout the to remove listener's timeout
 *
 * If the timer associated to tl has no more listeners after unregistration,
 * it will be stopped and deleted.
 */
void CuTimerService::unregisterListener(CuTimerListener *tl, int timeout)
{
    std::unique_lock lock(d->shared_mutex);
    CuTimer *t = m_findTimer(tl, timeout); // does not acquire lock
    if(!t)
        perr("CuTimerService.unregisterListener: no listener %p registered with timeout %d", tl, timeout);
    else {
        t->removeListener(tl); // CuTimer lock guards its listeners list
        if(t->listenersMap().size() == 0) {
            m_removeFromMaps(t);
            t->stop();
            printf("\e[1;31mCuTimerService::unregisterListener: deleting timer %p had timeout %d\e[0m\n",
                   t, t->timeout());
            delete t;
        }
    }
}

/*!
 * \brief Changes the timeout of the timer that serves the given listener
 * \param tl the CuTimerListener requiring a change in timeout
 * \param timeout the new timeout, in milliseconds
 * \return A fresh or existing timer satisfying the requirements in terms of timeout.
 *
 * \note
 * Internally, unregisterListener and registerListener are called
 */
CuTimer *CuTimerService::changeTimeout(CuTimerListener *tl, int from_timeo, int to_timeo)
{
    CuTimer *t = nullptr;
    {
        std::unique_lock lock(d->shared_mutex);
        t = m_findTimer(tl, from_timeo); // does not lock
    }
    printf("CuTimerService::changeTimeout frm %d to %d - timer found for timeo %d: %p\n", from_timeo, to_timeo, from_timeo, t);
    if(t) {
        unregisterListener(tl, t->timeout()); // locks
        t = registerListener(tl, to_timeo); // locks
        printf("CuTimerService::changeTimeout  returning timer %p timeo %d\n", t, t->timeout());

    }
    else if(!t)
        perr("CuTimerService.changeTimeout: no listener %p registered with timer's timeout %d", tl, from_timeo);
    return t;
}

/*!
 * \brief CuTimerService::isRegistered returns true if a timer is registered for the given listener and timeout
 * \param tlis the CuTimerListener
 * \param timeout the timeout
 * \return true if there is a timer registered for the given listener and timeout, false otherwise
 */
bool CuTimerService::isRegistered(CuTimerListener *tlis, int timeout) {
    std::shared_lock lock(d->shared_mutex);
    std::map<const CuTimerListener*, CuTimer *>::const_iterator it = d->ti_cache.find(tlis);
    if(it != d->ti_cache.end() && timeout == it->second->timeout())
        return true;
    return false;
}

/*!
 * \brief restart the timer t with interval millis
 * \param t the timer to restart
 * \param millis the desired interval
 *
 * Normally, CuThread asks CuTimerService to restart a timer after
 * activity execution, to prevent timer event accumulation on the
 * queue
 */
void CuTimerService::restart(CuTimer *t , int millis) {
    t->start(millis);
}

/*!
 * \brief Returns the name of the service
 * \return "CuTimerService"
 */
std::string CuTimerService::getName() const {
    return std::string("CuTimerService");
}

/*!
 * \brief CuTimerService::getType returns the service type
 * \return CuServices::Timer
 */
CuServices::Type CuTimerService::getType() const {
    return  CuServices::Timer;
}

void CuTimerService::m_stopAll()
{
    std::unique_lock lock(d->shared_mutex);
    for(std::map<int, CuTimer *>::iterator it = d->ti_map.begin(); it != d->ti_map.end(); ++it) {
        CuTimer *timer = it->second;
        timer->stop(); // stops and joins
    }
}

void CuTimerService::m_removeFromMaps(CuTimer *t) {
    // 1. find the timers connected to this listener (may be more than one)
    std::multimap<int, CuTimer *>::iterator iter;
    for(iter = d->ti_map.begin(); iter != d->ti_map.end(); ) {
        if(iter->second == t)  iter = d->ti_map.erase(iter);
        else ++iter;
    }
    std::multimap<const CuTimerListener*, CuTimer *>::iterator cacheiter = d->ti_cache.begin();
    while(cacheiter != d->ti_cache.end()) {
        if(cacheiter->second == t) cacheiter = d->ti_cache.erase(cacheiter);
        else ++cacheiter;
    }
}

/*! @private
 *
 * does not lock guard. Lock must be acquired by the caller
 */
CuTimer* CuTimerService::m_tmr_find(int timeout) const {
    std::map<int, CuTimer *>::const_iterator it = d->ti_map.find(timeout);
    return it != d->ti_map.end() ? it->second : nullptr;
}

/*! @private
 *
 * does not lock guard. Lock must be acquired by the caller
 */
CuTimer *CuTimerService::m_findTimer(const CuTimerListener *th, int timeout)
{
    std::multimap<const CuTimerListener *, CuTimer*>::const_iterator it;
    for(it = d->ti_cache.begin(); it != d->ti_cache.end(); ++it) {
        if(it->first == th && (timeout < 0 || it->second->timeout() == timeout))
            return it->second;
    }
    return nullptr;
}

/*!
 * \brief returns all the timers managed by the service
 * \return  a list of the registered CuTimer instances
 */
std::list<CuTimer *> CuTimerService::getTimers()
{
    std::list<CuTimer *>timers;
    std::shared_lock lock(d->shared_mutex);
    for(std::map<int, CuTimer *>::const_iterator it = d->ti_map.begin(); it != d->ti_map.end(); ++it)
        timers.push_back(it->second);
    return timers;
}

/*!
 * \brief Returns the list of CuTimerListener registered to the timer t
 * \param t the timer whose listeners the caller is seeking for
 * \return the list of listeners of the timer t
 */
std::set<CuTimerListener *> CuTimerService::getListeners(CuTimer *t) const {
    std::set<CuTimerListener *> l;
    for(auto i = t->m_lis_map.begin(); i != t->m_lis_map.end(); ++i)
        l.insert(i->first);
    return l;
}

