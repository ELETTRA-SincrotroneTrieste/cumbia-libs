#ifndef CUISOLATEDACTIVITY_H
#define CUISOLATEDACTIVITY_H

#include <cuactivity.h>

class CuIsolatedActivityPrivate;

class CuIsolatedActivity : public CuActivity
{
public:

    CuIsolatedActivity(const CuData& token = CuData(), CuActivityManager *activityManager = NULL, int f = (CuADeleteOnExit | CuADeleteOnExit));

    ~CuIsolatedActivity();

    bool unregisterOnExit() const;

    bool deleteOnExit() const;

    int repeat() const;

    // CuActivity interface
public:
    int getType() const;

private:
    CuIsolatedActivityPrivate *d;
};

#endif // CUISOLATEDACTIVITY_H
