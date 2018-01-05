#include "cueventloop.h"
#include "cuevent.h"

#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <assert.h>
#include <cumacros.h>

class CuEventLoopPrivate
{
public:
    std::queue<CuEventI *> m_eventQueue;
    std::mutex m_mutex;
    std::condition_variable m_evloop_cv;
    CuEventLoopListener *eventLoopListener;
    std::thread *thread;
};

CuEventLoopService::CuEventLoopService(CuEventLoopListener *l)
{
    d = new CuEventLoopPrivate;
    d->thread = NULL;
}

CuEventLoopService::~CuEventLoopService()
{
    if(d->thread)
        perr("~CuEventLoopService: destroyed while thread already running. Please call exit() and wait()");
    delete d;
}

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

void CuEventLoopService::postEvent(CuEventI *e)
{
    std::unique_lock<std::mutex> lk(d->m_mutex);
    d->m_eventQueue.push(e);
    d->m_evloop_cv.notify_one();
}

void CuEventLoopService::setCuEventLoopListener(CuEventLoopListener *l)
{
    d->eventLoopListener = l;
}

void CuEventLoopService::exit()
{
    std::unique_lock<std::mutex> lk(d->m_mutex);
    d->m_eventQueue.push(new CuExitLoopEvent);
    printf("pushed in queue, notifying now! [exit]\n");
    d->m_evloop_cv.notify_one();
}

void CuEventLoopService::wait()
{
    if(d->thread)
    {
        pbgreen2("JUANZANGGGG_G_G_G");
        d->thread->join();
        pbgreen2("JUANZAZZZZZ\n");
        delete d->thread;
        d->thread = NULL;
    }
}

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
            {
                printf("\e[1;32mcall onEvent on eventLoopListener\e[0m\n");
                d->eventLoopListener->onEvent(event);
            }
            printf("\e[1;32m AFTER call onEvent on eventLoopListener\e[0m\n");
            if(event->getType() == CuEventI::ExitLoop)
                repeat = false; /* leave loop */

            printf("\e[1;32mCuEventLoopService.run: repeat is %d\e[0m\n", repeat);
            delete event;
        }
    }
    printf("\e[1;32mCuEventLoopService.run leaving loop!\e[0m\n");
}

std::string CuEventLoopService::getName() const
{
    return "CuEventLoopService";
}

CuServices::Type CuEventLoopService::getType() const
{
    return CuServices::EventLoop;
}

