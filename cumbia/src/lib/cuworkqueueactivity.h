#ifndef CUWORKQUEUEACTIVITY_H
#define CUWORKQUEUEACTIVITY_H

#include <cuactivity.h>
#include <list>

class CuWorkQueueActivityPrivate;

class CuWorkQueueAEvent {
public:
    enum Type { CuWQActivityDefaultEventType = 1 };

    CuWorkQueueAEvent();
    CuWorkQueueAEvent(const CuData& userdata);
    virtual ~CuWorkQueueAEvent() {}

    virtual int type() const;

    CuData user_data;
};

class CuWorkQueueA_Worker {
public:
    virtual ~CuWorkQueueA_Worker() {};

    virtual void execute(const std::list<CuWorkQueueAEvent *>& event_q) = 0;
    virtual void init() {};
    virtual void exit() {};

    virtual void onResult(const CuData& da) { (void) da; }
    virtual void onResult(const std::vector<CuData> &dl) { (void) dl; }
};

class CuWorkQueueActivity : public CuActivity
{
public:
    CuWorkQueueActivity(CuWorkQueueA_Worker *worker);
    virtual ~CuWorkQueueActivity();

    void new_work_event(CuWorkQueueAEvent *e);
    void publish_result(const CuData& da);

    void stop();

    // CuActivity interface
public:
    int getType() const;
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;
    int repeat() const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuWorkQueueActivityPrivate *d;
};

#endif // CUWORKQUEUEACTIVITY_H
