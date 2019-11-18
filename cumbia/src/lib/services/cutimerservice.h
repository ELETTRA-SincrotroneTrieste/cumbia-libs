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
    CuTimer *findTimer(const CuTimerListener *th);
    std::list<CuTimer *> getTimers();
    std::list<CuTimerListener *>getTimerListeners(int timeout);
    void unregisterListener(CuTimerListener *th);
    void changeTimeout(CuTimerListener *th, int timeout);
    bool isRegistered(CuTimerListener *th, int timeout);

    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    void m_stopAll();

    CuTimerServicePrivate *d;
};

#endif // CUTIMERSERVICE_H
