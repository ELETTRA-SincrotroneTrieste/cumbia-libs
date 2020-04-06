#include "cutimer.h"
#include "cumacros.h"
#include "cutimerlistener.h"
#include <limits.h>

/*! \brief create the timer and install the listener
 *
 * @param l a CuTimerListener
 *
 * CuThread is a CuTimerListener
 *
 * By default, the timeout is set to 1000 milliseconds, and the single shot
 * property is true.
 */
CuTimer::CuTimer()
{
    m_quit = m_pause = m_exited = m_skip = false;
    m_pending = 0;
    m_timeout = 1000;
    m_thread = NULL;
}

/*! \brief class destructor
 *
 * If still running, CuTimer::stop is called to interrupt the timer
 * and join the thread
 */
CuTimer::~CuTimer()
{
    pdelete("CuTimer %p m_quit %d", this, m_quit);
    if(!m_quit)
        stop();
}

/*!
 * \brief change the timeout
 * \param millis the new timeout
 */
void CuTimer::setTimeout(int millis)
{
    pbblue("CuTimer.setTimeout %d", millis);
    m_timeout = millis;
    m_wait.notify_one();
}

/*!
 * \brief return the timeout in milliseconds
 * \return the timeout in milliseconds
 */
int CuTimer::timeout() const
{
    return m_timeout;
}

/*!
 * \brief cancels current scheduled timeout.
 *
 * \note
 * start must be called after reset if you want to schedule
 * the next timeout
 *
 * \note
 * Not lock guarded
 */
void CuTimer::reset() {
    m_skip = true;
    printf("CuTimer.reset: calling m_wait.notify_one\n");
    m_wait.notify_one();
}

/*! \brief start the timer with the given interval in milliseconds
 *
 * @param millis the desired timeout
 *
 * If the timer is still running, CuTimer waits for it to finish before starting
 * another one
 */
void CuTimer::start(int millis)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_quit = m_pause = false;
    m_timeout = millis;
    if(!m_thread) { // first time start is called or after stop
        m_thread = new std::thread(&CuTimer::run, this);
    }
    else {
        if(m_pending > 0) {
            printf("CuTimer.start: m_pending is true: calling reset\n");
            reset();
            //            m_last_start_pt = std::chrono::steady_clock::now();
        }
        else {
            printf("CuTimer.start: m_pending is false\n");
                        m_first_start_pt = m_last_start_pt = std::chrono::steady_clock::now();
        }
        m_pending++;
        printf("CuTimer.start: calling m_wait.notify_one from THREAD 0x%ld\n", pthread_self());
        m_wait.notify_one();
    }
}

/*! \brief stops the timer, if active
 *
 * stops the timer, joins the timer thread and deletes it
 */
void CuTimer::stop()
{
    if(m_exited)
        return; /* already quit */
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_quit = true;
        m_listeners.clear();
        m_wait.notify_one();
    }
    pgreen("CuTimer.stop called m_wait.notify_one...");
    if(m_thread->joinable()) {
        m_thread->join();
    }
    pbblue("CuTimer.stop joined this %p\n", this);
    m_exited = true;
    delete m_thread;
    m_thread = nullptr;
}

/*!
 * \brief CuTimer::addListener adds a listener to the timer
 * \param l the new listener
 *
 * This method can be accessed from several different threads
 */
void CuTimer::addListener(CuTimerListener *l) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_listeners.push_back(l);
}

/*!
 * \brief CuTimer::removeListener removes a listener from the timer
 * \param l the listener to remove
 *
 * This method can be accessed from several different threads
 */
void CuTimer::removeListener(CuTimerListener *l) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_listeners.remove(l);
}

/*!
 * \brief CuTimer::listeners returns the list of registered listeners
 *
 * @return the list of registered listeners
 *
 * This method can be accessed from several different threads
 */
std::list<CuTimerListener *> CuTimer::listeners() {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_listeners;
}

/*! \brief the timer loop
 *
 * \note internally used by the library
 *
 * The timer loop waits for the timeout to expire before quitting (if single shot)
 * or waiting again
 */

int cycle_cnt = 0;

void CuTimer::run()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    unsigned long timeout = m_timeout;
    while (!m_quit) {
        timeout = m_timeout;
        std::cv_status status;
        std::chrono::milliseconds ms{timeout};
        printf("CuTimer.run: waiting for condition (TIMER) %p THREAD 0x%ld\n", this, pthread_self());
        status = m_wait.wait_for(lock, ms);
        if(m_skip && !m_quit) {
            printf("\e[1;31mCuTimer.run: cycle %d: skipping this time... pending %d THREAD 0x%ld\e[0m\n", cycle_cnt, static_cast<int>(m_pending), pthread_self());
            m_skip = false;
        }
        else {
            printf("\n------------------CYCLE %d status is %d ------------------\n", ++cycle_cnt, status);
            if(status == std::cv_status::no_timeout)
                printf("CuTimer.run: STATUS IS no_timeout!!!!!!!!!!!!!!\n");
            m_pause ?  timeout = ULONG_MAX : timeout = m_timeout;
            if(/*status == std::cv_status::timeout && */!m_quit && !m_pause) /* if m_exit: m_listener must be NULL */
            {
                std::list<CuTimerListener *>::const_iterator it;
                for(it = m_listeners.begin(); it != m_listeners.end(); ++it) {
                    (*it)->onTimeout(this);
                }
                                auto t1 = std::chrono::steady_clock::now();
                                long int delta_first_ms = std::chrono::duration_cast<std::chrono::microseconds>(t1 - m_first_start_pt).count();
                                long int delta_last_ms = std::chrono::duration_cast<std::chrono::microseconds>(t1 - m_last_start_pt).count();
                                printf("\e[0;32mCuTimer.run: notified timeout after %ld ms instead of %ldus\e[0m\t\t\t(\e[1;31mdelay %ldus\e[0m)  THREAD 0x%ld\n",
                                       delta_first_ms, delta_last_ms, -delta_last_ms+delta_first_ms, pthread_self());
            }
            else
                printf("CuTimer.run: status is %d m_quit is %d m_pause is %d\n", status, m_quit, m_pause);
            m_pending = 0;
            if(!m_quit) { // wait for next start()
                printf("CuTimer.run: ---- END OF CYCLE ---- waiting for next start (m_wait.wait(lock)  THREAD 0x%ld ) \n", pthread_self());
                m_wait.wait(lock);
            }
        } // !m_skip
    }
}

