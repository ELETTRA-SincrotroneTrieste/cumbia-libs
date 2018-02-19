#ifndef CUACTIVITYEVENT_H
#define CUACTIVITYEVENT_H

#include <cudata.h>

/** \brief Generic event to pass data to an activity
 *
 */
class CuActivityEvent
{
public:
    enum Type { Pause = 0, Resume, TimeoutChange, Data, Execute, User = 100, MaxUser = 2048 };

    virtual ~CuActivityEvent();

    virtual Type getType() const = 0;
};

class CuPauseEvent : public CuActivityEvent
{
public:
    CuActivityEvent::Type getType() const;
};

class CuResumeEvent : public CuActivityEvent
{
public:
    CuActivityEvent::Type getType() const;
};

class CuExecuteEvent : public CuActivityEvent
{
public:
    CuActivityEvent::Type getType() const;
};

class CuTimeoutChangeEvent : public CuActivityEvent
{
public:
    CuTimeoutChangeEvent(int timeout);

    CuActivityEvent::Type getType() const;

    int getTimeout() const;

private:
    int m_timeout;
};


#endif // CUACTIVITYEVENT_H
