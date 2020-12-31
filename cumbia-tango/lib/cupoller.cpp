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
#include <cuthreadevents.h>

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
    delete d;
    pdelete("~CuPoller %p", this);
}

int CuPoller::period() const
{
    return d->period;
}

void CuPoller::registerAction(const TSource& tsrc, CuTangoActionI *a, const CuData& options)
{
    // insert in this thread
    d->actions_map.insert(std::pair<const CuTangoActionI*, const TSource>(a, tsrc));
    pgreen(" + CuPoller.registerAction: added %s - %p to poller - period %d\n", tsrc.getName().c_str(), a, d->period);

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
        // thread token. CuTReader.setOptions can customize thread grouping behaviour
        CuData tt;
        options.containsKey("thread_token") ? tt = options : tt = CuData("device", tsrc.getDeviceName());
        at.merge(options); // as of v1.1.2, merge options into activity token
        activity = new CuPollingActivity(at, df);
        const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
        const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
        d->cumbia_t->registerActivity(activity, this, tt, fi, bf);
    }
    d->cumbia_t->postEvent(activity, new CuAddPollActionEvent(a->getSource(), a));
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
 * \li after the last action is removed, this object is removed from the CuActivityManager
 * thread listeners before next step:
 * \li a CuRemovePollActionEvent is posted to the CuPollingActivity managing the CuTangoActionI device
 * with the CuPoller::period interval
 * \li a dummy CuData with the "exit" flag is delivered through CuTangoActionI::onResult.
 * This is why unregister operations must be enqueued and performed outside result delivery taking place
 * in CuPoller::onResult
 *
 */
void CuPoller::unregisterAction(CuTangoActionI *a) {
    if(!d->deliveringResults) m_do_unregisterAction(a);
    else d->to_remove_actionlist.push_back(a);
}

bool CuPoller::actionRegistered(const CuTangoActionI *a) const {
    return d->actions_map.find(a) != d->actions_map.end();
}

/*!
 * \brief counts the number of actions
 * \return the number of actions
 */
size_t CuPoller::count() const {
    return d->actions_map.size();
}

void CuPoller::onProgress(int , int , const CuData &) { }

void CuPoller::onResult(const CuData &) { }

void CuPoller::onResult(const std::vector<CuData> &datalist)
{
    d->deliveringResults = true;
    // for each CuData, get the point and find the associated CuTangoActionI's, if still there's one or more
    //
    for(size_t i = 0; i < datalist.size(); i++) {
        CuTangoActionI *receiver = static_cast<CuTangoActionI *>(datalist[i]["action_ptr"].toVoidP());
        // receiver information arrives from another thread. receiver may have been destroyed meanwhile
        if(d->actions_map.find(receiver) != d->actions_map.end()) {
            receiver->onResult(datalist[i]);
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

// m_do_unregisterAction
// - find action a
// - remove a from the actions_map
// this object will be deleted by CuPollingService upon returning from
// this method (actually from the caller CuPoller.unregisterAction) if
// there are no actions left. So:
//   - if actions_map empty: call CuActivityManager.removeConnection
//     to immediately remove this from the thread listeners
//   - post a CuRemovePollActionEvent to the activity matching this action
//     to remove the action
//     - CuPollingActivity::m_unregisterAction will call CuThread.unregisterActivity
//       if no actions are left
//
void CuPoller::m_do_unregisterAction(CuTangoActionI *a)
{
    // remove in this thread
    if(d->actions_map.find(a) != d->actions_map.end()) {
        d->actions_map.erase(a);
        TSource tsrc = a->getSource();
        CuData at("device", tsrc.getDeviceName()); /* activity token */
        at["activity"] = "poller";
        at["period"] = d->period;
        CuActivityManager *am = static_cast<CuActivityManager *>(d->cumbia_t->getServiceProvider()->
                                                                 get(static_cast<CuServices::Type> (CuServices::ActivityManager)));
        if(d->actions_map.size() == 0)
            am->removeConnection(this);
        CuActivity *activity = am->findActiveMatching(at); // polling activities compare device period and "activity"
        // post remove to activity's thread
        if(activity) {
            // CuPollingActivity will unregister itself if this is the last action
            d->cumbia_t->postEvent(activity, new CuRemovePollActionEvent(a->getSource()));
        }
    }
}

