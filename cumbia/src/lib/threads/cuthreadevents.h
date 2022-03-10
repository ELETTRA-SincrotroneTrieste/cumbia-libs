#ifndef CUEVENT_INTERFACE_H
#define CUEVENT_INTERFACE_H

#include <cuvariant.h>
#include <cudata.h>

class CuActivity;
class CuActivityEvent;
class CuTimer;

/*! @private */
class ThreadEvent
{
public:

    enum Type { RegisterActivity,
                UnregisterActivity,
                ZeroActivities,
                ThreadExit,
                TimerExpired,
                PostToActivity,
                DisposeActivity,
                ThreadAutoDestroy };

    ThreadEvent() {}

    virtual ~ThreadEvent() {}

    virtual ThreadEvent::Type getType() const = 0;

private:
};

/*! @private */ // will not auto destroy
class CuThreadExitEv : public ThreadEvent {
public:
    ThreadEvent::Type getType() const;
};

/*! @private */ // zero activities left - will auto destroy
class CuThZeroA_Ev  : public ThreadEvent{
public:
    ThreadEvent::Type getType() const;
};

/*! @private */
class CuThRun_Ev : public ThreadEvent
{
public:
    CuThRun_Ev(CuActivity* activity, CuActivityEvent *event);
    virtual ~CuThRun_Ev();

    CuActivity *getActivity() const;
    CuActivityEvent *getEvent() const;

private:
    CuActivity *mActivity;
    CuActivityEvent *mEvent;

    // ThreadEvent interface
public:
    ThreadEvent::Type getType() const;
};

/*! @private */
class CuThRegisterA_Ev : public ThreadEvent {
public:
    CuThRegisterA_Ev(CuActivity *a);
    CuActivity *activity;
    virtual ~CuThRegisterA_Ev() {}
    // ThreadEvent interface
public:
    ThreadEvent::Type getType() const;
};

/*! @private */
class CuThUnregisterA_Ev : public ThreadEvent {
public:
    CuThUnregisterA_Ev(CuActivity *a);
    virtual ~CuThUnregisterA_Ev() {}
    CuActivity *activity;
    // ThreadEvent interface
public:
    ThreadEvent::Type getType() const;
};

/*! @private */
class CuThreadTimer_Ev : public ThreadEvent {
public:
    CuThreadTimer_Ev(CuTimer *t);
    ThreadEvent::Type getType() const;
    CuTimer* getTimer() const;
    int getTimeout() const;

private:
    CuTimer* m_sender;
    int m_timeout;
};

#endif // CUEVENT_INTERFACE_H
