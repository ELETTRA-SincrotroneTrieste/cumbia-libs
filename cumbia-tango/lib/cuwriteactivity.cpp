#include "cuwriteactivity.h"
#include "cudevicefactoryservice.h"
#include "tdevice.h"
#include "cutangoactioni.h"
#include "cutango-world.h"

#include <cumacros.h>

class CuWriteActivityPrivate
{
public:
    CuDeviceFactoryService *device_service;
    TDevice *tdev;
    std::string msg;
    bool err;
    pthread_t my_thread_id, other_thread_id;
    CuData point_info; /* attribute or command info */
};

CuWriteActivity::CuWriteActivity(const CuData &token,
                                 CuDeviceFactoryService *df,
                                 const CuData& db_config)
    : CuIsolatedActivity(token)
{
    d = new CuWriteActivityPrivate;
    d->device_service = df;
    d->point_info = db_config;
    d->tdev = NULL;
    d->err = false;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->other_thread_id = pthread_self();
}

CuWriteActivity::~CuWriteActivity()
{
    pdelete("~CuWriteActivity %p", this);
    delete d;
}

void CuWriteActivity::event(CuActivityEvent *e)
{

}

bool CuWriteActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

void CuWriteActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    /* get a TDevice */
    d->tdev = d->device_service->getDevice(tk["device"].toString(), threadToken());
    // thread safely add ref (cumbia 1.1.0: no 1 thread per dev guaranteed)
    d->device_service->addRef(tk["device"].toString(), threadToken());
}

void CuWriteActivity::execute()
{
    assert(d->tdev != NULL);
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    d->err = !d->tdev->isValid();

    if(d->tdev->isValid())
    {
        Tango::DeviceProxy *dev = d->tdev->getDevice();
        CuTangoWorld tangoworld;
        bool success = !d->point_info.isEmpty();
        tangoworld.fillThreadInfo(at, this); /* put thread and activity addresses as info */
        if(dev && at["cmd"].toBool())
        {
            if(d->point_info.isEmpty()) {
                success = tangoworld.get_command_info(d->tdev->getDevice(), at["point"].toString(), d->point_info);
            }
            if(success)
            {
                Tango::DeviceData din = tangoworld.toDeviceData(at["write_value"], d->point_info);
                bool has_argout = d->point_info["out_type"].toLongInt() != Tango::DEV_VOID;
                success = tangoworld.cmd_inout(dev, at["point"].toString(), din, has_argout, at);
            }
        }
        else if(dev && !at["cmd"].toBool()) /* attribute */
        {
            bool skip_read_attribute = true;
            if(d->point_info.isEmpty()) {
                success = tangoworld.get_att_config(d->tdev->getDevice(), at["point"].toString(), d->point_info, skip_read_attribute);
            }
            if(success)
                success = tangoworld.write_att(dev, at["point"].toString(), at["write_value"], d->point_info, at);
        }
        else
        {
            d->msg = d->tdev->getMessage();
            d->err = true;
        }
        if(dev)
        {
            d->msg = tangoworld.getLastMessage();
            d->err = tangoworld.error();
        }

    }
    // is_result flag is checked within the listener's onUpdate
    at["err"] = d->err;
    at["msg"] = d->msg;
    at["mode"] = "WRITE";
    at["is_result"] = true;
    publishResult(at);
}

void CuWriteActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    if(d->tdev)
        refcnt = d->device_service->removeRef(at["device"].toString(), threadToken());
    if(refcnt == 0)
        d->tdev = NULL;
}
