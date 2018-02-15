#ifndef GETTDBPROPACTIVITY_H
#define GETTDBPROPACTIVITY_H

#include <cuisolatedactivity.h>
#include <list>

class CuGetTDbPropActivityPrivate;

/*! \brief an activity to get properties from the Tango database
 *
 * This activity implements CuIsolatedActivity to fetch Tango properties from the
 * database. It's designed to be executed once (CuIsolatedActivity).
 *
 */
class CuGetTDbPropActivity : public CuIsolatedActivity
{
public:
    CuGetTDbPropActivity(const std::list<CuData> &in_data);

    virtual ~CuGetTDbPropActivity();

    // CuActivity interface
public:
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuGetTDbPropActivityPrivate *d;
};

#endif // GETTDBPROPACTIVITY_H
