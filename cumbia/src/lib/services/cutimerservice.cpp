#include "cutimerservice.h"
#include <map>
#include <vector>
#include <cutimerlistener.h>
#include <cutimer.h>
#include <algorithm>
#include <cumacros.h>
#include <mutex>

class CuTimerServicePrivate {
public:
    // timers -> timeout map
    std::map<int,  CuTimer*> ti_map;

    std::mutex mutex;
};

CuTimerService::CuTimerService() {
    d = new CuTimerServicePrivate;
}

CuTimerService::~CuTimerService()
{
    m_stopAll();
    d->ti_map.clear();
    delete d;
}

CuTimer *CuTimerService::registerListener(CuTimerListener *th, int timeout)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    CuTimer *timer = m_getTimer(th);
    if(timer && timeout == timer->timeout()) {
        printf("\e[1;33m***\e[0m CuTimerService.registerThread: thread %p already registered with timeout %d\n", th, timeout);
        return timer;
    }
    else if(timer && timer->timeout() != timeout) {
        // found the thread th but different timeout, unregiter it from its timer
        CuTimer *timer = d->ti_map[timeout];
        timer->removeListener(th);
    }
    // find a timer with the needed timeout
    if(d->ti_map.count(timeout) > 0)
        timer = d->ti_map[timeout];
    if(!timer) {
        printf("\e[1;32m***\e[0m CuTimerService::registerThread \e[1;32m need to create a new timer with period %d\e[0m\n",
               timeout);
        timer = new CuTimer();
        timer->setTimeout(timeout);
        timer->setSingleShot(false);
        d->ti_map[timeout] = timer;
        timer->start(timeout);
    }
    else {
        printf("\e[1;32m***\e[0m CuTimerService::registerThread \e[1;35m timer already found with period %d\e[0m\n", timeout);
    }
    timer->addListener(th);
    return timer;
}

void CuTimerService::unregisterListener(CuTimerListener *tl)
{
    std::lock_guard<std::mutex> lock(d->mutex);
    CuTimer *t = m_getTimer(tl); // does not acquire lock
    t->removeListener(tl); // CuTimer lock guards its listeners list
    printf("\e[1;32m**\e[0m CuTimerService::unregisterListener: unregistering listener %p (timer %p): there are still %ld listeners\n", tl, t, t->listeners().size());
    if(t->listeners().size() == 0) {
        int to = t->timeout();
        printf("\e[1;32m** -->\e[0m CuTimerService::unregisterListener: \e[1;32mno more listeners: stopping timer %p and deleting\e[0m\n", t);
        t->stop();
        delete t;
        d->ti_map.erase(to);
    }
}

void CuTimerService::changeTimeout(CuTimerListener *th, int timeout)
{
    std::lock_guard<std::mutex> lock(d->mutex);

}

bool CuTimerService::isRegistered(CuTimerListener *tlis, int timeout) {
    std::lock_guard<std::mutex> lock(d->mutex);
    std::list<CuTimerListener *> listeners = m_getTimerListeners(timeout);
    std::list<CuTimerListener *>::const_iterator it = find(listeners.begin(), listeners.end(), tlis);
    return it != listeners.end();
}


std::string CuTimerService::getName() const {
    return std::string("CuTimerService");
}

CuServices::Type CuTimerService::getType() const {
    return  CuServices::Timer;
}

void CuTimerService::m_stopAll()
{
    for(std::map<int, CuTimer *>::iterator it = d->ti_map.begin(); it != d->ti_map.end(); ++it) {
        CuTimer *timer = it->second;
        timer->stop(); // stops and joins
    }
}

/*! @private
 *
 * does not lock guard. Lock must be acquired by the caller
 */
std::list<CuTimerListener *> CuTimerService::m_getTimerListeners(int timeout)
{
    std::list<CuTimerListener *> listeners;
    if(d->ti_map.count(timeout) > 0) {
        CuTimer *timer = d->ti_map[timeout];
        if(timer) {
            listeners = timer->listeners();
        }
    }
    return listeners;
}

/*! @private
 *
 * does not lock guard. Lock must be acquired by the caller
 */
CuTimer* CuTimerService::m_getTimer(const CuTimerListener *th)
{
    std::map<int, CuTimer *>::iterator it;
    for( it = d->ti_map.begin(); it != d->ti_map.end(); ++it) {
        std::list<CuTimerListener *>listeners = it->second->listeners();
        std::list<CuTimerListener *>::const_iterator tit = std::find(listeners.begin(), listeners.end(), th);
        if(tit != listeners.end()) // found a thread with the given timeout
            return it->second; // timeout
    }
    return nullptr;
}

