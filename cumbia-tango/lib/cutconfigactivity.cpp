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
    bool exiting;
    int repeat, try_cnt;
    CuTConfigActivity::Type type;
    CuData options;
    const CuTConfigActivityExecutor_I *tcexecutor;
};

CuTConfigActivity::CuTConfigActivity(const CuData &tok, CuDeviceFactoryService *df, Type t, const CuTConfigActivityExecutor_I *tx) : CuActivity(tok)
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
    d->tcexecutor = tx;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
}

CuTConfigActivity::~CuTConfigActivity()
{
    pdelete("CuTAttConfigActivity %p [%s]", this, vtoc2(getToken(), "src"));
    delete d->tcexecutor;
    delete d;
}

void CuTConfigActivity::setOptions(const CuData &o) {
    d->options = o;
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
    const std::string& dnam = getToken()["device"].toString();
    /* get a TDevice */
    d->tdev = d->device_service->getDevice(dnam, threadToken());
    // thread safe: since cumbia 1.1.0 no thread per device guaranteed
    d->device_service->addRef(dnam, threadToken());
}

void CuTConfigActivity::execute()
{
    CuData at = getToken(); /* activity token */
    d->err = !d->tdev->isValid();
    std::string point = at["point"].toString();
    bool cmd = at["is_command"].toBool();
    at["properties"] = std::vector<std::string>();
    at["type"] = "property";
    bool value_only = false, skip_read = false;
    d->options["value-only"].to<bool>(value_only);
    d->options["no-value"].to<bool>(skip_read);

    d->try_cnt++;
    bool success = false;

    if(d->tdev->isValid()) {
        Tango::DeviceProxy *dev = d->tdev->getDevice();
        CuTangoWorld tw;
        if(dev && cmd)
        {
            success = d->tcexecutor->get_command_info(dev, point, at);
            if(success && d->type == CuReaderConfigActivityType && !skip_read) {
                success = tw.cmd_inout(dev, point, at);
            } else if(success) { // successful get_command_info but no cmd_inout
                at.putTimestamp();
            }
        }
        else if(dev)  {
            value_only ? success = tw.read_att(dev, point, at)  : success = d->tcexecutor->get_att_config(dev, point, at, skip_read);
        }
        else
            d->msg = d->tdev->getMessage();

        //
        // fetch attribute properties
        if(d->options.containsKey("fetch_props")) {
            const std::vector<std::string> &props = d->options["fetch_props"].toStringVector();
            if(props.size() > 0 && success && dev)
                success = tw.get_att_props(dev, point, at, props);
        }
        //

        at["data"] = true;
        at["msg"] = "CuTConfigActivity.execute (1): " + tw.getLastMessage();
        at["err"] = tw.error();
        d->err = !success || tw.error();
        d->msg = tw.getLastMessage();

        // retry?
        d->err ?  d->repeat = 2000 * d->try_cnt : d->repeat = -1;
    }
    else {
        at["msg"] = "CuTConfigActivity.execute (2): " + d->tdev->getMessage();
        at["err"] = true;
        at.putTimestamp();
    }
    d->exiting = true;
    d->device_service->removeRef(at["device"].toString(), threadToken());

    publishResult(at);
}

void CuTConfigActivity::onExit() { }

bool CuTConfigActivityExecutor_Default::get_command_info(Tango::DeviceProxy *dev, const std::string &cmd, CuData &cmd_info) const
{
    CuTangoWorld w;
    return w.get_command_info(dev, cmd, cmd_info);
}

bool CuTConfigActivityExecutor_Default::get_att_config(Tango::DeviceProxy *dev, const string &attribute, CuData &dres, bool skip_read_att) const
{
    CuTangoWorld w;
    return w.get_att_config(dev, attribute, dres, skip_read_att);
}
