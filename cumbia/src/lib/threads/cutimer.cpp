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
        m_lis_map.clear();
        m_wait.notify_one();
    }
    if(m_thread->joinable()) {
        m_thread->join();
    }
    m_exited = true;
    delete m_thread;
    m_thread = nullptr;
}

void CuTimer::m_notify() {
    int timeout = m_timeout;
    for(std::map<CuTimerListener *, CuEventLoopService *>::const_iterator it = m_lis_map.begin(); it != m_lis_map.end(); ++it) {
        it->first->onTimeout(this);
    }
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
void CuTimer::addListener(CuTimerListener *l, CuEventLoopService *ls) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_lis_map[l] = ls;
    if(ls)
        ls->addCuEventLoopListener(this);
}

/*!
 * \brief CuTimer::removeListener removes a listener from the timer
 * \param l the listener to remove
 *
 * This method can be accessed from several different threads
 */
void CuTimer::removeListener(CuTimerListener *l) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_lis_map.erase(l);
}

/*!
 * \brief CuTimer::listeners returns the list of registered listeners
 *
 * @return the list of registered listeners
 *
 * This method can be accessed from several different threads
 */
std::map<CuTimerListener *, CuEventLoopService *> CuTimer::listenersMap()  {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_lis_map;
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
            if(/*status == std::cv_status::timeout && */!m_quit && !m_pause) {
                for(std::map<CuTimerListener *, CuEventLoopService *>::const_iterator it = m_lis_map.begin(); it != m_lis_map.end(); ++it) {
                    it->second != nullptr ?
                                it->second->postEvent(this, new CuTimerEvent())
                              : it->first->onTimeout(this);
                }
            }
            m_pending = 0;
            if(!m_quit) { // wait for next start()
                m_wait.wait(lock);
            }
        } // !m_skip
    }
}

