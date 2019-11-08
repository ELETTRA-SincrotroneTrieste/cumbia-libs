#ifndef CUPOLLER_H
#define CUPOLLER_H

#include <cuthreadlistener.h>
#include <sys/types.h> // size_t

class CuTangoActionI;
class CuData;
class CuPollerPrivate;
class CumbiaTango;
class TSource;

class CuPoller : public CuThreadListener
{
public:
    CuPoller(CumbiaTango* cu_t, int period);

    virtual ~CuPoller();

    int period() const;

    void registerAction(const TSource &tsrc, CuTangoActionI *a, const CuData &options);

    void unregisterAction(CuTangoActionI *a);

    bool actionRegistered(const CuTangoActionI *a) const;

    size_t count() const;

    // CuThreadListener interface
public:
    virtual void onProgress(int step, int total, const CuData &data);
    virtual void onResult(const CuData &data);
    virtual void onResult(const std::vector<CuData> &datalist);
    virtual CuData getToken() const;

private:
    CuPollerPrivate *d;

    void m_do_unregisterAction(CuTangoActionI *a);
};

#endif // CUPOLLER_H
