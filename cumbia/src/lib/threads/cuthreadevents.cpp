#include "cuthreadevents.h"

RegisterActivityEvent::RegisterActivityEvent(CuActivity *a) : ThreadEvent()
{
    activity = a;
}

ThreadEvent::Type RegisterActivityEvent::getType() const
{
    return ThreadEvent::RegisterActivity;
}

UnRegisterActivityEvent::UnRegisterActivityEvent(CuActivity *a)
{
    activity = a;
}

ThreadEvent::Type UnRegisterActivityEvent::getType() const
{
    return ThreadEvent::UnregisterActivity;
}

ExitThreadEvent::ExitThreadEvent(bool autodes)
{
    autodestroy = autodes;
}

ThreadEvent::Type ExitThreadEvent::getType() const
{
    return ThreadExit;
}

CuTimerEvent::CuTimerEvent(int timeout)
{
    m_timeout = timeout;
}

CuTimerEvent::CuTimerEvent(CuActivity *a, CuTimer *t)
{
    activity = a;
    m_sender = t;
}

ThreadEvent::Type CuTimerEvent::getType() const
{
    return TimerExpired;
}

CuTimer *CuTimerEvent::getTimer() const
{
    return m_sender;
}

int CuTimerEvent::getTimeout() const
{
    return m_timeout;
}

CuPostEventToActivity::CuPostEventToActivity(CuActivity *activity, CuActivityEvent *e)
{
    mEvent = e;
    mActivity = activity;
}

CuPostEventToActivity::~CuPostEventToActivity()
{

}

ThreadEvent::Type CuPostEventToActivity::getType() const
{
    return ThreadEvent::PostEventToActivity;
}

CuActivity *CuPostEventToActivity::getActivity() const
{
    return mActivity;
}

CuActivityEvent *CuPostEventToActivity::getEvent() const
{
    return mEvent;
}
