#ifndef CUEVENT_H
#define CUEVENT_H

#include "cudata.h"
class CuActivity;

class CuEventI
{
public:
    virtual ~CuEventI() {}

    enum CuEventType { Progress = 0, Result, CuActivityExitEvent, ExitLoop, User = 100 };

    virtual CuEventType getType() const = 0;

};

/** \brief This event is used by CuEventLoop service
 */
class CuExitLoopEvent : public CuEventI
{
    // CuEventI interface
public:
    CuEventType getType() const { return CuEventI::ExitLoop; }
};

class CuResultEventPrivate
{
public:
    CuResultEventPrivate(const CuData &d) : data(d)
    {
        step = total = 0;
        type = CuEventI::Result;
    }

    CuEventI::CuEventType type;
    const CuData data;
    const CuActivity *activity;
    int step, total;
};

class CuResultEvent : public CuEventI
{
public:
    CuResultEvent(const CuActivity* sender, const CuData &data, CuEventType t = Result);

    CuResultEvent(const CuActivity* sender, int step, int total, const CuData &data);

    virtual ~CuResultEvent();

    // CuResultEventI interface
public:
    void setType(CuEventType t);
    CuEventType getType() const;
    int getStep() const;
    int getTotal() const;
    const CuData getData() const;
    const CuActivity *getActivity() const;

protected:
    CuResultEventPrivate *d_p;
};

class CuActivityExitEvent : public CuEventI
{
public:
    CuActivityExitEvent(CuActivity *sender);

    CuEventType getType() const;

    CuActivity *getActivity() const;

private:
    CuActivity *m_activity;

};

#endif // CUELOOPEVENT_H
