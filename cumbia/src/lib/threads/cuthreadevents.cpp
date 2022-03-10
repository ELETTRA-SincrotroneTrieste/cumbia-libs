#include "cuthreadevents.h"

CuThRegisterA_Ev::CuThRegisterA_Ev(CuActivity *a) : ThreadEvent()
{
    activity = a;
}

ThreadEvent::Type CuThRegisterA_Ev::getType() const
{
    return ThreadEvent::RegisterActivity;
}

CuThUnregisterA_Ev::CuThUnregisterA_Ev(CuActivity *a)
{
    activity = a;
}

ThreadEvent::Type CuThUnregisterA_Ev::getType() const
{
    return ThreadEvent::UnregisterActivity;
}

ThreadEvent::Type CuThZeroA_Ev::getType() const {
    return ThreadEvent::ZeroActivities;
}

ThreadEvent::Type CuThreadExitEv::getType() const {
    return ThreadExit;
}

CuThreadTimer_Ev::CuThreadTimer_Ev(CuTimer *t) {
    m_sender = t;
}

ThreadEvent::Type CuThreadTimer_Ev::getType() const
{
    return TimerExpired;
}

CuTimer *CuThreadTimer_Ev::getTimer() const
{
    return m_sender;
}

int CuThreadTimer_Ev::getTimeout() const
{
    return m_timeout;
}

CuThRun_Ev::CuThRun_Ev(CuActivity *activity, CuActivityEvent *e)
{
    mEvent = e;
    mActivity = activity;
}

CuThRun_Ev::~CuThRun_Ev()
{

}

ThreadEvent::Type CuThRun_Ev::getType() const
{
    return ThreadEvent::PostToActivity;
}

CuActivity *CuThRun_Ev::getActivity() const
{
    return mActivity;
}

CuActivityEvent *CuThRun_Ev::getEvent() const
{
    return mEvent;
}
