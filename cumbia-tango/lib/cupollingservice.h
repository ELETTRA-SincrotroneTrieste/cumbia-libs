#ifndef CUPOLLINGSERVICE_H
#define CUPOLLINGSERVICE_H

#include <cuservicei.h>

class CuPoller;
class CuPollingServicePrivate;
class CumbiaTango;
class TSource;
class CuTangoActionI;
class CuData;

class CuPollingService : public CuServiceI
{
public:

    enum Type { CuPollingServiceType = CuServices::User + 26 };

    CuPollingService();

    virtual ~CuPollingService();

    CuPoller *getPoller(CumbiaTango *cu_t, int period);

    void registerAction(CumbiaTango* ct, const TSource& tsrc, int period, CuTangoActionI *action, const CuData &options);

    void unregisterAction(int period, CuTangoActionI *action);

    bool actionRegistered(CuTangoActionI *ac, int period);

    // CuServiceI interface
public:
    virtual std::string getName() const;
    virtual CuServices::Type getType() const;

private:
    CuPollingServicePrivate *d;
};

#endif // CUPOLLINGSERVICE_H
