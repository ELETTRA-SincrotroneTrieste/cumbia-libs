#include "cueventactivity.h"
#include "tdevice.h"
#include "cutango-world.h"
#include "cudevicefactoryservice.h"
#include <cumacros.h>
#include <tango.h>

CuActivityEvent::Type CuTAStopEvent::getType() const
{
    return static_cast<CuActivityEvent::Type>(CuActivityEvent::User + 10);
}

class CuEventActivityPrivate
{
public:
    CuDeviceFactoryService *device_srvc;
    TDevice *tdev;
    int event_id;
    pthread_t my_thread_id, other_thread_id;
    omni_thread::ensure_self *se;
};

CuEventActivity::CuEventActivity(const CuData &token,  CuDeviceFactoryService *df) : CuActivity(token)
{
    d = new CuEventActivityPrivate;
    setFlag(CuActivity::CuAUnregisterAfterExec, false);
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->device_srvc = df;
    d->tdev = NULL;
    d->event_id = -1;
    d->other_thread_id = pthread_self();
    d->se = NULL;
}

CuEventActivity::~CuEventActivity()
{
    pdelete("~CuEventActivity %p", this);
    delete d;
}

int CuEventActivity::getType() const
{
    return CuEventActivityType;
}

void CuEventActivity::event(CuActivityEvent *e)
{

}

bool CuEventActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

int CuEventActivity::repeat() const
{
    return false;
}

void CuEventActivity::init()
{
    // hack to FIX event failure if subscribing to more than one device
    // in the same application
    d->se = new omni_thread::ensure_self;

    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    /* get a reference to a TDevice, new or existing one */
    d->tdev = d->device_srvc->getDevice(tk["device"].toString());
    pbgreen("CuEventActivity.init: got TDevice %p DeviceProxy %p name %s from THIS THREAD 0x%lx is valid: %d",
            d->tdev, d->tdev->getDevice(), d->tdev->getName().c_str(), pthread_self(), d->tdev->isValid());
    tk["conn"] = d->tdev->isValid();
    tk["msg"] = d->tdev->getMessage();
    CuTangoWorld().fillThreadInfo(tk, this);
    //  sleep(5);
    d->tdev->addRef();
    publishResult(tk);
}

Tango::EventType CuEventActivity::m_tevent_type_from_string(const std::string& set) const
{
    if(set == "ArchiveEventRefresh")
        return Tango::ARCHIVE_EVENT;
    else if(set ==  "PeriodicEventRefresh")
        return Tango::PERIODIC_EVENT;
    return Tango::CHANGE_EVENT;
}

void CuEventActivity::execute()
{
    assert(d->tdev != NULL);
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    std::string devnam = at["device"].toString();
    std::string att = at["point"].toString();
    const std::string ref_mode_str = at["rmode"].toString();
    Tango::DeviceProxy *dev = d->tdev->getDevice();
    at["err"] = !d->tdev->isValid();
    if(dev)
    {
        try
        {
            d->event_id = dev->subscribe_event(att, m_tevent_type_from_string(ref_mode_str), this);
            at["msg"] = "subscribe to: " + ref_mode_str;
            pbgreen("subscribed %s/%s to change events err flag is %d" , devnam.c_str(), att.c_str(), at["err"].toBool());
        }
        catch(Tango::DevFailed &e)
        {
            d->event_id = -1;
            at["err"] = true;
            at["msg"] = CuTangoWorld().strerror(e);
            pbyellow("failed to subscribe events for \"%s/%s\": \"%s\" ----> SHOULD UNREGISTER... soon!", devnam.c_str(), att.c_str(), at["msg"].toString().c_str());
            publishResult(at);
        }
    }
    else
    {
        at["msg"] = d->tdev->getMessage();
    }
    /* do not publish result if subscription is successful because push_event with the first result is invoked immediately */
}

void CuEventActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    std::string devnam = at["device"].toString();
    std::string att = at["point"].toString();
    int refcnt;
    cuprintf(">>>>>>>>>>>> CuEventActivity.onExit: device %p event id %d\e[0m\n", d->tdev->getDevice(), d->event_id);
    if(d->tdev->getDevice() && d->event_id != -1)
    {
        at["value"] = "-";
        try{
            d->tdev->getDevice()->unsubscribe_event(d->event_id);
            pbgreen("ReadActivity.onExit: unsubscribed! OK!");
            at["msg"] = "successfully unsubscribed events";
            at["err"]  = false;
        }
        catch(Tango::DevFailed &e)
        {
            at["msg"] = CuTangoWorld().strerror(e);
            at["err"]  = true;
        }


    }
    refcnt = d->tdev->removeRef();
    cuprintf("\e[1;31mCuEventActivity::onExit(): refcnt = %d called actionRemove for device %s att %s\e[0m\n",
           refcnt, at["device"].toString().c_str(), at["src"].toString().c_str());
    if(refcnt == 0)
        d->device_srvc->removeDevice(at["device"].toString());
    CuTangoWorld().fillThreadInfo(at, this); /* put thread and activity addresses as info */
    at["exit"] = true;
    publishResult(at);

    // delete omni_thread::ensure_self
    if(d->se) delete d->se;
}

void CuEventActivity::push_event(Tango::EventData *e)
{
    CuData d = getToken();
    CuTangoWorld utils;
    pbyellow2("ReadActivity.push_event: in thread: 0x%lx attribute %s activity %p", pthread_self(), e->attr_name.c_str(), this);
    utils.fillThreadInfo(d, this); /* put thread and activity addresses as info */
    d["mode"] = "EVENT";
    d["event"] = e->event;
    d["rmode"] = d["rmode"];
    Tango::DeviceAttribute *da = e->attr_value;
    if(!e->err)
    {
        utils.extractData(da, d);
        d["msg"] = utils.getLastMessage();
        d["err"] = utils.error();
    }
    else
    {
        d["err"] = true;
        d["msg"] = utils.strerror(e->errors);
    }
    publishResult(d);
}

