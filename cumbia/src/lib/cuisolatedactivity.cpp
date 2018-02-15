#include "cuisolatedactivity.h"
#include "cuactivitymanager.h"
#include "cumacros.h"

/*! @private */
class CuIsolatedActivityPrivate
{
public:
};

/*! \brief class constructor. Takes optional arguments and sets up the isolated activity
 *
 * @param token a CuData  that will describe this activity (default: empty CuData)
 * @param activityManager a pointer to the CuActivityManager (default: NULL and usually set later
 *        by Cumbia::registerActivity)
 *
 * \par activity flags
 *
 * The following flags are set to true: CuActivity::CuADeleteOnExit and
 * CuActivity::CuAUnregisterAfterExec.
 * Call CuActivity::setFlags to change the default behaviour.
 *
 */
CuIsolatedActivity::CuIsolatedActivity(const CuData& token, CuActivityManager *activityManager)
    : CuActivity(activityManager, token)
{
    d = new CuIsolatedActivityPrivate;
    setFlags(CuActivity::CuADeleteOnExit | CuActivity::CuAUnregisterAfterExec);
}

/*! \brief the class destructor
 *
 * nothing to report
 */
CuIsolatedActivity::~CuIsolatedActivity()
{
    pdelete("~CuIsolatedActivity %p", this);
    delete d;
}

/*! \brief returns the value of the CuAUnregisterAfterExec flag (true or false)
 *
 * @return true if the CuAUnregisterAfterExec flag is set, false otherwise
 *
 * @see deleteOnExit
 */
bool CuIsolatedActivity::unregisterOnExit() const
{
    return getFlags() & CuAUnregisterAfterExec;
}

/*! \brief returns the value of the CuADeleteOnExit flag (true or false)
 *
 * @return true if the CuADeleteOnExit flag is set, false otherwise
 *
 * If this method returns true, the activity is automatically deleted when
 * it finishes.
 *
 * @see unregisterOnExit
 */
bool CuIsolatedActivity::deleteOnExit() const
{
    return getFlags() & CuADeleteOnExit;
}

/*! \brief returns the value that identifies an *isolated activity*
 *
 * @return the CuActivity::Isolated value from the CuActivity::ActivityType enum
 *
 */
int CuIsolatedActivity::getType() const
{
    return CuActivity::Isolated;
}

/*! \brief returns -1 to indicate that CuActivity::execute is called only once
 *
 * @return -1 to indicate that CuActivity::execute is called only once
 *
 * @see CuContinuousActivity
 */
int CuIsolatedActivity::repeat() const
{
    return -1;
}
