#include "cugettdbpropactivity.h"
#include <cutango-world.h>
#include <cumacros.h> // for print

class CuGetTDbPropActivityPrivate
{
public:
    std::vector<CuData> in_data;
};

/*! \brief the class constructor, that accepts a list of CuData defining the properties
 *         the client wants to fetch from the Tango database
 *
 * @param in_data a std::list of CuData describing the names and the types of properties
 *        that have to be read from the database.
 *
 * \par activity flags
 * The activity sets to true the following two flags: CuActivity::CuAUnregisterAfterExec
 * and CuActivity::CuADeleteOnExit to automatically get rid of the activity once its task
 * is accomplished.
 */
CuGetTDbPropActivity::CuGetTDbPropActivity(const std::vector<CuData> &in_data)
{
    d = new CuGetTDbPropActivityPrivate;
    d->in_data = in_data;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
}

/*! \brief the class destructor
 *
 * class destructor
 */
CuGetTDbPropActivity::~CuGetTDbPropActivity()
{
    pdelete("~CuGetTDbPropActivity %p", this);
    delete d;
}

/** \brief Receive events *from the main thread to the CuActivity thread*.
 *
 * @param e the event. Do not delete e after use. Cumbia will delete it after this method invocation.
 *
 * @see CuActivity::event
 *
 * \note the body of this function is void
 *
 * @implements CuActivity::event
 */
void CuGetTDbPropActivity::event(CuActivityEvent *e)
{

}

/*! \brief returns true if the passed token is the same as this activity's token
 *
 * @param token a CuData containg key/value pairs of another activity's token
 * @return true if the input token is exactly the same as this activity's token
 *
 * @implements CuActivity::matches
 */
bool CuGetTDbPropActivity::matches(const CuData &token) const
{
    return getToken() == token;
}

/*! \brief implements the CuActivity::init method
 *
 * \note
 * The body of this function is empty.
 *
 * Database property access is
 * set up and executed within the CuGetTDbPropActivity::execute method.
 *
 * @implements CuActivity::init
 *
 */
void CuGetTDbPropActivity::init()
{
}

/*! \brief get Tango database properties and post the result on the main thread.
 *
 * Implements the CuActivity::execute hook
 *
 * Property fetch is delegated to CuTangoWorld::get_properties
 * publishResult delivers data to the main thread.
 *
 * @implements CuActivity::execute
 */
void CuGetTDbPropActivity::execute()
{
    CuTangoWorld tw;
    CuData res;
    tw.get_properties(d->in_data, res);
    publishResult(res);
}

/*! \brief implements the onExit method from CuActivity
 *
 * \note
 * this method is currently empty. See CuGetTDbPropActivity::execute
 *
 * @implements CuActivity::onExit
 */
void CuGetTDbPropActivity::onExit()
{

}
