#ifndef CUISOLATEDACTIVITY_H
#define CUISOLATEDACTIVITY_H

#include <cuactivity.h>

class CuIsolatedActivityPrivate;

/*! \brief A convenient CuActivity designed to be executed only once
 *
 * This activity enables the flags CuActivity::CuADeleteOnExit and CuActivity::CuAUnregisterAfterExec
 * and the CuIsolatedActivity::repeat method returns -1 in order to be executed once,
 * automatically unregistered and disposed after its execution.
 *
 * Two convenience methods  unregisterOnExit and deleteOnExit are a shortcut to get the values of the
 * aforementioned flags.
 *
 */
class CuIsolatedActivity : public CuActivity
{
public:

    CuIsolatedActivity(const CuData& token = CuData(), CuActivityManager *activityManager = NULL);

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
