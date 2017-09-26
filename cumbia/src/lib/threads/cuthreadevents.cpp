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

ThreadEvent::Type ExitThreadEvent::getType() const
{
    return ThreadExit;
}

CuTimerEvent::CuTimerEvent(CuActivity* a, CuTimer *sender)
{
    m_sender = sender;
    activity = a;
}

ThreadEvent::Type CuTimerEvent::getType() const
{
    return TimerExpired;
}

CuTimer *CuTimerEvent::getTimer() const
{
    return m_sender;
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
