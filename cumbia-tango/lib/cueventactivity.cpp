#include "cueventactivity.h"
#include "tdevice.h"
#include "cutango-world.h"
#include "tsource.h"
#include "cutthread.h"
#include "cudevicefactory_i.h"
#include <cumacros.h>
#include <tango/tango.h>

/*! @private */
CuActivityEvent::Type CuTAStopEvent::getType() const {
    return static_cast<CuActivityEvent::Type>(CuActivityEvent::User + 10);
}

/*! @private
 */
class CuEventActivityPrivate {
public:
    CuData tag, atok; // tag is carried along results, activity token initialized in constructor
    CuDeviceFactory_I *devfa;
    TDevice *tdev;
    TSource tsrc;
    int event_id;
    int64_t ucnt; // update counter
    pthread_t my_thread_id, other_thread_id;
    std::string refreshmo;
    omni_thread::ensure_self *se;
};

/*! \brief the class constructor that configures the activity event flags
 *
 * @param token a CuData that will identify this activity
 * @param df a pointer to a CuDeviceFactoryService that is used by init and onExit to create/obtain and
 *        later get rid of a Tango device, respectively.
 *
 * \par Default activity flags
 * The default activity flags set by the CuEventActivity constructor are
 *
 * \li CuActivity::CuAUnregisterAfterExec: *false*: the activity is not unregistered after exec.
 *     Instead, it keeps living within an event loop that delivers Tango events over time
 * \li CuActivity::CuADeleteOnExit: *true* lets the activity be deleted after onExit
 */
CuEventActivity::CuEventActivity(const TSource &ts, CuDeviceFactory_I *df, const string &refreshmo, const CuData &tag, int update_policy)
    : CuActivity(CuData("activity", "event").set("src", ts.getName())) {  // token with keys relevant to matches()
    d = new CuEventActivityPrivate;
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->devfa = df;
    d->tdev = NULL;
    d->event_id = -1;
    d->other_thread_id = pthread_self();
    d->se = NULL;
    d->tsrc = ts;
    d->refreshmo = refreshmo;
    d->tag = tag;
    d->atok = getToken();
    d->ucnt = (update_policy & CuDataUpdatePolicy::SkipFirstReadUpdate) ? -1 : 0;
}

/*! \brief the class destructor
 *
 */
CuEventActivity::~CuEventActivity() {
    pdelete("~CuEventActivity %p", this);
    delete d;
}

/*! \brief returns the CuEventActivityType value
 *
 * @return the constant value CuEventActivityType defined in CuEventActivity::Type
 */
int CuEventActivity::getType() const {
    return CuEventActivityType;
}

/** \brief Receive events *from the main thread to the CuActivity thread*.
 *
 * @param e the event. Do not delete e after use. Cumbia will delete it after this method invocation.
 *
 * @see CuActivity::event
 *
 * \note the body of this method is currently empty
 */
void CuEventActivity::event(CuActivityEvent *e) {
    if(e->getType() == CuActivityEvent::Cu_Data) {
        publishResult(static_cast<CuDataEvent *>(e)->getData());
    }
}

/** \brief returns true if the passed token's *src* and *activity* values matche this activity token's
 *         *src* and *activity* values.
 *
 * @param token a CuData containg key/value pairs of another activity's token
 * @return true if the input token's "src" and "activity" values match this token's "src" and "activity"
 *         values
 *
 * Two CuEventActivities match if the "src" and the "activity" names match.
 * CuEventActivity "activity" key is set to the "E" value by CuTReader, so two activities match
 * if they are both CuEventActivity and share the same source name.
 *
 */
bool CuEventActivity::matches(const CuData &token) const {
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

/*! \brief returns 0. CuEventActivity's execute is called only once.
 *
 * @return the integer -1
 *
 * @see CuActivity::repeat
 */
int CuEventActivity::repeat() const {
    return -1;
}

/*! \brief the implementation of the CuActivity::init hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \par Notes
 * \li in cumbia-tango, threads are grouped by device
 * \li a new omni_thread::ensure_self is created and lives across the entire CuEventActivity lifetime
 *     to work around a known Tango issue that breaks the event reception when the client application
 *     subscribes to attributes across different devices
 * \li CuDeviceFactoryService::getDevice is called to obtain a reference to a Tango device (in the form
 *     of TDevice)
 * \li TDevice's user refrence count is incremented with TDevice::addRef
 *
 * See also CuActivity::init, execute and onExit
 *
 */
void CuEventActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    // hack to FIX event failure if subscribing to more than one device
    // in the same application
    d->se = new omni_thread::ensure_self;
    /* get a TDevice reference, new or existing. getDevice increases refcnt for the device */
    if(thread()->type() == CuTThread::CuTThreadType) // upgrade to CuTThread / lock free CuTThreadDevices
        d->devfa = static_cast<CuTThread *>(thread())->device_factory();
    d->tdev = d->devfa->getDevice(d->tsrc.getDeviceName(), threadToken());
    // since v1.2.0, do not publishResult
}

Tango::EventType CuEventActivity::m_tevent_type_from_string(const std::string& set) const
{
    if(set == "ArchiveEventRefresh")
        return Tango::ARCHIVE_EVENT;
    else if(set ==  "PeriodicEventRefresh")
        return Tango::PERIODIC_EVENT;
    return Tango::CHANGE_EVENT;
}

