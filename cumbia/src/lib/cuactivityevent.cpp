#include "cuactivityevent.h"

CuActivityEvent::Type CuPauseEvent::getType() const { return CuActivityEvent::Pause; }

CuActivityEvent::Type CuResumeEvent::getType() const
{
    return CuActivityEvent::Resume;
}

CuTimeoutChangeEvent::CuTimeoutChangeEvent(int timeout)
{
    m_timeout = timeout;
}

CuActivityEvent::Type CuTimeoutChangeEvent::getType() const
{
    return CuActivityEvent::TimeoutChange;
}

int CuTimeoutChangeEvent::getTimeout() const
{
    return m_timeout;
}

CuActivityEvent::~CuActivityEvent()
{

}

CuActivityEvent::Type CuExecuteEvent::getType() const
{
    return CuActivityEvent::Execute;
}
