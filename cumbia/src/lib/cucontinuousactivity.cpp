#include "cucontinuousactivity.h"
#include "cuactivityevent.h"
#include "cumacros.h"

class CuContinuousActivityPrivate
{
public:
    int timeout;
    bool pause;
};

CuContinuousActivity::CuContinuousActivity(const CuData &token, int interval, CuActivityManager *activityManager)
    : CuActivity(activityManager, token)
{
    d = new CuContinuousActivityPrivate();
    d->timeout = interval;
    d->pause = false;
    setFlags(CuADeleteOnExit | CuAUnregisterAfterExec);
}

CuContinuousActivity::~CuContinuousActivity()
{
    delete d;
}

void CuContinuousActivity::setInterval(int millis)
{
    d->timeout = millis;
}

int CuContinuousActivity::getTimeout() const
{
    return d->timeout;
}

int CuContinuousActivity::repeat() const
{
    cuprintf("\e[1;31mCuContinuousActivity.REPEAT: PAUSE %d interval %d ================= ==============================\e[0m\n",
             d->pause, d->timeout);
    if(!d->pause)
        return d->timeout;
    return 0;
}

int CuContinuousActivity::getType() const
{
    return CuActivity::Continuous;
}

void CuContinuousActivity::event(CuActivityEvent *e)
{
    cuprintf("CuContinuousActivity.event type %d\n", e->getType());
    if(e->getType() == CuActivityEvent::TimeoutChange)
        d->timeout = static_cast<CuTimeoutChangeEvent *>(e)->getTimeout();
    else if(e->getType() == CuActivityEvent::Pause)
        d->pause = true;
    else if(e->getType() == CuActivityEvent::Resume)
        d->pause = false;
    else if(e->getType() == CuActivityEvent::Execute)
        execute();
}
