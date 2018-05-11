#ifndef CUWRITEACTIVITY_H
#define CUWRITEACTIVITY_H

#include <cuisolatedactivity.h>
#include <cadef.h>

class CuEpCAService;
class CuPutActivityPrivate;

class CuPutActivity : public CuIsolatedActivity
{
public:
    CuPutActivity(const CuData &token,
                    CuEpCAService *df);

    virtual ~CuPutActivity();

    static void exception_handler_cb ( struct exception_handler_args excargs );


    void exception_handler ( struct exception_handler_args excargs );


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
    void m_setTokenError(const char *msg, CuData &d);
};

#endif // CUWRITEACTIVITY_H
