#ifndef CUCONTINUOUSACTIVITY_H
#define CUCONTINUOUSACTIVITY_H

#include <cuactivity.h>

class CuActivityManager;
class CuContinuousActivityPrivate;

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
