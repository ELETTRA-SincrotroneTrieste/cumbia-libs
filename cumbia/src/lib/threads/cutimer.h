#ifndef CUTIMER_H
#define CUTIMER_H

#include <cueventloop.h>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <unordered_map>
#include <atomic>
#include <chrono>

class CuTimerListener;

// definition here in .h because friend class CuTimerService needs it
//
class CuTimerPrivate {
public:
    CuTimerPrivate() : m_quit(false), m_pause(false), m_pending(false), /*m_skip(false),*/
        m_timeout(1000), m_thread(nullptr) {}

    ~CuTimerPrivate() {}

    std::unordered_map<CuTimerListener *, CuEventLoopService *> m_lis_map;
    bool m_quit, m_pause;

    std::atomic_int m_pending;
    // bool m_skip;
    int m_timeout;

    std::thread *m_thread;
    std::mutex m_mutex;
    std::condition_variable m_wait;

    int m_id;
    std::string m_name;

private:
    CuTimerPrivate(const CuTimerPrivate& other) {}
//    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_start_pt, m_start_pt;
};
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

    void setName(const std::string& name);
    std::string name() const;
    bool pending() const;
    int id() const;
    void setTimeout(int millis);
    void setSingleShot(bool single);
    int timeout() const;
    bool isSingleShot() const;

    // CuEventLoopListener interface
public:
    void onEvent(CuEventI *e);

protected:
    void run();

private:

    void addListener(CuTimerListener *l, CuEventLoopService *ls);
    void removeListener(CuTimerListener *l);
    std::unordered_map<CuTimerListener *, CuEventLoopService *> listenersMap();

    void restart();
    void start(int millis = 0);
    void stop();

    void m_notify();

    CuTimerPrivate *d;
};

#endif // CUTIMER_H
