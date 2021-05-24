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
    std::multimap<int, CuTimer *> ti_map;
    // for fast listener --> timer search
    std::multimap<const CuTimerListener *, CuTimer *> ti_cache;
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
 * If a timer associated to the given listener and timeout is already registered, it is reused.
 * The CuEventLoopService reference is updated on the timer.
 * If timer_listener is new, the behaviour depends on the number of timers for the given timeout and
 * the value of timerMaxCount:
 * \li number of timers <= timersMaxCount: a new timer is returned
 * \li an existing timer with the given timeout is reused
 *
 * \par Note: if the timer is reused, loop_service is updated on the timer itself.
 *
 * @see timerMaxCount
 * @see setTimerMaxCount
 */
CuTimer *CuTimerService::registerListener(CuTimerListener *timer_listener, int timeout, CuEventLoopService *loop_service)
{
    std::unique_lock lock(d->shared_mutex);
    CuTimer *timer = m_findTimer(timer_listener, timeout);
    if(!timer) {
        if(d->ti_map.count(timeout) >= static_cast<size_t>(d->timer_max_count)) {
            // we have one or more timers with this timeout: reuse the one with less listeners
            timer = m_findReusableTimer(timeout); // const, no map modification
            timer->setEventLoop(loop_service); // update loop_service
            pgreen("CuTimerService::registerListener timers count for timeout %d is %ld >= max timers %d: \e[0;32mreusing timer %p\e[0m] that has now %ld listeners\e[0m\n",
                   timeout, d->ti_map.count(timeout), d->timer_max_count, timer, timer->listeners().size());
        }
        else {
            timer = new CuTimer(loop_service);
            timer->setTimeout(timeout);
            pgreen("CuTimerService::registerListener timers count for timeout %d is %ld > max timers %d: \e[1;32mcreating NEW timer %p\e[0m] that has now %ld listeners\e[0m\n",
                   timeout, d->ti_map.count(timeout), d->timer_max_count, timer, timer->listeners().size());
            timer->start(timeout);
            std::pair<int, CuTimer *> new_tmr(timeout, timer);
            d->ti_map.insert(new_tmr);  // timeout -> timer  map
        }
        timer->addListener(timer_listener);
        std::pair<CuTimerListener *, CuTimer *> ltp(timer_listener, timer);
        d->ti_cache.insert(ltp); // listeners -> timer cache
    }
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
        if(t->listeners().size() == 0) {
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
    CuTimer *t = nullptr;
    {
        std::unique_lock lock(d->shared_mutex);
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
    std::shared_lock lock(d->shared_mutex);
    std::pair<std::multimap<const CuTimerListener*, CuTimer *>::const_iterator, std::multimap<const CuTimerListener*, CuTimer *>::const_iterator > iterpair;
    iterpair = d->ti_cache.equal_range(tlis); // find timers connected to the listener l
    for(std::multimap<const CuTimerListener*, CuTimer *>::const_iterator cacheiter = iterpair.first; cacheiter != iterpair.second; ++cacheiter) {
        if(cacheiter->second->timeout() == timeout)
            return true;
    }
    return false;
}

/*!
 * \brief Set the maximum number of timers that are allowed (per period)
 *
 * \param count the desired maximum number of timers for each period.
 *
 * After timerMaxCount number of timers is reached, timers are reused
 *
 *  \par Default value
 * The default value is 25.
 *
 * \note
 * Please configure this number before registering activities:
 * changing this number later does not redistribute already registered timers.
 */
void CuTimerService::setTimerMaxCount(int count) {
    d->timer_max_count = count;
}

int CuTimerService::timerMaxCount() const
{
    return d->timer_max_count;
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
CuTimer* CuTimerService::m_findReusableTimer(int timeout) const {
    int min = -1;
    CuTimer *reuse = nullptr;
    std::pair<std::multimap<int, CuTimer *>::const_iterator, std::multimap<int, CuTimer *>::const_iterator > ret;
    ret = d->ti_map.equal_range(timeout); // find all timers with desired timeout
    for(std::multimap<int, CuTimer *>::const_iterator it = ret.first; it != ret.second; ++it) {
        std::list<CuTimerListener *> tm_lis = it->second->listeners();
        int listeners_cnt = tm_lis.size();
        if(min < 0) { // first time
            min = listeners_cnt;
            reuse = it->second;
        }
        else if(listeners_cnt < min) {
            min = listeners_cnt;
            reuse = it->second;
        }
    }
    return reuse;
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
std::list<CuTimerListener *> CuTimerService::getListeners(CuTimer *t) const {
    return t->m_listeners;
}

