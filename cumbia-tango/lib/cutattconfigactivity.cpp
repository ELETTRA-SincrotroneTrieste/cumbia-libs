#include "cutattconfigactivity.h"
#include <tango.h>
#include <cumacros.h>
#include <cudatatypes_ex.h>
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
};

CuTAttConfigActivity::CuTAttConfigActivity(const CuData &tok, CuDeviceFactoryService *df) : CuActivity(tok)
{
    d = new CuTAttConfigActivityPrivate;
    d->device_service = df;
    d->tdev = NULL;
    d->err = false;
    d->other_thread_id = pthread_self();
    d->exiting = false;
    d->repeat = -1;
    d->try_cnt = 0;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
}

void CuTAttConfigActivity::setDesiredAttributeProperties(const std::vector<string> &props)
{
    d->props = props;
}

CuTAttConfigActivity::~CuTAttConfigActivity()
{
    pdelete("CuTAttConfigActivity %p", this);
    delete d;
}

int CuTAttConfigActivity::getType() const
{
    return CuAttConfigActivityType;
}

void CuTAttConfigActivity::event(CuActivityEvent *e)
{
    (void )e;
}

bool CuTAttConfigActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token[CuDType::Src] == mytok[CuDType::Src] && mytok[CuDType::Activity] == token[CuDType::Activity];
}

int CuTAttConfigActivity::repeat() const
{
    return -1;
}

void CuTAttConfigActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    /* get a TDevice */
    d->tdev = d->device_service->getDevice(tk[CuXDType::Device].toString());
    d->tdev->addRef();
    tk[CuDType::Message] = d->tdev->getMessage();
    tk[CuXDType::Connected] = d->tdev->isValid();
    tk[CuDType::Err] = !d->tdev->isValid();
    tk.putTimestamp();
}

void CuTAttConfigActivity::execute()
{
    assert(d->tdev != NULL);
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    d->err = !d->tdev->isValid();
    std::string point = at[CuXDType::Point].toString();
    bool cmd = at[CuXDType::IsCommand].toBool();
    at[CuXDType::Properties] = std::vector<std::string>();
    at[CuDType::Type] = "property";

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
            if(success) {
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

        at[CuDType::Message] = utils.getLastMessage();
        at[CuDType::Err] = utils.error();
        d->err = utils.error();
        d->msg = utils.getLastMessage();

        // retry?
        d->err ?  d->repeat = 2000 * d->try_cnt : d->repeat = -1;
        publishResult(at);
    }
}

void CuTAttConfigActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    if(!d->exiting)
    {
        int refcnt = -1;
        CuData at = getToken(); /* activity token */
        at[CuDType::Message] = d->msg;
        at[CuDType::Type] = "property";
        at[CuDType::Err] = d->err;
        CuTangoWorld utils;
        utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
        if(d->tdev)
            refcnt = d->tdev->removeRef();
        if(refcnt == 0)
        {
            d->device_service->removeDevice(at[CuXDType::Device].toString());
            d->tdev = NULL;
        }
        at[CuDType::Exit] = true;
        publishResult(at);
    }
    else
        perr("CuTAttConfigActivity.onExit already called for %p", this);
}
