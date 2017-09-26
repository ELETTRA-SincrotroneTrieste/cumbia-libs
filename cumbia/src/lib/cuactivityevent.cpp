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

CuActivityDataEvent::CuActivityDataEvent(const CuData &data) : CuActivityEvent(), m_data(data)
{

}

CuActivityEvent::Type CuActivityDataEvent::getType() const
{
    return CuActivityEvent::Data;
}

const CuData CuActivityDataEvent::getData() const
{
    return m_data;
}

CuActivityEvent::~CuActivityEvent()
{

}
