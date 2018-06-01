#include "cupollingservice.h"
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


std::string CuPollingService::getName() const
{
    return "CuPollingService";
}

CuServices::Type CuPollingService::getType() const
{
    return static_cast<CuServices::Type> (CuPollingServiceType);
}
