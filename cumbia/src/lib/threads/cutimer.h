#ifndef CUTIMER_H
#define CUTIMER_H

#include <thread>
#include <mutex>
#include <condition_variable>

class CuTimerListener;

class CuTimer
{
public:
    CuTimer(CuTimerListener *l);

    ~CuTimer();

    void setTimeout(int millis);

    void setSingleShot(bool single);

    int timeout() const;

    bool isSingleShot() const;

    void pause();

    void start(int millis);

    void resume();

    void stop();

protected:
    void run();

private:
    CuTimerListener *m_listener;
    unsigned long m_timeout;
    bool m_quit, m_pause, m_exited, m_singleShot;

    std::thread *m_thread;
    std::mutex m_mutex;
    std::condition_variable m_terminate;
};

#endif // CUTIMER_H
