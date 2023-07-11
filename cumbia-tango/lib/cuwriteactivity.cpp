#include "cuwriteactivity.h"
#include "cudevicefactoryservice.h"
#include "tdevice.h"
#include "cutangoactioni.h"
#include "cutango-world.h"
#include "cutthread.h"

#include <cumacros.h>

class CuWriteActivityPrivate
{
public:
    CuDeviceFactory_I *devfa;
    TDevice *tdev;
    std::string msg;
    bool err;
    pthread_t my_thread_id, other_thread_id;
    CuData point_info; /* attribute or command info */
    CuData tag;
};

CuWriteActivity::CuWriteActivity(const CuData &token,
                                 CuDeviceFactory_I *df,
                                 const CuData& db_config, const CuData &tag)
    : CuActivity(token)
{
    d = new CuWriteActivityPrivate;
    d->tdev = nullptr;
    d->devfa = df;
    d->point_info = db_config;
    d->tag = tag;
    d->err = false;
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->other_thread_id = pthread_self();
}

CuWriteActivity::~CuWriteActivity()
{
    pdelete("~CuWriteActivity %p", this);
    delete d;
}

void CuWriteActivity::event(CuActivityEvent *)
{

}

bool CuWriteActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token[CuDType::Src] == mytok[CuDType::Src] && mytok[CuDType::Activity] == token[CuDType::Activity];  // mytok["activity"], token["activity"]
}

void CuWriteActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    /* get a TDevice, increasing refcnt */
    if(thread()->type() == CuTThread::CuTThreadType) // upgrade to CuTThread / lock free CuTThreadDevices
        d->devfa = static_cast<CuTThread *>(thread())->device_factory();
    d->tdev = d->devfa->getDevice(tk[CuDType::Device].toString(), threadToken());
}

void CuWriteActivity::execute()
{
    assert(d->tdev != NULL);
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    at.merge(d->tag);
    d->err = !d->tdev->isValid();

    if(d->tdev->isValid())  {  // isValid if DeviceProxy != nullptr
        Tango::DeviceProxy *dev = d->tdev->getDevice();
        CuTangoWorld tangoworld;
        bool success = !d->point_info.isEmpty();
        tangoworld.fillThreadInfo(at, this); /* put thread and activity addresses as info */
        if(dev && at[CuDType::CmdName].toBool()) {  // at["cmd"]
            if(d->point_info.isEmpty()) {
                success = tangoworld.get_command_info(d->tdev->getDevice(), at[CuDType::Point].toString(), d->point_info);
            }
            if(success)
            {
                Tango::DeviceData din = tangoworld.toDeviceData(at["write_value"], d->point_info);
                bool has_argout = d->point_info[CuDType::OutType].toLongInt() != Tango::DEV_VOID;  // point_info["out_type"]
                success = tangoworld.cmd_inout(dev, at[CuDType::Point].toString(), din, has_argout, at);
            }
        }
        else if(dev && !at[CuDType::CmdName].toBool()) { /* attribute */  // at["cmd"]
            bool skip_read_attribute = true;
            if(d->point_info.isEmpty())
                success = tangoworld.get_att_config(d->tdev->getDevice(), at[CuDType::Point].toString(), d->point_info, skip_read_attribute);
            if(success)
                success = tangoworld.write_att(dev, at[CuDType::Point].toString(), at["write_value"], d->point_info, at);
        }
        d->msg = tangoworld.getLastMessage();
        d->err = tangoworld.error();
        d->devfa->removeRef(at[CuDType::Device].toString(), threadToken());
    }
    else {
        printf("\e[1;31mCuWriteActivity dev is null getting message from d->tdev\e[0m\n");
        d->msg = d->tdev->getMessage();
        d->err = true;
    }
    // is_result flag is checked within the listener's onUpdate
    at[CuDType::Err] = d->err;
    at[CuDType::Message] = d->msg;
    at[CuDType::Mode] = "WRITE";  // at["mode"]
    at["is_result"] = true;
    at.putTimestamp();
    publishResult(at);
}

void CuWriteActivity::onExit() { }


int CuWriteActivity::getType() const {
    return CuWriteA_Type;
}

int CuWriteActivity::repeat() const {
    return 0;
}
