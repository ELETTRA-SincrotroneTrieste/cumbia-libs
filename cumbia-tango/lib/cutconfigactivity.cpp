#include "cutconfigactivity.h"
#include <tango.h>
#include <cumacros.h>
#include "cudevicefactoryservice.h"
#include "tdevice.h"
#include "cutango-world.h"

class CuTAttConfigActivityPrivate
{
public:
    CuDeviceFactoryService *device_service;
    TDevice *tdev;
    std::string msg;
    bool err;
    pthread_t my_thread_id, other_thread_id;
    std::vector<string> props;
    bool exiting;
    int repeat, try_cnt;
    CuTConfigActivity::Type type;
};

CuTConfigActivity::CuTConfigActivity(const CuData &tok, CuDeviceFactoryService *df, Type t) : CuActivity(tok)
{
    d = new CuTAttConfigActivityPrivate;
    d->device_service = df;
    d->type = t;
    d->tdev = NULL;
    d->err = false;
    d->other_thread_id = pthread_self();
    d->exiting = false;
    d->repeat = -1;
    d->try_cnt = 0;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
}

void CuTConfigActivity::setDesiredAttributeProperties(const std::vector<string> &props)
{
    d->props = props;
}

CuTConfigActivity::~CuTConfigActivity()
{
    pdelete("CuTAttConfigActivity %p", this);
    delete d;
}

int CuTConfigActivity::getType() const
{
    return d->type;
}

void CuTConfigActivity::event(CuActivityEvent *e)
{
    (void )e;
}

bool CuTConfigActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

int CuTConfigActivity::repeat() const
{
    return -1;
}

void CuTConfigActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    const std::string& dnam = tk["device"].toString();
    /* get a TDevice */
    d->tdev = d->device_service->getDevice(dnam, threadToken());
    // thread safe: since cumbia 1.1.0 no thread per device guaranteed
    d->device_service->addRef(dnam, threadToken());
    tk["msg"] =  "CuTConfigActivity.init: " + d->tdev->getMessage();
    tk["conn"] = d->tdev->isValid();
    tk["err"] = !d->tdev->isValid();
    tk.putTimestamp();
}

void CuTConfigActivity::execute()
{
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    d->err = !d->tdev->isValid();
    std::string point = at["point"].toString();
    bool cmd = at["is_command"].toBool();
    at["properties"] = std::vector<std::string>();
    at["type"] = "property";

    d->try_cnt++;
    bool success = false;

    if(d->tdev->isValid())
    {
        Tango::DeviceProxy *dev = d->tdev->getDevice();
        CuTangoWorld utils;
        utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
        if(dev && cmd)
        {
            success = utils.get_command_info(dev, point, at);
            if(success && d->type == CuReaderConfigActivityType) {
                success = utils.cmd_inout(dev, point, at);
            }
        }
        else if(dev)
        {
            success = utils.get_att_config(dev, point, at);
        }
        else
            d->msg = d->tdev->getMessage();

        if(d->props.size() > 0 && success && dev)
            success = utils.get_att_props(dev, point, at, d->props);

        at["data"] = true;
        at["msg"] = "CuTConfigActivity.execute: " + utils.getLastMessage();
        at["err"] = utils.error();
        d->err = utils.error();
        d->msg = utils.getLastMessage();

        // retry?
        d->err ?  d->repeat = 2000 * d->try_cnt : d->repeat = -1;
    }
    else {
        at["msg"] = "CuTConfigActivity.execute: " + d->tdev->getMessage();
        at["err"] = true;
        at.putTimestamp();
    }
    publishResult(at);
}

void CuTConfigActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    if(!d->exiting)
    {
        int refcnt = -1;
        CuData at = getToken(); /* activity token */
        at["msg"] = "CuTConfigActivity.onExit: " + d->msg;
        at["type"] = "property";
        at["err"] = d->err;
        CuTangoWorld utils;
        utils.fillThreadInfo(at, this); /* put thread and actiity addresses as info */
        // thread safe remove ref and disposal
        printf("\e[0;35mCuTConfigActivity::onExit: removing refernce to dev %s d->tdev %p thread 0x%lx thread tok %s\e[0m\n",
               at["device"].toString().c_str(), d->tdev, pthread_self(), threadToken().toString().c_str());
        refcnt = d->device_service->removeRef(at["device"].toString(), threadToken());
        if(!refcnt)
            d->tdev = nullptr;
        at["exit"] = true;
        publishResult(at);
    }
    else
        perr("CuTAttConfigActivity.onExit already called for %p", this);
}
