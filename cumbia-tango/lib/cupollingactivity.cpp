#include "cupollingactivity.h"
#include "cudevicefactoryservice.h"
#include "tdevice.h"
#include "tsource.h"
#include "cutango-world.h"
#include <tango.h>
#include <cumacros.h>

class CuPollingActivityPrivate
{
public:
    CuDeviceFactoryService *device_srvc;
    TDevice *tdev;
    int repeat, errCnt;
    std::string message;
    pthread_t my_thread_id, other_thread_id;
    CuVariant argins;
    CuData point_info;
    bool exiting;
};

CuPollingActivity::CuPollingActivity(const CuData &token,
                                     CuDeviceFactoryService *df,
                                     const CuVariant & argins)
    : CuContinuousActivity(token)
{
    d = new CuPollingActivityPrivate;
    d->device_srvc = df;
    d->repeat = 1000;
    d->errCnt = 0;
    d->other_thread_id = pthread_self();
    d->argins = argins;
    d->exiting = false;

    int period = 1000;
    if(token.containsKey("period"))
        period = token["period"].toInt();
    d->repeat = period;
    setInterval(period);
}

CuPollingActivity::~CuPollingActivity()
{
    delete d;
}

void CuPollingActivity::setArgins(const CuVariant &argins)
{
    d->argins = argins;
}

bool CuPollingActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

void CuPollingActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    /* get a reference to TDevice */
    d->tdev = d->device_srvc->getDevice(tk["device"].toString());
    /* if polling activity is a fallback because event subscription fails, no need to add ref */
  //  if(!tk["fallback"].toBool())
    d->tdev->addRef();
}

void CuPollingActivity::execute()
{
    assert(d->tdev != NULL);
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    std::string devnam = at["device"].toString();
    std::string point = at["point"].toString();
    bool is_command = at["is_command"].toBool();
    Tango::DeviceProxy *dev = d->tdev->getDevice();
    CuTangoWorld tangoworld;
    tangoworld.fillThreadInfo(at, this); /* put thread and activity addresses as info */
    at["err"] = d->tdev->isValid();
    at["mode"] = "POLLED";
    bool success = false;
    if(dev && !is_command)
    {
        success = tangoworld.read_att(dev, point, at);
    }
    else if(dev) /* command */
    {
        if(d->point_info.isEmpty())
            success = tangoworld.get_command_info(d->tdev->getDevice(), at["point"].toString(), d->point_info);
        if(d->point_info.isEmpty())
            d->errCnt++;
        else
        {
            success = tangoworld.cmd_inout(dev, point, d->argins, d->point_info, at);
        }
    }

    at["msg"] = tangoworld.getLastMessage();
    at["err"] = tangoworld.error();

    if(dev && success)
    {
        d->repeat = getTimeout();
        d->errCnt = 0;
    }
    else if(dev)
    {
        ++(d->errCnt) > 0 && d->errCnt < 3 ? d->repeat = 5000 : d->repeat = 10000;
        pbred("failed to read attribute for \"%s/%s\": \"%s\" ---> decreasing timeout to %d cuz errcnt is %d",
              devnam.c_str(), point.c_str(), at["msg"].toString().c_str(), d->repeat, d->errCnt);
    }
    else
    {
        at["msg"] = d->tdev->getMessage();
    }
    publishResult(at);
}

void CuPollingActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    d->exiting = true;
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    at["msg"] = "EXITED";
    at["mode"] = "POLLED";
    CuTangoWorld utils;
    utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
    if(d->tdev)
        refcnt = d->tdev->removeRef();
    printf("\e[1;31mCuPollingActivity::onExit(): refcnt = %d called actionRemove for device %s att %s\e[0m\n",
           refcnt, at["device"].toString().c_str(), at["src"].toString().c_str());
    if(refcnt == 0)
    {
        d->device_srvc->removeDevice(at["device"].toString());
        d->tdev = NULL;
    }
    at["exit"] = true;
    publishResult(at);
}

void CuPollingActivity::event(CuActivityEvent *e)
{
    assert(d->my_thread_id == pthread_self());
    CuContinuousActivity::event(e);
}

int CuPollingActivity::getType() const
{
    return CuPollingActivityType;
}

int CuPollingActivity::repeat() const
{
    assert(d->my_thread_id == pthread_self());
    return d->repeat;
}
