#ifndef CUTIMER_H
#define CUTIMER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>

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
class CuTimer
{
    friend class CuTimerService;

public:
    CuTimer();

    ~CuTimer();

    void setTimeout(int millis);

    void setSingleShot(bool single);

    int timeout() const;

    bool isSingleShot() const;

protected:
    void run();

private:
    std::list<CuTimerListener *> m_listeners;
    int m_timeout;
    bool m_quit, m_pause, m_exited, m_singleShot;

    std::thread *m_thread;
    std::mutex m_mutex, m_listeners_mutex;
    std::condition_variable m_wait;

    void addListener(CuTimerListener *l);
    void removeListener(CuTimerListener *l);
    std::list<CuTimerListener *> listeners();

    void pause();
    void start(int millis);
    void resume();
    void stop();
};

#endif // CUTIMER_H
