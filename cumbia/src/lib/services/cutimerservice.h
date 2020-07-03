#ifndef CUTIMERSERVICE_H
#define CUTIMERSERVICE_H

#include <cuservicei.h>
#include <list>

class CuTimerServicePrivate;
class CuTimerListener;
class CuTimer;

/*!
 * \brief The CuTimerService class provides access to cumbia timers. It is normally used internally by the library
 *        itself, but it can be occasionally employed by applications to limit the maximum number of allocated timers,
 *        for optimization purposes.
 *
 * CuTimerService implements CuServiceI in order to provide access to CuTimer objects. They are not directly available
 * to clients. Use registerListener to either get a fresh timer or reuse an existing one, according to setTimerMaxCount.
 * unregisterListener is called when a timer is no more needed.
 * CuTimer is single shot: CuTimerService::restart method must then be called when the client needs to wait for a new timeout.
 * Finally, it is possible to get the list of CuTimerListener for a given timer and the list of all timers.
 * The timerMaxCount method returns the upper limit on the timers count set with setTimerMaxCount or the default value, which is
 * 25. Each timer runs in a separate thread. That's why library offers the possibility to curb the number of timers. It makes sense
 * if the thread creation policy of the engine in use promotes distributing sources across different threads.
 *
 * \par Changing the max timers count.
 * From an application, get a handle to the CuTimerService from Cumbia and then call CuTimerService::setTimerMaxCount
 *
 * \code
  CuTimerService *ts = static_cast<CuTimerService *>(cumbia_ptr->getServiceProvider()->get(CuServices::Timer));
  ts->setTimerMaxCount(m_conf.max_timers);
 * \endcode
 *
 */
class CuTimerService : public CuServiceI
{
public:
    CuTimerService();
    ~CuTimerService();
    CuTimer* registerListener(CuTimerListener *timer_listener, int timeout);
    std::list<CuTimer *> getTimers();
    std::list<CuTimerListener *>getListeners(CuTimer *t) const;
    void unregisterListener(CuTimerListener *th, int timeout);
    CuTimer *changeTimeout(CuTimerListener *th, int from_timeo, int to_timeo);
    bool isRegistered(CuTimerListener *th, int timeout);
    void setTimerMaxCount(int count);
    int timerMaxCount() const;
    void restart(CuTimer *t, int millis);

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    void m_stopAll();
    void m_removeFromMaps(CuTimer *t);
    CuTimer *m_findReusableTimer(int timeout) const;
    CuTimer *m_findTimer(const CuTimerListener *th, int timeout);

    CuTimerServicePrivate *d;
};

#endif // CUTIMERSERVICE_H
