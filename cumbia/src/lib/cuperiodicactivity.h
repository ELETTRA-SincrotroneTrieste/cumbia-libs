#ifndef CUPERIODICACTIVITY_H
#define CUPERIODICACTIVITY_H

#include <cuactivity.h>

class CuActivityManager;
class CuPeriodicActivityPrivate;

/*! \brief an implementation of CuActivity that calls CuActivity::execute repeatedly at a
 *         certain interval of time expressed in milliseconds.
 *
 * This is a convenience class to implement a *poller* that executes code in a separate
 * thread with a period expressed in milliseconds.
 * setInterval can be used to change the period, also at runtime.
 *
 * The CuPeriodicActivity::repeat method can be used (i.e. reimplemented) to suspend the
 * call to the CuActivity::execute method. The default implementation returns the period
 * (see setInterval, getTimeout) or zero if a CuActivityEvent::Pause event has been received
 * (see CuPeriodicActivity::event)
 *
 * The cumbia-tango module defines an implementation of CuPeriodicActivity named CuPollingActivity
 * that reads Tango attributes or commands periodically.
 */
class CuPeriodicActivity : public CuActivity
{
public:
    CuPeriodicActivity(const CuData& token = CuData(), int interval = 1000, CuActivityManager *activityManager = NULL);

    virtual ~CuPeriodicActivity();

    void setInterval(int millis);
    int interval() const;
    int repeat() const;

    // CuActivity interface
public:
    int getType() const;

private:
    CuPeriodicActivityPrivate *d;

    // CuActivity interface
public:
    void event(CuActivityEvent *e);

};

#endif // CUPERIODICACTIVITY_H
