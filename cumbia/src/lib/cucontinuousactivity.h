#ifndef CUCONTINUOUSACTIVITY_H
#define CUCONTINUOUSACTIVITY_H

#include <cuactivity.h>

class CuActivityManager;
class CuContinuousActivityPrivate;

/*! \brief an implementation of CuActivity that calls CuActivity::execute repeatedly at a
 *         certain interval of time expressed in milliseconds.
 *
 * This is a convenience class to implement a *poller* that executes code in a separate
 * thread with a period expressed in milliseconds.
 * setInterval can be used to change the period, also at runtime.
 *
 * The CuContinuousActivity::repeat method can be used (i.e. reimplemented) to suspend the
 * call to the CuActivity::execute method. The default implementation returns the period
 * (see setInterval, getTimeout) or zero if a CuActivityEvent::Pause event has been received
 * (see CuContinuousActivity::event)
 *
 * The cumbia-tango module defines an implementation of CuContinuousActivity named CuPollingActivity
 * that reads Tango attributes or commands periodically.
 */
class CuContinuousActivity : public CuActivity
{
public:
    CuContinuousActivity(const CuData& token = CuData(), int interval = 1000, CuActivityManager *activityManager = NULL);

    virtual ~CuContinuousActivity();

    void setInterval(int millis);

    int getTimeout() const;

    int repeat() const;

    // CuActivity interface
public:
    int getType() const;

private:
    CuContinuousActivityPrivate *d;

    // CuActivity interface
public:
    void event(CuActivityEvent *e);

};

#endif // CUCONTINUOUSACTIVITY_H
