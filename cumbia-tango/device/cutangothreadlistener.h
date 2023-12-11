#ifndef CUTANGOTHREADLISTENER_H
#define CUTANGOTHREADLISTENER_H

#include <cuthreadlistener.h>
#include <cumacros.h>
#include "CumbiaAbstractDevice.h"

class CuTangoThreadListenerPrivate
{
public:
    CumbiaAbstractDevice_ns::CumbiaAbstractDevice *cumbiaDev;
    CuData token;
};


class CuTangoThreadListener : public CuThreadListener
{
public:
    ~CuTangoThreadListener() { pdelete("~CuTangoThreadListener %p: %s", this, d->token.toString().c_str()); }

    CuTangoThreadListener(CumbiaAbstractDevice_ns::CumbiaAbstractDevice *cumbiaDev, const CuData& token)
    {
        d = new CuTangoThreadListenerPrivate;
        d->cumbiaDev = cumbiaDev;
        d->token = token;
    }

    // CuThreadListener interface
    void onThreadStarted() {}

    void onProgress(int step, int total, const CuData &data)
    {
        printf("CuTangoThreadListener.onResult: calling onResult on cumbiaDev [%d/%d/%s]...................this thread \e[1;31m0x%lx\e[0m...........\n",
               step, total, data[CuDType::Value].toString().c_str(),
               pthread_self());
        Tango::AutoTangoMonitor synch(d->cumbiaDev);
        d->cumbiaDev->onProgress(step, total, data);
    }

    void onResult(const CuData &data)
    {
        printf("CuTangoThreadListener.onResult: calling onResult on cumbiaDev...................this thread \e[1;31m0x%lx\e[0m...........\n", pthread_self());
        pr_thread();
        Tango::AutoTangoMonitor synch(d->cumbiaDev);
        d->cumbiaDev->onResult(data);
    }

    void onThreadFinished() {}

    CuData getToken() const
    {
        return d->token;
    }

private:
    CuTangoThreadListenerPrivate *d;

};


#endif // CUTANGOTHREADLISTENER_H
