#include "cupoller.h"
#include <cudata.h>
#include <map>
#include <cutangoactioni.h>
#include <cupollingactivity.h>
#include <cupollingactivity.h>
#include <string>
#include <cumbiatango.h>
#include <cuactivitymanager.h>
#include <cuservices.h>
#include <cuserviceprovider.h>
#include <cudevicefactoryservice.h>

class CuPollerPrivate
{
public:
    CumbiaTango *cumbia_t;
    // maps point to action
    std::map<const CuTangoActionI*, const TSource > actions_map;
    // maps device name -> activity
    std::map<std::string, CuPollingActivity *> activity_map;
    int period;
    CuData token;
};

CuPoller::CuPoller(CumbiaTango *cu_t, int period)
{
    d = new CuPollerPrivate;
    d->period = period;
    d->cumbia_t = cu_t;
    d->token = CuData("period", period);
    d->token["class"] = "CuPoller";
    d->token["activity_count"] = 0;
}

CuPoller::~CuPoller()
{
    pdelete("~CuPoller %p", this);
}

int CuPoller::period() const
{
    return d->period;
}

void CuPoller::registerAction(const TSource& tsrc, CuTangoActionI *a)
{
    // insert in this thread
    d->actions_map.insert(std::pair<const CuTangoActionI*, const TSource>(a, tsrc));
    pgreen(" + CuPoller.registerAction: added %s - %p to poller\n", tsrc.getName().c_str(), a);

    CuActivityManager *am = static_cast<CuActivityManager *>(d->cumbia_t->getServiceProvider()->
                                                             get(static_cast<CuServices::Type> (CuServices::ActivityManager)));
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));
    CuData at("device", tsrc.getDeviceName()); /* activity token */
    at["activity"] = "poller";
    at["period"] = d->period;

    CuActivity *activity = am->findMatching(at); // polling activities compare device period and "activity"
    if(!activity) {
        CuData tt("device", tsrc.getDeviceName()); /* thread token */
        activity = new CuPollingActivity(at, df);
        const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
        const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
        d->cumbia_t->registerActivity(activity, this, tt, fi, bf);
        pgreen("(+) CuPoller.m_startPollingActivity: created a new polling activity for device \"%s\" period %d\n",
                  at["device"].toString().c_str(), at["period"].toInt());
    }
    else {
        pgreen("(i) CuPoller.m_startPollingActivity: found a running polling activity [%p] DISPOSABLE %d for device \"%s\" period %d\n",
                  activity, activity->isDisposable(), at["device"].toString().c_str(), at["period"].toInt());
        if(d->actions_map.size() == 1)
            d->cumbia_t->resumeActivity(activity);
    }

    // post insert to activity's thread. TSource is all what polling activity needs. don't pass pointers
    // to something that can be destroyed while activity is running in the background
    d->cumbia_t->postEvent(activity, new CuAddPollActionEvent(a->getSource(), a));
    //    if(d->actions_map.size() == 1)
    //        d->cumbia_t->resumeActivity(activity);
}

void CuPoller::unregisterAction(CuTangoActionI *a)
{
    // remove in this thread
    if(d->actions_map.find(a) != d->actions_map.end()) {
        pgreen(" - CuPoller.unregisterAction: removed %s - %p from poller with period %d\n",
                a->getSource().getName().c_str(), a, d->period);
        d->actions_map.erase(a);
    }

    TSource tsrc = a->getSource();
    CuData at("device", tsrc.getDeviceName()); /* activity token */
    at["activity"] = "poller";
    at["period"] = d->period;

    CuActivityManager *am = static_cast<CuActivityManager *>(d->cumbia_t->getServiceProvider()->
                                                             get(static_cast<CuServices::Type> (CuServices::ActivityManager)));
    CuActivity *activity = am->findMatching(at); // polling activities compare device period and "activity"
    // post remove to activity's thread
    if(activity) {
        d->cumbia_t->postEvent(activity, new CuRemovePollActionEvent(a->getSource()));
    }
    a->onResult(CuData("exit", true));
    if(d->actions_map.size() == 0) {
        am->removeConnection(this);
    }
}

bool CuPoller::actionRegistered(const CuTangoActionI *a) const
{
    return d->actions_map.find(a) != d->actions_map.end();
}

size_t CuPoller::count() const
{
    return d->actions_map.size();
}

void CuPoller::onProgress(int step, int total, const CuData &data)
{
}

void CuPoller::onResult(const CuData &data)
{
    pyellow2("CuPoller.onResult data %s EXIT FLAG %d", data.toString().c_str(), data["exit"].toBool());
    pyellow2("actions size now: %ld", d->actions_map.size());
}

void CuPoller::onResult(const std::vector<CuData> &datalist)
{
    // for each CuData, get the point and find the associated CuTangoActionI's, if still there's one or more
    //
    for(size_t i = 0; i < datalist.size(); i++) {
        CuTangoActionI *receiver = static_cast<CuTangoActionI *>(datalist[i]["action_ptr"].toVoidP());
        // receiver information arrives from another thread. receiver may have been destroyed meanwhile
        const std::string& src = datalist[i]["src"].toString();
        if(d->actions_map.find(receiver) != d->actions_map.end()) {
            //            pgreentmp("CuPoller.onResult: [%s] delivering result to \e[1;36m%p\e[0m", src.c_str(), receiver);
            receiver->onResult(datalist[i]);
        }
        else {
            pred("CuPoller.onResult: [%s] action \e[1;36m%p\e[1;31m has been removed! [%s]",
                    src.c_str(), receiver, datalist[i].toString().c_str());
        }

    }
}

CuData CuPoller::getToken() const
{
    return d->token;
}
