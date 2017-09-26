#include "cuevent_qt.h"
#include "cudata.h"

CuEvent_Qt::CuEvent_Qt(CuEventI *re) : QEvent((QEvent::Type) (QEvent::User + 101))
{
       m_event = re;
}

CuEvent_Qt::~CuEvent_Qt()
{
    delete m_event;
}

CuEventI *CuEvent_Qt::getEvent() const
{
    return m_event;
}


