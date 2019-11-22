#ifndef CUTIMERSERVICE_H
#define CUTIMERSERVICE_H

#include <cuservicei.h>
#include <list>

class CuTimerServicePrivate;
class CuTimerListener;
class CuTimer;

class CuTimerService : public CuServiceI
{
public:
    CuTimerService();
    ~CuTimerService();
    CuTimer* registerListener(CuTimerListener *timer_listener, int timeout);
    std::list<CuTimer *> getTimers();
    void unregisterListener(CuTimerListener *th, int timeout);
    CuTimer *changeTimeout(CuTimerListener *th, int timeout);
    bool isRegistered(CuTimerListener *th, int timeout);
    void setTimerMaxCount(int count);
    int timerMaxCount() const;
    void restart(CuTimer *t, int millis);

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    void m_stopAll();
    void m_removeFromMaps(CuTimerListener *l, int timeout);
    CuTimer *m_findReusableTimer(int timeout);
    CuTimer *m_findTimer(const CuTimerListener *th, int timeout);

    CuTimerServicePrivate *d;
};

#endif // CUTIMERSERVICE_H
