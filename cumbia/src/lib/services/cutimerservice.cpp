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
    std::shared_mutex shmu;
    std::list<CuTimerListener *>restart_queue;
    int timer_max_count;
    int tmrcnt;
};

/*!
 * \brief Class constructor
 * Initializes timerMaxCount to 25
 */
CuTimerService::CuTimerService() {
    d = new CuTimerServicePrivate;
    d->timer_max_count = 25;
    d->tmrcnt = 0;
}

CuTimerService::~CuTimerService()
{
    m_stopAll();
    for(std::map<int, CuTimer *>::iterator it = d->ti_map.begin(); it != d->ti_map.end(); ++it)
        delete it->second;
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
 * \param tmrname an optional name for the timer to be created. If the timer is reused, passing a non empty
 *        tmrname will overwrite the timer name.
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
                                          CuEventLoopService *loo_s,
                                          const std::string& tmrname) {
    std::unique_lock lock(d->shmu);
    CuTimer *timer = m_tmr_find(timeout);
    if(!timer) {
            timer = new CuTimer(loo_s);
            timer->setTimeout(timeout);
            pgreen("CuTimerService::registerListener timers count for timeout %d is %ld > max timers %d: \e[1;32mcreating NEW timer %p\e[0m] that has now %ld listeners\e[0m\n",
                   timeout, d->ti_map.count(timeout), d->timer_max_count, timer, timer->listenersMap().size());
            d->ti_map[timeout] = timer; // timeout -> timer  map
            timer->d->m_id = ++d->tmrcnt;
    }
    if(tmrname.length() > 0)
        timer->d->m_name = tmrname;
    timer->start(timeout); // if pending, timeout is unaffected
    timer->addListener(tl, loo_s);
    d->ti_cache[tl] = timer; // listeners -> timer cache
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
    std::unique_lock lock(d->shmu);
    CuTimer *t = m_findTimer(tl, timeout); // does not acquire lock
    if(!t) {
        perr("CuTimerService.unregisterListener: no listener %p registered with timeout %d", tl, timeout);
        abort();
    }
    else {
        t->removeListener(tl); // CuTimer lock guards its listeners list
        if(t->listenersMap().size() == 0) {
            m_removeFromMaps(t);
            t->stop();
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
    CuTimer *t = nullptr; {
        std::unique_lock lock(d->shmu);
        t = m_findTimer(tl, from_timeo); // does not lock
    }
    if(t) {
        unregisterListener(tl, t->timeout()); // locks
        t = registerListener(tl, to_timeo); // locks
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
    std::shared_lock lock(d->shmu);
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
 * The timer is *rescheduled* if a timeout is already pending
 */
void CuTimerService::restart(CuTimer *t , int millis) {
    t->restart(millis);
}

/*!
 * \brief start the timer
 * \param t which timer
 * \param millis timeout in milliseconds
 *
 * if the timer is pending, nothing is done. Timer is started
 * otherwise.
 */
void CuTimerService::start(CuTimer *t, int millis) {
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
    std::unique_lock lock(d->shmu);
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
    std::shared_lock lock(d->shmu);
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
    for(auto i = t->d->m_lis_map.begin(); i != t->d->m_lis_map.end(); ++i)
        l.insert(i->first);
    return l;
}

