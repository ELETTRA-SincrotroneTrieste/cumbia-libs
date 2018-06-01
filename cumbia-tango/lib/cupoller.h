#ifndef CUPOLLER_H
#define CUPOLLER_H

#include <cuthreadlistener.h>

class CuTangoActionI;
class CuData;
class CuPollerPrivate;
class CumbiaTango;
class TSource;

class CuPoller : public CuThreadListener
{
public:
    CuPoller(CumbiaTango* cu_t, int period);

    int period() const;

    void registerAction(const TSource &tsrc, CuTangoActionI *a);

    void unregisterAction(CuTangoActionI *a);

    // CuThreadListener interface
public:
    virtual void onProgress(int step, int total, const CuData &data);
    virtual void onResult(const CuData &data);
    virtual void onResult(const std::vector<CuData> &datalist);
    virtual CuData getToken() const;

private:
    CuPollerPrivate *d;
};

#endif // CUPOLLER_H
