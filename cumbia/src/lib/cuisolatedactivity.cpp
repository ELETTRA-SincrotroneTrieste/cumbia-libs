#include "cuisolatedactivity.h"
#include "cuactivitymanager.h"
#include "cumacros.h"

class CuIsolatedActivityPrivate
{
public:
};

CuIsolatedActivity::CuIsolatedActivity(const CuData& token, CuActivityManager *activityManager, int f)  : CuActivity(activityManager, token)
{
    d = new CuIsolatedActivityPrivate;
    setFlags(CuActivity::CuADeleteOnExit | CuActivity::CuAUnregisterAfterExec);
}

CuIsolatedActivity::~CuIsolatedActivity()
{
    pdelete("~CuIsolatedActivity %p", this);
    delete d;
}

bool CuIsolatedActivity::unregisterOnExit() const
{
    return getFlags() & CuAUnregisterAfterExec;
}

bool CuIsolatedActivity::deleteOnExit() const
{
    return getFlags() & CuADeleteOnExit;
}

int CuIsolatedActivity::getType() const
{
    return CuActivity::Isolated;
}

int CuIsolatedActivity::repeat() const
{
    return -1;
}
