#ifndef CURESULTEVENT_QT_H
#define CURESULTEVENT_QT_H

#include <QEvent>
#include "cuevent.h"

class CUData;

/*! @private
 *
 */
class CuEvent_Qt : public QEvent
{
public:

    CuEvent_Qt(CuEventI *re);

    virtual ~CuEvent_Qt();

    CuEventI *getEvent() const;

private:
    CuEventI *m_event;
};

#endif // CURESULTEVENT_QT_H
