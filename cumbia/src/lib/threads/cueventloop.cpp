#include "cueventloop.h"
#include "cuevent.h"

#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <assert.h>
#include <cumacros.h>
#include <set>
#include <algorithm> // for std::find

class CuEventInfo {
public:
    CuEventInfo(CuEventI *eve, CuEventLoopListener *l) :
        event(eve), lis(l) {}
    CuEventInfo() { event = nullptr; lis = nullptr; }

    CuEventI *event;
    CuEventLoopListener *lis;
};

/*! @private */
class CuEventLoopPrivate
{
public:
    std::queue<CuEventInfo> queue;
    std::mutex m_mutex;
    std::condition_variable m_evloop_cv;
    std::set<CuEventLoopListener* > eloo_liss;
    std::thread *thread;
    pthread_t mythread;
};

/*! The class constructor
 *
 * @param l a CuEventLoopListener
 */
CuEventLoopService::CuEventLoopService(CuEventLoopListener *l)
{
    d = new CuEventLoopPrivate;
    d->mythread = pthread_self();
    d->thread = nullptr;
    if(l) d->eloo_liss.insert(l);
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
void CuEventLoopService::exec() {
    run();
}

/*! \brief post an event to the internal thread event queue
 *
 * This class is used internally.
 *
 * @param e the event to be delivered
 */
void CuEventLoopService::postEvent(CuEventLoopListener *lis, CuEventI *e)
{
    std::unique_lock<std::mutex> lk(d->m_mutex);
    d->queue.push(CuEventInfo(e, lis));
    d->m_evloop_cv.notify_one();
}

/*! \brief set the CuEventLoopListener that will receive events from this service
 *
 * @param l a CuEventLoopListener that will receive events from the event loop
 */
void CuEventLoopService::addCuEventLoopListener(CuEventLoopListener *l) {
    assert(d->mythread == pthread_self());
    d->eloo_liss.insert(l);
}

void CuEventLoopService::removeCuEventLoopListener(CuEventLoopListener *l) {
    assert(d->mythread == pthread_self());
    d->eloo_liss.erase(l);
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
    cuprintf("CuEventLoopService::exit\n");
    std::unique_lock<std::mutex> lk(d->m_mutex);
    d->queue.push(CuEventInfo(new CuExitLoopEvent, nullptr));
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
    assert(d->mythread == pthread_self());
    bool repeat = true;
    while (repeat)  {
        std::queue<CuEventInfo> qcopy;
        // Wait for a message to be added to the queue
        {
            // lock as short as possible: just to copy d->queue into a local queue
            std::unique_lock<std::mutex> lk(d->m_mutex);
            while (d->queue.empty()) {
                d->m_evloop_cv.wait(lk);
            }
            if(d->queue.empty())
                continue;
            while(!d->queue.empty()) {
                qcopy.push(d->queue.front());
                d->queue.pop();
            }
        }
        // lock free section ensues
        while(!qcopy.empty()) {
            const CuEventInfo event_i = qcopy.front();
            if(event_i.event->getType() == CuEventI::CuExitLoopEv)
                repeat = false;
            else if(std::find(d->eloo_liss.begin(), d->eloo_liss.end(), event_i.lis)
                    != d->eloo_liss.end()) {
                event_i.lis->onEvent(event_i.event);
            }
            delete event_i.event;
            qcopy.pop();
        }
    }
}

std::string CuEventLoopService::getName() const
{
    return "CuEventLoopService";
}

CuServices::Type CuEventLoopService::getType() const
{
    return CuServices::EventLoop;
}

