#ifndef CUWRITEACTIVITY_H
#define CUWRITEACTIVITY_H

#include <cuisolatedactivity.h>

class CuEpCAService;
class CuPutActivityPrivate;

class CuPutActivity : public CuIsolatedActivity
{
public:
    CuPutActivity(const CuData &token,
                    CuEpCAService *df);

    virtual ~CuPutActivity();

    // CuActivity interface
public:
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuPutActivityPrivate *d;
};

#endif // CUWRITEACTIVITY_H
