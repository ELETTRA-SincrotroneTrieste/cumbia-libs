#include "cuworkqueueactivity.h"
#include <queue>
#include <condition_variable>
#include <mutex>
#include <list>

CuWorkQueueAEvent::CuWorkQueueAEvent() {

}

CuWorkQueueAEvent::CuWorkQueueAEvent(const CuData &userdata) : user_data(userdata)  {

}

int CuWorkQueueAEvent::type() const {
    return CuWQActivityDefaultEventType;
}

class CuWorkQueueActivityPrivate {
public:
    CuWorkQueueActivityPrivate() : quit(false), init_f (nullptr), exit_f (nullptr), exec_f (nullptr),
        result_f(nullptr), resultlist_f(nullptr), worker(nullptr) {}

    std::queue <CuWorkQueueAEvent *> eq;
    std::mutex mu;
    std::condition_variable wait;
    bool quit;
    std::string msg;

    void (*init_f)();
    void (*exit_f)();
    void (*exec_f)(const std::list<CuWorkQueueAEvent *>&);

    void (*result_f)(const CuData&);
    void (*resultlist_f)(const std::vector<CuData> &);

    CuWorkQueueA_Worker *worker;
};

/*!
 * \brief initialise a work queue activity with the subclass worker model
 * \param worker an instance of CuWorkQueueA_Worker
 *
 * The CuWorkQueueA_Worker::execute method will be invoked in the CuWorkQueueActivity's thread
 * and the list of events in the queue will be passed
 */
CuWorkQueueActivity::CuWorkQueueActivity(CuWorkQueueA_Worker *worker) {
    d = new CuWorkQueueActivityPrivate;
    d->worker = worker;
}

CuWorkQueueActivity::~CuWorkQueueActivity() {
       delete d;
}

/*!
 * \brief enqueue a new work event
 * \param e an instance of CuWorkQueueAEvent that will be deleted by CuWorkQueueActivity
 *        after execution
 */
void CuWorkQueueActivity::new_work_event(CuWorkQueueAEvent *e) {
    std::unique_lock<std::mutex> lock(d->mu);
    d->eq.push(e);
    d->wait.notify_one();
}

void CuWorkQueueActivity::publish_result(const CuData &da) {
    publishResult(da);
}

/*!
 * \brief stop the activity
 *
 * Events may still be delivered from the activity thread before exiting the execution loop
 */
void CuWorkQueueActivity::stop() {
    d->quit = true;
    d->wait.notify_one();
}

int CuWorkQueueActivity::getType() const {
    return CuActivity::WorkQueue;
}

void CuWorkQueueActivity::event(CuActivityEvent *e) { }

bool CuWorkQueueActivity::matches(const CuData &token) const {
    return token == getToken();
}

int CuWorkQueueActivity::repeat() const {
    return -1;
}

void CuWorkQueueActivity::init() {
    if(d->init_f) d->init_f();
    if(d->worker) d->worker->init();
}

void CuWorkQueueActivity::execute()
{
    while(!d->quit) {
        std::list<CuWorkQueueAEvent *> events;
        {
            std::unique_lock<std::mutex> lock(d->mu);
            // copy messages locally while holding lock
            while(d->eq.empty() && !d->quit)
                d->wait.wait(lock);
            while(!d->eq.empty() && !d->quit) {
                events.emplace_back(std::move(d->eq.front()));
                d->eq.pop();
            }
        }// end locked section
        if(d->exec_f) // function pointer model
            d->exec_f(events);
        if(d->worker) // polymorphic model used
            d->worker->execute(events);
        for(CuWorkQueueAEvent * xd : events) {
            delete xd;
        }
    }
}

void CuWorkQueueActivity::onExit() {
    if(d->exit_f) d->exit_f();
    if(d->worker) d->worker->exit();
}
