#ifndef CUPOLLER_H
#define CUPOLLER_H

#include <cuthreadlistener.h>
#include <cupollingservice.h>
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
    void registerAction(const TSource &tsrc, CuTangoActionI *a, const CuData &options, const CuData &tag, int updpo);
    void unregisterAction(CuTangoActionI *a);
    bool actionRegistered(const std::string &src) const;
    size_t count() const;

    // CuThreadListener interface
    virtual void onProgress(int step, int total, const CuData &data);
    virtual void onResult(const CuData &data);
    virtual void onResult(const CuData *p, int siz);
    virtual CuData getToken() const;

private:
    CuPollerPrivate *d;

    CuTangoActionI *m_find_a(const std::string& src) const;
};

#endif // CUPOLLER_H
