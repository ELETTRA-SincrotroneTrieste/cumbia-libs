#ifndef CUTADBACTIVITY_H
#define CUTADBACTIVITY_H

#include <cuisolatedactivity.h>

class TSource;
class CuTaDbActivityPrivate;

/*!
 * \brief Activity to get data from the Tango database
 */
class CuTaDbActivity : public CuActivity
{
public:
    enum Type { CuTaDbActivityTypeRead = CuActivity::User + 18, CuTaDbActivityTypeWrite };
    enum OpType { OpNoop = 0, OpGetDoma, OpGetFam, OpGetMem, OpGetAtts, OpGetCmds, OpGetAProp, OpGetCmdI, EndOpts };

    const char *opnames[EndOpts + 1] = { "OpNoop", "OpGetDoma", "OpGetFam", "OpGetMem",
                                     "OpGetAtts", "OpGetCmds", "OpGetAProp", "OpGetCmdI", "EndOpts" };

    CuTaDbActivity(const TSource& tsrc, const CuData& options, const CuData& tag);

    virtual ~CuTaDbActivity();

    void setOptions(const CuData& o);

    // CuActivity interface
public:
    int getType() const;
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;
    int repeat() const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuTaDbActivityPrivate *d;
};

#endif // CUTADBACTIVITY_H
