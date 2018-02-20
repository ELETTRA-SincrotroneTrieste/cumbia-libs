#include "cueventloop.h"
#include "cuevent.h"

#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <assert.h>
#include <cumacros.h>

/*! @private */
class CuEventLoopPrivate
{
public:
    std::queue<CuEventI *> m_eventQueue;
    std::mutex m_mutex;
    std::condition_variable m_evloop_cv;
    CuEventLoopListener *eventLoopListener;
    std::thread *thread;
};

/*! The class constructor
 *
 * @param l a CuEventLoopListener
 */
CuEventLoopService::CuEventLoopService(CuEventLoopListener *l)
{
    d = new CuEventLoopPrivate;
    d->thread = NULL;
}

/*! \brief the class destructor
 *
 * \par Note
 * CuEventLoopService must not be destroyed while its thread is still running.
 * Instead, CuEventLoopService::exit and CuEventLoopService::wait must be called.
 */
CuEventLoopService::~CuEventLoopService()
{
    if(d->thread)
        perr("~CuEventLoopService: destroyed while thread still running. Please call exit() and wait()");
    delete d;
}

/*! \brief executes the event loop
 *
 * @param threaded true if the CuEventLoopService::run method has to be run in a separate
 *        thread, false if CuEventLoopService::run has to be run in the caller's same
 *        thread.
 */
void CuEventLoopService::exec(bool threaded)
{
    if(threaded)
    {
        pr_thread();
        pblue("CuEventLoop.exec: will run the event loop in a separate thread from this: 0x%lx...", pthread_self());
        if(!d->thread)
            d->thread = new std::thread(&CuEventLoopService::run, this);
        else
            perr("CuEventLoopService::exec: event loop in a separate thread already running");
    }
    else
        run();
}

/*! \brief post an event to the internal thread event queue
 *
 * This class is used internally.
 *
 * @param e the event to be delivered
 */
void CuEventLoopService::postEvent(CuEventI *e)
{
    std::unique_lock<std::mutex> lk(d->m_mutex);
    d->m_eventQueue.push(e);
    d->m_evloop_cv.notify_one();
}

/*! \brief set the CuEventLoopListener that will receive events from this service
 *
 * @param l a CuEventLoopListener that will receive events from the event loop
 */
void CuEventLoopService::setCuEventLoopListener(CuEventLoopListener *l)
{
    d->eventLoopListener = l;
}

/*! \brief exit the event loop cleanly
 *
 * an appropriate event is queued to the event queue of the event loop service.
 * As soon as it is received, the event loop exits.
 *
 * \par Note
 * call CuEventLoop::wait to wait for the secondary thread (if CuEventLoopService::exec
 * was called with a true parameter) to leave the event loop
 */
void CuEventLoopService::exit()
{
    std::unique_lock<std::mutex> lk(d->m_mutex);
    d->m_eventQueue.push(new CuExitLoopEvent);
    d->m_evloop_cv.notify_one();
}

/*! \brief wait for the secondary thread  to leave the event loop
 *
 * Wait for the secondary thread  to leave the event loop ( if CuEventLoopService::exec
 * was called with a true parameter)
 */
void CuEventLoopService::wait()
{
    if(d->thread)
    {
        pbgreen2("joining...");
        d->thread->join();
        pbgreen2("joined\n");
        delete d->thread;
        d->thread = NULL;
    }
}

/*! \brief run the event loop waiting for events on a queue
 *
 * Runs the event loop in the same caller's thread or in a background one,
 * according to how CuEventLoopService::exec was called
 */
void CuEventLoopService::run()
{
    pblue("CuEventLoop run: entering loop: this thread: \e[1;31m0x%lx\e[0m", pthread_self());
    bool repeat = true;
    while (repeat)
    {
        CuEventI* event = 0;
        {
            cuprintf("\e[1;32mthread \e[1;31m0x%lx\e[0m: waiting for events...\e[0m\n", pthread_self());

            // Wait for a message to be added to the queue
            {
                std::unique_lock<std::mutex> lk(d->m_mutex);
                while (d->m_eventQueue.empty())
                    d->m_evloop_cv.wait(lk);

                if (d->m_eventQueue.empty())
                {
                    cuprintf("\e[1;31mevent queue is emtpyyyy\e[0m\n");
                    continue;
                }
                event = d->m_eventQueue.front();
                d->m_eventQueue.pop();
            }

            if(d->eventLoopListener)
                d->eventLoopListener->onEvent(event);

            if(event->getType() == CuEventI::ExitLoop)
                repeat = false; /* leave loop */

            delete event;
        }
    }
    pblue("\e[1;32mCuEventLoopService.run leaving loop!\e[0m\n");
}

std::string CuEventLoopService::getName() const
{
    return "CuEventLoopService";
}

CuServices::Type CuEventLoopService::getType() const
{
    return CuServices::EventLoop;
}

