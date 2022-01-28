#ifndef CUTIMER_H
#define CUTIMER_H

#include <cueventloop.h>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <set>
#include <chrono>
#include <atomic>

class CuTimerListener;

/*! \brief a timer used by CuThread for periodic execution of an *activity*
 *
 * This class is used internally by the cumbia library.
 *
 * When the timer is started, it creates a new std::thread that calls
 * CuTimerListener::onTimeout each time the *timeout* expires.
 * CuTimerListener is installed in the CuTimer constructor.
 *
 * In the *cumbia library* CuTimer is used by CuThread. CuThread *is* a CuTimerListener and,
 * within CuThread::onTimeout a timer event is pushed into the CuThread's *event queue*.
 *
 * A timer can be started, stopped, paused and resumed. Timeout can be changed.
 * The timer can also be used in the *single shot* mode, and in this case it is run
 * only once. The timeout is in *milliseconds*. CuThread uses the timer in single shot
 * mode.
 *
 * CuThread creates and runs a CuTimer after an activity is registered. CuTimer is created
 * in the CuThread's background thread if the CuActivity::repeat method returns greater than
 * zero (in the private member CuThread::mActivityInit).
 *
 */
class CuTimer : public CuEventLoopListener
{
    friend class CuTimerService;

public:
    CuTimer(CuEventLoopService *loos);

    ~CuTimer();

    void setTimeout(int millis);
    void setSingleShot(bool single);
    void setEventLoop(CuEventLoopService *loop_sr);
    int timeout() const;
    bool isSingleShot() const;

    // CuEventLoopListener interface
public:
    void onEvent(CuEventI *e);

protected:
    void run();

private:
    std::set<CuTimerListener *> m_listeners;
    std::chrono::time_point<std::chrono::steady_clock> m_last_start_pt, m_first_start_pt;
    bool m_quit, m_pause, m_exited;

    std::atomic_int m_pending;
    std::atomic_bool m_skip;
    std::atomic_int m_timeout;

    std::thread *m_thread;
    std::mutex m_mutex;
    std::condition_variable m_wait;
    CuEventLoopService *m_loop_service;

    void addListener(CuTimerListener *l);
    void removeListener(CuTimerListener *l);
    std::set<CuTimerListener *> listeners();

    void reset();
    void start(int millis);
    void stop();

    void m_notify();

};

#endif // CUTIMER_H
