#include "cupollingservice.h"
#include <cuactivitymanager.h>
#include <cumbiatango.h>
#include <cuservices.h>
#include "cupoller.h"
#include <map>

class CuPollingServicePrivate {
public:
    CuPollingServicePrivate() : updpo(CuPollDataUpdatePolicy::UpdateAlways) {}

    std::map<int, CuPoller *> pollers_map;
    CuPollDataUpdatePolicy updpo;
};

CuPollingService::CuPollingService() {
    d = new CuPollingServicePrivate;
}

CuPollingService::~CuPollingService() {
    delete d;
}

CuPoller *CuPollingService::getPoller(CumbiaTango *cu_t, int period) {
    if(d->pollers_map.find(period) != d->pollers_map.end())
        return d->pollers_map[period];
    else {
        CuPoller *poller = new CuPoller(cu_t, period);
        d->pollers_map[period] = poller;
        return poller;
    }
}

void CuPollingService::registerAction(CumbiaTango *ct,
                                      const TSource &tsrc,
                                      int period,
                                      CuTangoActionI *action,
                                      const CuData& options,
                                      const CuData& tag) {
    CuPoller *poller = getPoller(ct, period);
    poller->registerAction(tsrc, action, options, tag, d->updpo);
}

void CuPollingService::unregisterAction(int period, CuTangoActionI *action) {
    if(d->pollers_map.find(period) != d->pollers_map.end()) {
        CuPoller* poller = d->pollers_map[period];
        poller->unregisterAction(action);
        if(poller->count() == 0) {
            d->pollers_map.erase(period);
            delete poller;
        }
    }
}

bool CuPollingService::actionRegistered(CuTangoActionI *ac, int period) {
    if(d->pollers_map.find(period) != d->pollers_map.end()) {
        CuPoller* poller = d->pollers_map[period];
        return poller->actionRegistered(ac->getSource().getName());
    }
    return false;
}

void CuPollingService::setDataUpdatePolicy(CuPollDataUpdatePolicy p) {
    d->updpo = p;
}

CuPollDataUpdatePolicy CuPollingService::dataUpdatePolicy() const {
    return d->updpo;
}


std::string CuPollingService::getName() const {
    return "CuPollingService";
}

CuServices::Type CuPollingService::getType() const {
    return static_cast<CuServices::Type> (CuPollingServiceType);
}
