#include "cutimer.h"
#include "cumacros.h"
#include "cutimerlistener.h"

CuTimer::CuTimer(CuTimerListener *l)
{
    m_listener = l;
    m_quit = false;
    m_pause = false;
    m_exited = false;
    m_timeout = 1000;
    m_singleShot = true;
    m_thread = NULL;
}

CuTimer::~CuTimer()
{
    pdelete("CuTimer %p", this);
    if(!m_quit)
        stop();
}

void CuTimer::setTimeout(int millis)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    pbblue("CuTimer.setTimeout %d", millis);
    m_timeout = millis;
    m_terminate.notify_one();
}

void CuTimer::setSingleShot(bool single)
{
    m_singleShot = single;
}

int CuTimer::timeout() const
{
    return m_timeout;
}

bool CuTimer::isSingleShot() const
{
    return m_singleShot;
}

void CuTimer::pause()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    pbblue("CuTimer.pause");
    m_pause = true;
    m_terminate.notify_one();
}

void CuTimer::resume()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    pbblue("CuTimer.resume");
    m_pause = false;
    if(!m_quit) /* thread still running */
        m_terminate.notify_one();
    else  /* thread loop had been quit */
        start(m_timeout);
}

void CuTimer::start(int millis)
{
    m_quit = m_pause = false;
    if(m_thread)
        m_thread->join();
    if(m_thread)
        delete m_thread;
    m_timeout = millis;
    m_thread = new std::thread(&CuTimer::run, this);
}

void CuTimer::stop()
{
    if(m_exited)
        return; /* already quit */
    pbblue("CuTimer.stop!!!! for this %p before lock", this);
    {
        auto locked = std::unique_lock<std::mutex>(m_mutex);
        m_quit = true;
        m_listener = NULL;
    }
    m_terminate.notify_one();
    if(m_thread->joinable())
    {
        m_thread->join();
        pbblue("CuTimer.stop: JOINETH!");
    }
    else
        pbblue("CuTimer.stop: NOT JOINABLE!!!");
    m_exited = true;
    delete m_thread;
    m_thread = NULL;
}

void CuTimer::run()
{
    pbblue("CuTimer:run");
    std::unique_lock<std::mutex> lock(m_mutex);
    unsigned long timeout = m_timeout;
    while (!m_quit)
    {
        std::chrono::milliseconds ms{timeout};
        std::cv_status status = m_terminate.wait_for(lock, ms);
        cuprintf("CuTimer.run pause is %d status is %d timeout %d\n", m_pause, (int) status, m_timeout);
        //        if(status == std::cv_status::no_timeout)
        m_pause ?  timeout = 5000 : timeout = m_timeout;
        if(status == std::cv_status::timeout && m_listener)
        {
            pbblue("CuTimer:run: this: %p triggering timeout in pthread 0x%lx (CuTimer's) m_listener %p m_exit %d CURRENT TIMEOUT is %lu m_pause %d", this,
                   pthread_self(), m_listener, m_quit, timeout, m_pause);
            if(m_listener && !m_quit && !m_pause) /* if m_exit: m_listener must be NULL */
            {
                m_listener->onTimeout(this);
            }
        }
        /* check status: don't exit for a timeout change! Don't exit if we are in pause either */
        if(m_singleShot && (status == std::cv_status::timeout) & !m_pause)
            m_quit = true;
    }
    pbblue("CuTimer:run: exiting!");
}

