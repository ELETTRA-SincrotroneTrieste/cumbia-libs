#include "cutattconfigactivity.h"
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
};

CuTAttConfigActivity::CuTAttConfigActivity(const CuData &tok, CuDeviceFactoryService *df) : CuIsolatedActivity(tok)
{
    d = new CuTAttConfigActivityPrivate;
    d->device_service = df;
    d->tdev = NULL;
    d->err = false;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->other_thread_id = pthread_self();
    d->exiting = false;
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
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

int CuTAttConfigActivity::repeat() const
{
    return -1;
}

void CuTAttConfigActivity::init()
{
    cuprintf("CuTAttConfigActivity::init: enter\n");
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    /* get a TDevice */
    d->tdev = d->device_service->getDevice(tk["device"].toString());
    d->tdev->addRef();
    tk["msg"] = d->tdev->getMessage();
    tk["conn"] = d->tdev->isValid();
    tk["err"] = !d->tdev->isValid();
}

void CuTAttConfigActivity::execute()
{
    cuprintf("CuTAttConfigActivity::execute: enter\n");
    assert(d->tdev != NULL);
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    d->err = !d->tdev->isValid();
    std::string point = at["point"].toString();
    bool cmd = at["is_command"].toBool();
    at["properties"] = std::vector<std::string>();
    at["type"] = "property";

    bool success = false;

    if(d->tdev->isValid())
    {
        cuprintf("CuTAttConfigActivity::execute: GETTING get_att_config token %s\n", at.toString().c_str());
        Tango::DeviceProxy *dev = d->tdev->getDevice();
        CuTangoWorld utils;
        utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
        if(dev && cmd)
        {
            success = utils.get_command_info(dev, point, at);
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
        at["msg"] = utils.getLastMessage();
        at["err"] = utils.error();
        d->err = utils.error();
        d->msg = utils.getLastMessage();
        publishResult(at);
    }
}

void CuTAttConfigActivity::onExit()
{
//    printf("[0x%lx] CuTAttConfigActivity this %p onExit: enter\n", pthread_self(), this);
    assert(d->my_thread_id == pthread_self());
    if(d->exiting)
    {
//        printf("\e[1;31mCuTAttConfigActivity::onExit onExit already called\e[0m\n!!\n");
        return;
    }
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    at["msg"] = d->msg;
    at["type"] = "property";
    at["err"] = d->err;
    CuTangoWorld utils;
    utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
//    printf("[0x%lx] \e[1;34mCuTAttConfigActivity::onExit this %p  DECREMENTING REF CNT ON TDevice %p\e[0m\n\n", pthread_self(), this, d->tdev);
    if(d->tdev)
        refcnt = d->tdev->removeRef();
    if(refcnt == 0)
    {
        d->device_service->removeDevice(at["device"].toString());
        d->tdev = NULL;
    }
    at["exit"] = true;
//    printf("[0x%lx] \e[1;34mCuTAttConfigActivity::onExit this %p  calling publish result for EXIT!\e[0m\n", pthread_self(), this);
    publishResult(at);
}
