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
    bool deliveringResults; // true if inside the onResult method
    std::list<CuTangoActionI *> to_remove_actionlist;
};

CuPoller::CuPoller(CumbiaTango *cu_t, int period)
{
    d = new CuPollerPrivate;
    d->period = period;
    d->cumbia_t = cu_t;
    d->token = CuData("period", period);
    d->token["class"] = "CuPoller";
    d->token["activity_count"] = 0;
    d->deliveringResults = false;
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

    CuActivity *activity = am->findActiveMatching(at); // polling activities compare device period and "activity"
    if(!activity) {
        CuData tt("device", tsrc.getDeviceName()); /* thread token */
        activity = new CuPollingActivity(at, df);
        const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
        const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
        d->cumbia_t->registerActivity(activity, this, tt, fi, bf);
        pgreen("(+) CuPoller.registerAction: created a new polling activity for device \"%s\" period %d\n",
                  at["device"].toString().c_str(), at["period"].toInt());
    }
    else {
        pviolet2("(i) CuPoller.registerAction: found a running polling activity [%p] DISPOSABLE %d for device \"%s\" period %d\n",
                  activity, activity->isDisposable(), at["device"].toString().c_str(), at["period"].toInt());
    }

    // post insert to activity's thread. TSource is all what polling activity needs. don't pass pointers
    // to something that can be destroyed while activity is running in the background
    d->cumbia_t->postEvent(activity, new CuAddPollActionEvent(a->getSource(), a));
    //    if(d->actions_map.size() == 1)
    //        d->cumbia_t->resumeActivity(activity);
}

/*! \brief unregister an action from the poller.
 *
 * @param a the action to unregister
 *
 * A CuTangoActionI can invoke unregisterAction from within CuTangoActionI::onResult. In this case, the
 * operation is delayed after CuPoller::onResult has finished delivering results.
 * If the method is called when CuPoller is not inside CuPoller::onResult, the action is unregistered
 * immediately.
 *
 * \li a CuRemovePollActionEvent is posted to the CuPollingActivity managing the CuTangoActionI device
 * with the CuPoller::period interval
 * \li a dummy CuData with the "exit" flag is delivered through CuTangoActionI::onResult.
 * This is why unregister operations must be enqueued and performed outside result delivery taking place
 * in CuPoller::onResult
 *
 */
void CuPoller::unregisterAction(CuTangoActionI *a)
{
    if(!d->deliveringResults)
        m_do_unregisterAction(a);
    else
        d->to_remove_actionlist.push_back(a);
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
    d->deliveringResults = true;
    // for each CuData, get the point and find the associated CuTangoActionI's, if still there's one or more
    //
    for(size_t i = 0; i < datalist.size(); i++) {
        CuTangoActionI *receiver = static_cast<CuTangoActionI *>(datalist[i]["action_ptr"].toVoidP());
        // receiver information arrives from another thread. receiver may have been destroyed meanwhile
        const std::string& src = datalist[i]["src"].toString();
        if(d->actions_map.find(receiver) != d->actions_map.end()) {
            receiver->onResult(datalist[i]);
        }
        else {
            pred("CuPoller.onResult: [%s] action \e[1;36m%p\e[1;31m has been removed! [%s]",
                    src.c_str(), receiver, datalist[i].toString().c_str());
        }
    }
    d->deliveringResults = false;
    for(std::list<CuTangoActionI*>::iterator it = d->to_remove_actionlist.begin(); it != d->to_remove_actionlist.end(); ++it) {
        m_do_unregisterAction(*it);
    }
    d->to_remove_actionlist.clear();
}

CuData CuPoller::getToken() const
{
    return d->token;
}

void CuPoller::m_do_unregisterAction(CuTangoActionI *a)
{
    // remove in this thread

    if(d->actions_map.find(a) != d->actions_map.end()) {
        pgreen(" - CuPoller.unregisterAction: removed %s - %p from poller with period %d actions count %d\n",
                a->getSource().getName().c_str(), a, d->period, count());
        d->actions_map.erase(a);
        pgreen(" - CuPoller.unregisterAction: after removing %p count %d\n", a, count());
        TSource tsrc = a->getSource();
        CuData at("device", tsrc.getDeviceName()); /* activity token */
        at["activity"] = "poller";
        at["period"] = d->period;

        CuActivityManager *am = static_cast<CuActivityManager *>(d->cumbia_t->getServiceProvider()->
                                                                 get(static_cast<CuServices::Type> (CuServices::ActivityManager)));
        CuActivity *activity = am->findActiveMatching(at); // polling activities compare device period and "activity"
        // post remove to activity's thread
        if(activity) {
            d->cumbia_t->postEvent(activity, new CuRemovePollActionEvent(a->getSource()));
        }

        if(d->actions_map.size() == 0) {
            am->removeConnection(this);
        }
    }
}
