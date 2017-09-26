#ifndef CUEVENT_INTERFACE_H
#define CUEVENT_INTERFACE_H

#include <cuvariant.h>
#include <cudata.h>

class CuActivity;
class CuActivityEvent;
class CuTimer;

/*
 * ///////////////////////////////////////

 * The following do not derive from QEvent
 *
 * ///////////////////////////////////////
 */

class ThreadEvent
{
  public:

    enum Type { RegisterActivity, UnregisterActivity, ThreadExit, TimerExpired, PostEventToActivity, DisposeActivity };

    ThreadEvent() {}

    virtual ~ThreadEvent() {}

    virtual ThreadEvent::Type getType() const = 0;

private:
};

class ExitThreadEvent : public ThreadEvent
{
public:
    ThreadEvent::Type getType() const;
};

class CuPostEventToActivity : public ThreadEvent
{
public:

    CuPostEventToActivity(CuActivity* activity, CuActivityEvent *event);

    virtual ~CuPostEventToActivity();

    CuActivity *getActivity() const;

    CuActivityEvent *getEvent() const;

private:
    CuActivity *mActivity;
    CuActivityEvent *mEvent;

    // ThreadEvent interface
public:
    ThreadEvent::Type getType() const;
};

class RegisterActivityEvent : public ThreadEvent
{
public:

    RegisterActivityEvent(CuActivity *a);

    CuActivity *activity;

    virtual ~RegisterActivityEvent() {}

    // ThreadEvent interface
public:
    ThreadEvent::Type getType() const;
};

class UnRegisterActivityEvent : public ThreadEvent
{
public:

    UnRegisterActivityEvent(CuActivity *a);

    virtual ~UnRegisterActivityEvent() {}

    CuActivity *activity;


    // ThreadEvent interface
public:
    ThreadEvent::Type getType() const;
};

class CuTimerEvent : public ThreadEvent
{
public:
    CuTimerEvent(CuActivity *a, CuTimer * sender);

    ThreadEvent::Type getType() const;

    CuTimer* getTimer() const;

    CuActivity *activity;

private:
    CuTimer* m_sender;
};


#endif // CUEVENT_INTERFACE_H
