#ifndef CUWRITEACTIVITY_H
#define CUWRITEACTIVITY_H

#include <cuisolatedactivity.h>

class CuEpCAService;
class CuWriteActivityPrivate;

class CuWriteActivity : public CuIsolatedActivity
{
public:
    CuWriteActivity(const CuData &token,
                    CuEpCAService *df);

    virtual ~CuWriteActivity();

    // CuActivity interface
public:
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuWriteActivityPrivate *d;
};

#endif // CUWRITEACTIVITY_H
