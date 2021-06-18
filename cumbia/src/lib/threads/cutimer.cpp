#include "cutimer.h"
#include "cuevent.h"
#include "cumacros.h"
#include "cutimerlistener.h"
#include <limits.h>



/*! \brief create the timer and install the listener
 *
 * @param loos a pointer to a CuEventLoopService
 *
 * \note if loos is not null, listeners will be notified in the CuEventLoopService thread
 * of execution.
 *
 * CuThread is a CuTimerListener
 *
 * By default, the timeout is set to 1000 milliseconds, and the single shot
 * property is true.
 */
CuTimer::CuTimer(CuEventLoopService *loos)
{
    m_quit = m_pause = m_exited = m_skip = false;
    m_pending = 0;
    m_timeout = 1000;
    m_thread = NULL;
    m_loop_service = loos;
    if(loos)
        m_loop_service->addCuEventLoopListener(this);
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
    m_timeout = millis;
    m_wait.notify_one();
}

/*!
 * \brief set or update the reference to the event loop service
 * \param loop_sr pointer to a CuEventLoopService
 * \since 1.3.0
 *
 * \note If null, the listeners will be notified *in the timer's thread*.
 */
void CuTimer::setEventLoop(CuEventLoopService *loop_sr) {
    if(loop_sr != m_loop_service) {
        m_loop_service = loop_sr;
        m_loop_service->addCuEventLoopListener(this);
    }
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
//    printf("CuTimer.start [thread 0x%lx] ->  timeout  %d\n", pthread_self(), millis );
    std::unique_lock<std::mutex> lock(m_mutex);
    m_quit = m_pause = false;
    m_timeout = millis;
    if(!m_thread) { // first time start is called or after stop
        m_thread = new std::thread(&CuTimer::run, this);
    }
    else {
        if(m_pending > 0) {
            reset();
//            m_last_start_pt = std::chrono::steady_clock::now();
        }
        else {
//            m_first_start_pt = m_last_start_pt = std::chrono::steady_clock::now();
        }
        m_pending++;
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

void CuTimer::m_notify() {
    std::list<CuTimerListener *>::const_iterator it;
    for(it = m_listeners.begin(); it != m_listeners.end(); ++it) {
//        printf("CuTimer.onEvent [thread 0x%lx] -> on timeout on %p\n", pthread_self(), (*it) );
        (*it)->onTimeout(this);
    }
//                auto t1 = std::chrono::steady_clock::now();
//                long int delta_first_ms = std::chrono::duration_cast<std::chrono::microseconds>(t1 - m_first_start_pt).count();
//                long int delta_last_ms = std::chrono::duration_cast<std::chrono::microseconds>(t1 - m_last_start_pt).count();
//                printf("\e[0;32mCuTimer.run: notified timeout after %ld ms instead of %ldus\e[0m\t\t\t(\e[1;31mdelay %ldus\e[0m)  THREAD 0x%ld\n",
//                       delta_first_ms, delta_last_ms, -delta_last_ms+delta_first_ms, pthread_self());
}

/*! \brief implements CuEventLoopListener interface.
 *
 */
void CuTimer::onEvent(CuEventI *e) {
    m_notify();
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
void CuTimer::run()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    unsigned long timeout = m_timeout;
    while (!m_quit) {
        timeout = m_timeout;
        std::cv_status status;
        std::chrono::milliseconds ms{timeout};
        status = m_wait.wait_for(lock, ms);
        if(m_skip && !m_quit) {
            m_skip = false;
        }
        else {
            m_pause ?  timeout = ULONG_MAX : timeout = m_timeout;
            // issues with wasm: erratically expected timeout status is no_timeout
            (void ) status;
            if(/*status == std::cv_status::timeout && */!m_quit && !m_pause) /* if m_exit: m_listener must be NULL */ {
                if(m_loop_service) {
                    m_loop_service->postEvent(this, new CuTimerEvent());
                }
                else
                    m_notify();
            }
            m_pending = 0;
            if(!m_quit) { // wait for next start()
                m_wait.wait(lock);
            }
        } // !m_skip
    }
}

