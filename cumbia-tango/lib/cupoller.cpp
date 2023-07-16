#include "cupoller.h"
#include <cudata.h>
#include <unordered_map>
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
#include <cutreader.h>
#include <assert.h>

class CuPollerPrivate
{
public:
    CumbiaTango *cumbia_t;
    std::unordered_map<std::string, CuTangoActionI*> actions_map;
    std::unordered_multimap<CuActivity*, CuTangoActionI*> pa_amap; // polling a -> actions
    int period;
    CuData token;
    std::list<CuTangoActionI *> to_remove_actionlist;

    pthread_t my_thread;
};

CuPoller::CuPoller(CumbiaTango *cu_t, int period)
{
    d = new CuPollerPrivate;
    d->period = period;
    d->cumbia_t = cu_t;
    d->token = CuData(CuDType::Period, period);
    d->token[CuDType::Class] = "CuPoller";
    d->token["activity_count"] = 0;
    //    d->deliveringResults = false;
    d->my_thread = pthread_self();
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

void CuPoller::registerAction(const TSource& tsrc,
                              CuTangoActionI *a,
                              const CuData& options,
                              const CuData& tag,
                              int dataupdpo)
{
    assert(d->my_thread == pthread_self());
    // insert in this thread
    d->actions_map[tsrc.getName()] = a;
    pgreen(" + CuPoller.registerAction: added %s - %p to poller - period %d\n", tsrc.getName().c_str(), a, d->period);

    CuActivityManager *am = static_cast<CuActivityManager *>(d->cumbia_t->getServiceProvider()->
                                                             get(static_cast<CuServices::Type> (CuServices::ActivityManager)));
    CuDeviceFactoryService *df =
            static_cast<CuDeviceFactoryService *>(d->cumbia_t->getServiceProvider()->
                                                  get(static_cast<CuServices::Type> (CuDeviceFactoryService::CuDeviceFactoryServiceType)));
    CuData at(CuDType::Device, tsrc.getDeviceName()); /* activity token */
    at[CuDType::Activity] = "poller";
    at[CuDType::Period] = d->period;
    CuActivity *activity = am->find(at); // polling activities compare device period and "activity"
    if(!activity) {
        // thread token. CuTReader.setOptions can customize thread grouping behaviour
        const std::string& tt = options.containsKey("thread_token") ? options.s("thread_token") : tsrc.getDeviceName();
        activity = new CuPollingActivity(tsrc, df, options, tag, dataupdpo, d->period);
        const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_t->getThreadEventsBridgeFactory());
        const CuThreadFactoryImplI &fi = *(d->cumbia_t->getThreadFactoryImpl());
        d->cumbia_t->registerActivity(activity, this, tt, fi, bf);
    }
    d->cumbia_t->postEvent(activity, new CuAddPollActionEvent(a->getSource(), a));
    d->pa_amap.insert(std::pair<CuActivity *, CuTangoActionI* >(activity, a));
}

/*! \brief unregister an action from the poller.
 */
void CuPoller::unregisterAction(CuTangoActionI *a) {
    assert(d->my_thread == pthread_self());
    // remove in this thread
    const std::string& s = a->getSource().getName();
    if(d->actions_map.find(s) != d->actions_map.end()) {
        d->actions_map.erase(s);
        TSource tsrc = a->getSource();
        CuData at(CuDType::Device, tsrc.getDeviceName()); /* activity token */
        at.set(CuDType::Activity, "poller").set(CuDType::Period, d->period);  // set("activity", "poller")
        CuActivityManager *am = static_cast<CuActivityManager *>(d->cumbia_t->getServiceProvider()->
                                                                 get(static_cast<CuServices::Type> (CuServices::ActivityManager)));
        if(d->actions_map.size() == 0)
            am->disconnect(this);
        CuActivity *activity = am->find(at); // polling activities compare device period and "activity"
        // post remove to activity's thread
        if(activity) {
            d->cumbia_t->postEvent(activity, new CuRemovePollActionEvent(a->getSource()));
            int cnt = 0;
            auto ipair = d->pa_amap.equal_range(activity);
            auto it = ipair.first;
            while(it != ipair.second) {
                if(it->second == a) {
                    it = d->pa_amap.erase(it);
                }
                else {
                    ++it; ++cnt;
                }
            }
            if(cnt == 0) {
                d->cumbia_t->unregisterActivity(activity);
            }
        }
    }
}

bool CuPoller::actionRegistered(const std::string& src) const {
    return d->actions_map.find(src) != d->actions_map.end();
}

/*!
 * \brief counts the number of actions
 * \return the number of actions
 */
size_t CuPoller::count() const {
    assert(d->my_thread == pthread_self());
    return d->actions_map.size();
}

void CuPoller::onProgress(int , int , const CuData &) { }

void CuPoller::onResult(const CuData & da) { }

void CuPoller::onResult(const std::vector<CuData> &datalist) {
    assert(d->my_thread == pthread_self());
}

void CuPoller::onResult(const std::vector<CuData> *datalist) {
    assert(d->my_thread == pthread_self());
    // for each CuData, get the point and find the associated CuTangoActionI's, if still there's one or more
    for(size_t i = 0; i < datalist->size(); i++) {
        const CuData& da = (*datalist)[i];
        const std::string& src = da[CuDType::Src].toString();
        CuTangoActionI *a = m_find_a(src);
        if(a) a->onResult(da);
    }
    // datalist deleted by calling CuThread::onEventPosted
}

CuData CuPoller::getToken() const {
    return d->token;
}

CuTangoActionI *CuPoller::m_find_a(const string &src) const {
    assert(d->my_thread == pthread_self());
    std::unordered_map<std::string, CuTangoActionI *>::const_iterator it = d->actions_map.find(src);
    return it != d->actions_map.end() ? it->second : nullptr;
}

