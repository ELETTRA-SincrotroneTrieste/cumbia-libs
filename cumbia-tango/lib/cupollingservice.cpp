#include "cupollingservice.h"
#include <cuactivitymanager.h>
#include <cumbiatango.h>
#include <cuservices.h>
#include "cupoller.h"
#include <map>

class CuPollingServicePrivate
{
public:
    std::map<int, CuPoller *> pollers_map;
};

CuPollingService::CuPollingService()
{
    d = new CuPollingServicePrivate;
}

CuPollingService::~CuPollingService()
{

}

CuPoller *CuPollingService::getPoller(CumbiaTango *cu_t, int period)
{
    if(d->pollers_map.find(period) != d->pollers_map.end())
        return d->pollers_map[period];
    else {
        CuPoller *poller = new CuPoller(cu_t, period);
        d->pollers_map[period] = poller;
        return poller;
    }
}

void CuPollingService::registerAction(CumbiaTango *ct, const TSource &tsrc, int period, CuTangoActionI *action)
{
    CuPoller *poller = getPoller(ct, period);
    poller->registerAction(tsrc, action);
}

void CuPollingService::unregisterAction(int period, CuTangoActionI *action)
{
    if(d->pollers_map.find(period) != d->pollers_map.end()) {
        CuPoller* poller = d->pollers_map[period];
        poller->unregisterAction(action);
        if(poller->count() == 0) {
            printf("\e[1;31mdeleting poller %p cuz has actions %d\e[0m\n", poller, poller->count());
            d->pollers_map.erase(period);
            delete poller;
        }
        else {
           // printf("\e[1;32mnot deleting poller %p cuz has actions %d\e[0m\n", poller, poller->count());
        }
    }
}

bool CuPollingService::actionRegistered(CuTangoActionI *ac, int period)
{
    if(d->pollers_map.find(period) != d->pollers_map.end()) {
        CuPoller* poller = d->pollers_map[period];
        return poller->actionRegistered(ac);
    }
    return false;
}


std::string CuPollingService::getName() const
{
    return "CuPollingService";
}

CuServices::Type CuPollingService::getType() const
{
    return static_cast<CuServices::Type> (CuPollingServiceType);
}
