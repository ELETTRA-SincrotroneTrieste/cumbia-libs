#include "cuhltimer.h"

#include <climits>
#include <set>
#include <cutimerservice.h>
#include <cumbia.h>
#include <cuserviceprovider.h>
#include <cueventloop.h>
#include <cutimerlistener.h>
#include <cutimer.h>
#include <cumacros.h>

class CuHLTimer_P {
public:
    CuHLTimer_P(Cumbia *c)  : ticks{0}, period{0}, cu{c}, tserv(nullptr) {
        // period == 0 indicates timer not running
    }

    unsigned int ticks, period; // count timeouts
    Cumbia *cu;
    CuTimerService *tserv;
    std::multimap<unsigned int, CuHLTimerListener *>limap;
};

CuHLTimer::CuHLTimer(Cumbia *c) {
    d = new CuHLTimer_P(c);
}

CuHLTimer::~CuHLTimer() {
    delete d;
}

bool CuHLTimer::m_duplicate_pair(CuHLTimerListener *tl, unsigned int timeout) const {
    auto a = d->limap.equal_range(timeout);
    for(auto b = a.first; b != a.second; ++b)
        if(b->second == tl)
            return true;
    printf("%p / %u not duplicate\n", tl, timeout);
    return false;
}

/*!
 * \brief start executing the timer after one or more listeners have been configured with addTimerListener
 *
 * In case of multiple timeouts, they are rounded up so as to be multiples of the smallest timeout.
 * Duplicated couples (timeout, listener) are removed.
 */
void CuHLTimer::start() {
    // d->timeouts is sorted
    if(d->limap.size() > 0) {
        d->period = d->limap.begin()->first; // d->period > 0 --> timer running
        d->tserv = static_cast<CuTimerService *>(d->cu->getServiceProvider()->get(CuServices::Timer));
        if(!d->tserv) {
            d->tserv = new CuTimerService();
            d->cu->getServiceProvider()->registerService(CuServices::Timer, d->tserv);
        }
        CuEventLoopService *loos = static_cast<CuEventLoopService *>(d->cu->getServiceProvider()->get(CuServices::EventLoop));
        if(!loos) {
            loos = new CuEventLoopService();
            d->cu->getServiceProvider()->registerService(CuServices::EventLoop, loos);
        }
        d->tserv->registerListener(this, d->period, loos);
    }
    else {
        perr("CuHLTimer.start: timeout not configured. Use addTimerListener with a valid non zero integer timeout");
    }
}

void CuHLTimer::stop() {
    d->tserv->unregisterListener(this, d->period);
    d->period = 0; // indicates timer not running
}

bool CuHLTimer::isRunning() const {
    return d->period > 0;
}

/*!
 * \brief add a timer listener and a notification period (called timeout)
 * \param tl a CuHLTimerListener
 * \param timeout period (>0) in milliseconds for the periodic invocation of CuHLTimerListener::onTimeout
 *
 * After adding listeners with their timeouts, call CuHLTimer::start
 * This method can be called after the timer is started.
 */
bool CuHLTimer::addTimerListener(CuHLTimerListener *tl, unsigned int timeout) {
    bool ok = timeout > 0;
    if(ok && !m_duplicate_pair(tl, timeout)) {
        ok = checkTimeout(timeout);
        if(ok)
            d->limap.insert(std::pair<int, CuHLTimerListener *>(timeout, tl));
    }
    return ok;
}

/*! \brief remove a listener from this timer
 *
 * If the number of listeners drops to zero, the timer is stopped, and needs to be started again
 * after adding new listeners.
 */
void CuHLTimer::removeTimerListener(CuHLTimerListener *tl) {
    auto it = d->limap.begin();
    while(it != d->limap.end()) {
        if(it->second == tl)
            it = d->limap.erase(it);
        else
            ++it;
    }
    if(d->limap.size() == 0)
        stop();
}

bool CuHLTimer::checkTimeout(unsigned int t) const {
    bool ok;
    std::multimap<unsigned int, CuHLTimerListener *>::const_iterator it = d->limap.begin();
    while(it != d->limap.end() && t < d->limap.begin()->first && it->first % t == 0) {
        ++it;
    }
    ok = (it == d->limap.end() || it == d->limap.begin());
    if(!ok) {
        perr("CuHLTimer::checkTimeout: timeout %u does not divide %u exactly (current period unit)", t, it->first);
    }
    else if(d->limap.size() > 0 && t > d->limap.begin()->first)
        ok = (t % d->limap.begin()->first == 0);
    if(!ok)
        perr("CuHLTimer::checkTimeout: timeout %u is not divided by smallest period unit %u", t, d->limap.begin()->first);
    return ok;
}

void CuHLTimer::onTimeout(CuTimer *t) {
    d->ticks++;
    std::multimap<unsigned int, CuHLTimerListener *>::const_iterator it = d->limap.begin();
    int i = 0, elapsed = d->ticks * d->period;
    while(it != d->limap.end()) {
        if(elapsed >= it->first && elapsed % it->first == 0) {
            // printf("CuHLTimer.onTimeout: \e[0;32m%d\e[0mms elapsed (unit period %dms): elapsed period \e[1;36m%d\e[0m\n", elapsed, t->timeout(), it->first);
            it->second->onTimeout(it->first);
        }
        if(i++ == d->limap.size() && d->ticks * d->period >= it->first)
            d->ticks = 0;
        ++it;
    }
    // timeouts changed with addTimeout or removeTimeout?
    // if so, restart with smallest period
    if(d->limap.size() && d->limap.begin()->first != t->timeout())
        t->setTimeout(d->limap.begin()->first);
    d->tserv->restart(t);
}
