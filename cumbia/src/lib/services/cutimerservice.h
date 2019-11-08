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
    CuTimer* registerListener(CuTimerListener *th, int timeout);
    void unregisterListener(CuTimerListener *th);
    void changeTimeout(CuTimerListener *th, int timeout);
    bool isRegistered(CuTimerListener *th, int timeout);

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    void m_stopAll();
    CuTimer *m_getTimer(const CuTimerListener *th);
    std::list<CuTimerListener *>m_getTimerListeners(int timeout);

    CuTimerServicePrivate *d;
};

#endif // CUTIMERSERVICE_H
