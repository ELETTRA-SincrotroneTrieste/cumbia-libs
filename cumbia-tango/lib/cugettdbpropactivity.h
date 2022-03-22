#ifndef GETTDBPROPACTIVITY_H
#define GETTDBPROPACTIVITY_H

#include <cuactivity.h>
#include <vector>

class CuGetTDbPropActivityPrivate;

/*! \brief an activity to get properties from the Tango database
 *
 * This activity implements CuIsolatedActivity to fetch Tango properties from the
 * database. It's designed to be executed once (CuIsolatedActivity).
 *
 */
class CuGetTDbPropActivity : public CuActivity
{
public:
    enum Type { CuGetTDbPropA_Type = CuActivity::UserAType + 22 };
    CuGetTDbPropActivity(const std::vector<CuData> &in_data);

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

    // CuActivity interface
public:
    int getType() const;
    int repeat() const;
};


#endif // GETTDBPROPACTIVITY_H