/*! \brief the implementation of the CuActivity::execute hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \par Notes
 * \li subscribe_event for the attribute to read is called on the TangoDevice and the desired
 *     event subscription type is requested according to the "rmode" (refresh mode) value in
 *     the activity token obtained by getToken
 *
 * If subscribe_event is successful, the client will start receiving events through the
 * Tango::Callback push_event function.
 * If an error occurs here, publishResult is called in order to deliver the error message
 * to the main thread, where a CuDataListener will deal with it.
 *
 * \par note
 * Typically, if subscribe_event fails, a CuPollingActivity is started as a fallback mode for
 * the reader.
 *
 * To learn about the contents of the CuData delivered as result, please see the CuEventActivity::push_event
 * documentation.
 *
 * See also CuActivity::execute
 *
 * \par note
 * In the CuEventActivity::push_event callback, CuData "E" value is copied from
 * Tango::EventData::event. Here data["E"] is set to "subscribe" to identify the
 * *subscribe_event* phase (CuTReader looks for this not to issue an error if subscription fails).
 *
 *
 */
void CuEventActivity::execute()
{
    assert(d->tdev != NULL);
    assert(d->my_thread_id == pthread_self());
    CuData at("activity", "event"); /* activity token */
    std::string att = d->tsrc.getPoint();
    const std::string& ref_mode_str = d->refreshmo;
    Tango::DeviceProxy *dev = d->tdev->getDevice();
    at.set("src", d->tsrc.getName()).set("mode", "E").set("E", "subscribe").putTimestamp();
    at["err"] = !d->tdev->isValid();
    if(dev) {
        try {

            d->event_id = dev->subscribe_event(att, m_tevent_type_from_string(ref_mode_str), this);
            at["msg"] = "subscribe to: " + ref_mode_str;
        }
        catch(Tango::DevFailed &e) {
            d->event_id = -1;
            at["err"] = true;
            at["msg"] = CuTangoWorld().strerror(e);
            at["ev_except"] = true;
            publishResult(at);
        }
    }
    else
        at["msg"] = d->tdev->getMessage();
    /* do not publish result if subscription is successful because push_event with the first result is invoked immediately */
}

/*! \brief the implementation of the CuActivity::onExit hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \li unsubscribe_event is called for the Tango attribute
 * \li client reference counter is decreased on the TDevice (TDevice::removeRef)
 * \li CuDeviceFactoryService::removeDevice is called to remove the device from the device factory
 *     if the reference count is zero
 * \li publishResult is not called (it's likely that all listeners have been removed)
 * \li the omni_thread::ensure_self instance is deleted
 *
 * See also CuActivity::onExit
 */
void CuEventActivity::onExit() {
    assert(d->my_thread_id == pthread_self());
    int refcnt = -1;
    if(d->tdev->getDevice() && d->event_id != -1) {
        try {
            d->tdev->getDevice()->unsubscribe_event(d->event_id);
            cuprintf("CuEventActivity.onExit: \e[1;35munsubscribed id %d for \e[1;32m%s! OK!\e[0m\n", d->event_id, vtoc2(getToken(), "src"));
        }
        catch(Tango::DevFailed &e) {
            CuTangoWorld tw;
            perr("CuEventActivity.onExit: failed to unsubscribe_event for src \"%s\": \"%s\"",
                 d->tdev->getName().c_str(), tw.strerror(e).c_str());
        }
    }
    // removes reference (lock guarded) and deletes TDev if no more necessary
    // Lock guarded because since 1.1.0 one thread per device is not a rule.
    refcnt = d->devfa->removeRef(d->tdev->getName().c_str(), threadToken());
    if(refcnt == 0)
        d->tdev = nullptr;

    // delete omni_thread::ensure_self
    if(d->se) delete d->se;
}

/*! \brief receive events from Tango, extract data and post the result on the main thread through
 *         publishResult
 *
 * @param e the Tango::EventData passed by the tango callback
 *
 * \li receive the event
 * \li extract data exploiting CuTangoWorld utility methods
 * \li publishResult with the data extracted and packed in a CuData
 *
 * \par contents of the CuData delivered by publishResult ("key": value)
 * \li "is_command": bool: true if the source is a command, false if it is an attribute (CuVariant::toBool)
 * \li "err": bool: true if an error occurred, false otherwise
 * \li "mode": string: the read mode: "E" or "P" ("E" in this case)
 * \li "msg": string: in case of error, the error message is reported
 * \li "E": string: a copy of the value of Tango::EventData::event string
 *      (documented as *the event name* in lib/cpp/client/event.h)
 * \li refer to \ref md_lib_cudata_for_tango for a complete description of the CuData key/value
 *     pairs that result from attribute or command read operations.
 */
void CuEventActivity::push_event(Tango::EventData *e) {
    d->ucnt++;
    // in d, copy only src from token
    CuData da("src", getToken()["src"].toString());
    da.merge(this->d->tag);
    CuTangoWorld utils;
    da["mode"] = "E";
    da["E"] = e->event;
    Tango::DeviceAttribute *dat = e->attr_value;
    if(!e->err)  {
        utils.extractData(dat, da);
        da["err"] = utils.error(); // no "msg" if no err
        if(da.b("err")) da["msg"] = utils.getLastMessage();
        if(d->ucnt > 0) // if -1, skip first (successful) data update
            publishResult(da);
    }
    else  {
        // CuTReader must distinguish between push_event exception
        // and another error, like attribute quality invalid
        da["ev_except"] = true;
        da["err"] = true;
        da["msg"] = utils.strerror(e->errors);
        da.putTimestamp();
        publishResult(da); // publish in case of event subscription failure
    }
}

