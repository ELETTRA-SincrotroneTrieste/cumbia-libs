#ifndef CUPOLLINGACTIVITY_H
#define CUPOLLINGACTIVITY_H

#include <cucontinuousactivity.h>
#include <list>
#include <cadef.h>

class CuData;
class CuMonitorActivityPrivate;
class CuEpCAService;

class CuMonitorActivity : public CuContinuousActivity
{
public:
    enum Type { CuMonitorActivityType = CuActivity::User + 16 };

    CuMonitorActivity(const CuData& token, CuEpCAService *ep_s, const CuVariant &argins =
            std::vector<std::string>());

    ~CuMonitorActivity();

    void setArgins(const CuVariant &argins);

    static void event_handler_cb(evargs args);

    static void connection_handler_cb(connection_handler_args args);

    static void exception_handler_cb ( struct exception_handler_args excargs );

    void event_handler(evargs args);

    void connection_handler(connection_handler_args args);

    void exception_handler ( struct exception_handler_args excargs );

    // CuActivity interface
public:
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuMonitorActivityPrivate *d;

    void m_setTokenError(const char *msg, CuData& d);

    // CuActivity interface
public:
    void event(CuActivityEvent *e);

    int getType() const;

    int repeat() const;
};

// int (CuMonitorActivity::*connection_handler(connection_handler_args args)) = NULL;
// void (CuMonitorActivity::*event_handler(evargs args)) = NULL;

#endif // CUPOLLINGACTIVITY_H
