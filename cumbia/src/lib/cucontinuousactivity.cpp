#include "cucontinuousactivity.h"
#include "cuactivityevent.h"
#include "cumacros.h"

/*! @private */
class CuContinuousActivityPrivate
{
public:
    int timeout;
    bool pause;
};

/*! \brief the class constructor. Initializes a continuous activity with an interval in milliseconds.
 *
 * @param token a CuData used to characterise the activity. It is used by the CuActivity::matches
 *        method to match *similar* activities. The meaning of *similar* is determined by the
 *        nature of the activity and within the CuActivity::matches method.
 *        (Default: empty CuData)
 * @param interval an integer, in milliseconds, that determines the time between two
 *        subsequent CuActivity::execute calls (default: 1000)
 * @param activityManager a pointer to a CuActivityManager (default: NULL and usually set later
 *        by Cumbia::registerActivity)
 *
 * The constructor sets the activity flags to *CuADeleteOnExit | CuAUnregisterAfterExec*, so that
 * the activity is deleted after it finishes and automatically unregister after the execution.
 * The flags can be changed with CuActivity:setFlags or CuActivity::setFlags
 *
 * The interval can be changed with setInterval
 */
CuContinuousActivity::CuContinuousActivity(const CuData &token, int interval, CuActivityManager *activityManager)
    : CuActivity(token)
{
    d = new CuContinuousActivityPrivate();
    d->timeout = interval;
    d->pause = false;
    setFlags(CuADeleteOnExit | CuAUnregisterAfterExec);
}

/*! \brief the class destructor
 */
CuContinuousActivity::~CuContinuousActivity()
{
    delete d;
}

/*! \brief sets the time, in milliseconds, between two subsequent calls of the execute method
 *
 * @param millis the interval between two CuActivity::execute calls in a continuous activity
 *        (milliseconds)
 */
void CuContinuousActivity::setInterval(int millis)
{
    d->timeout = millis;
}

/*! \brief returns the time, in milliseconds, between two subsequent calls of the execute method
 *
 * @return the interval between two execute calls in a continuous activity (milliseconds)
 */
int CuContinuousActivity::getTimeout() const
{
    return d->timeout;
}

/*! \brief returns the interval of time, in milliseconds, before the next CuActivity::execute call,
 *         or zero or negative in order to not execute again
 *
 * @return an integer greater than zero representing the interval, in milliseconds, before next execution
 *         or zero (or negative) to prevent CuActivity::execute to be called again
 *
 * This implementation returns the value of CuContinuousActivity::getTimeout if the activity is not
 * paused, zero otherwise
 *
 * @implements CuActivity::repeat
 */
int CuContinuousActivity::repeat() const
{
    if(!d->pause)
        return d->timeout;
    return 0;
}

/*! \brief returns the activity type
 *
 * @return the activity type: CuActivity::Continuous
 *
 * @implements CuActivity::getType
 */
int CuContinuousActivity::getType() const
{
    return CuActivity::Continuous;
}

/*! \brief received in the CuActivity thread from the main thread, it can change the
 *         execution timeout, pause, resume and trigger an execution
 *
 * @param e an event implementing the CuActivityEvent interface
 *
 * Four kind of events are recognised and processed:
 * \li timeout change (CuActivityEvent::TimeoutChange)
 * \li pause the execution of the continuous activity (CuActivityEvent::Pause)
 * \li resume the execution of the activity (CuActivityEvent::Resume)
 * \li trigger an execution (CuEventActivity::Execute)
 *
 * @implements CuActivity::event
 *
 */
void CuContinuousActivity::event(CuActivityEvent *e)
{
    cuprintf("CuContinuousActivity.event type %d (pause %d resume %d exec %d timeout change %d)\n",
           e->getType(), CuActivityEvent::Pause, CuActivityEvent::Resume, CuActivityEvent::Execute,
           CuActivityEvent::TimeoutChange);
    if(e->getType() == CuActivityEvent::TimeoutChange)
        d->timeout = static_cast<CuTimeoutChangeEvent *>(e)->getTimeout();
    else if(e->getType() == CuActivityEvent::Pause)
        d->pause = true;
    else if(e->getType() == CuActivityEvent::Resume)
        d->pause = false;
    else if(e->getType() == CuActivityEvent::Execute)
        execute();
    else if(e->getType() == CuActivityEvent::Cu_Data) {
        CuDataEvent *de = static_cast<CuDataEvent *>(e);
        publishResult(de->getData());
    }
}
