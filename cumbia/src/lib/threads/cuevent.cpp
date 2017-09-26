#include "cuevent.h"



CuResultEvent::CuResultEvent(const CuActivity* sender, const CuData &data, CuEventI::CuEventType t)
{
    d_p = new CuResultEventPrivate(data);
    d_p->type = t;
    d_p->activity = sender;
}

CuResultEvent::CuResultEvent(const CuActivity* sender, int step, int total, const CuData &data)
{
    d_p = new CuResultEventPrivate(data);
    d_p->step = step;
    d_p->total = total;
    d_p->type = Progress;
    d_p->activity = sender;
}

CuResultEvent::~CuResultEvent()
{
    delete d_p;
}

void CuResultEvent::setType(CuEventI::CuEventType t)
{
    d_p->type = t;
}

CuEventI::CuEventType CuResultEvent::getType() const
{
    return d_p->type;
}

int CuResultEvent::getStep() const
{
    return d_p->step;
}

int CuResultEvent::getTotal() const
{
    return d_p->total;
}

const CuData CuResultEvent::getData() const
{
    return d_p->data;
}

const CuActivity *CuResultEvent::getActivity() const
{
    return d_p->activity;
}

CuActivityExitEvent::CuActivityExitEvent(CuActivity *sender)
{
    m_activity = sender;
}

CuEventI::CuEventType CuActivityExitEvent::getType() const
{
    return CuEventI::CuActivityExitEvent;
}

CuActivity *CuActivityExitEvent::getActivity() const
{
    return m_activity;
}
