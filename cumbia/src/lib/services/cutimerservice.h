#ifndef CUTIMERSERVICE_H
#define CUTIMERSERVICE_H

#include <cuservicei.h>
#include <list>
#include <set>

class CuTimerServicePrivate;
class CuTimerListener;
class CuEventLoopService;
class CuTimer;

/*!
 * \brief The CuTimerService class provides access to cumbia timers. It is normally used internally by the library.
 *
 * CuTimerService implements CuServiceI in order to provide access to CuTimer objects. They are not directly available
 * to clients. Use registerListener to either get a timer. Timers are shared across all listeners registered with
 * the same timeout.
 * unregisterListener is called when a timer is no more needed.
 * CuTimer is single shot: CuTimerService::restart shall be called to restart the timer.
 * It is possible to get the list of CuTimerListener for a given timer and the list of all timers.
 */
class CuTimerService : public CuServiceI
{
public:
    CuTimerService();
    ~CuTimerService();
    CuTimer* registerListener(CuTimerListener *timer_listener, int timeout, CuEventLoopService *loop_service = nullptr);
    std::list<CuTimer *> getTimers();
    std::set<CuTimerListener *> getListeners(CuTimer *t) const;
    void unregisterListener(CuTimerListener *th, int timeout);
    CuTimer *changeTimeout(CuTimerListener *th, int from_timeo, int to_timeo);
    bool isRegistered(CuTimerListener *th, int timeout);
    void restart(CuTimer *t, int millis);

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    void m_stopAll();
    void m_removeFromMaps(CuTimer *t);
    CuTimer *m_tmr_find(int timeout) const;
    CuTimer *m_findTimer(const CuTimerListener *th, int timeout);

    CuTimerServicePrivate *d;
};

#endif // CUTIMERSERVICE_H
