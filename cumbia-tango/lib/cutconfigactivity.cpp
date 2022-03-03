#include "cutconfigactivity.h"
#include <tango.h>
#include <cumacros.h>
#include <functional>
#include "cudevicefactoryservice.h"
#include "tdevice.h"
#include "cutango-world.h"
#include "tsource.h"

class CuTAttConfigActivityPrivate
{
public:
    CuDeviceFactoryService *device_service;
    TDevice *tdev;
    TSource ts;
    std::string msg;
    bool err;
    pthread_t my_thread_id, other_thread_id;
    bool exiting;
    int repeat, try_cnt;
    CuTConfigActivity::Type type;
    CuData options, tag;
    const CuTConfigActivityExecutor_I *tcexecutor;
};

// initialize CuActivity token with the keys relevant to the matches method
CuTConfigActivity::CuTConfigActivity(const TSource& ts,
                                     CuDeviceFactoryService *df,
                                     Type t,
                                     const CuTConfigActivityExecutor_I *tx,
                                     const CuData& o,
                                     const CuData& tag)
    : CuActivity(CuData("activity", "property").set("src", ts.getName()))
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
    d->tag = std::move(tag);
    d->options = std::move(o);
    d->ts = std::move(ts);
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
}

CuTConfigActivity::~CuTConfigActivity()
{
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

bool CuTConfigActivity::matches(const CuData &token) const {
    return token.s("src") == d->ts.getName() && "property" == token.s("activity");
}

int CuTConfigActivity::repeat() const {
    return -1;
}

void CuTConfigActivity::init()
{
    const std::string& dnam = d->ts.getDeviceName();
    /* get a TDevice */
    const std::string& tok = threadToken();
    d->tdev = d->device_service->getDevice(dnam, tok);
    // thread safe: since cumbia 1.1.0 no thread per device guaranteed
    d->device_service->addRef(dnam, tok);
}

void CuTConfigActivity::execute()
{
    d->err = !d->tdev->isValid();
//    bool value_only = d->options.containsKey("value-only") && d->options.B("value-only");
//    bool skip_read =  d->options.containsKey("no-value") && d->options.B("no-value");
    const std::string& point = d->ts.getPoint();
    CuData at("src", d->ts.getName());
    at["device"] = d->ts.getDeviceName();
    at["point"] = point;
    at["argins"] = d->ts.getArgs();
    at["activity"] = "property";
    at["is_command"] = d->ts.getType() == TSource::SrcCmd;
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
        if(dev && d->ts.getType() == TSource::SrcCmd)
        {
            success = d->tcexecutor->get_command_info(dev, point, at);
            if(success && d->type == CuReaderConfigActivityType && !skip_read) {
                success = tw.cmd_inout(dev, point, at);
            } else if(success) { // successful get_command_info but no cmd_inout
                at.putTimestamp();
            }
        }
        else if(dev)  {
            value_only ? success = tw.read_att(dev, point, at)  : success = d->tcexecutor->get_att_config(dev, point, at, skip_read, d->tdev->getName());
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
        at.merge(std::move(d->options));
        at.merge(std::move(d->tag)); // tag is carried along in results
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

bool CuTConfigActivityExecutor_Default::get_att_config(Tango::DeviceProxy *dev,
                                                       const std::string &attribute,
                                                       CuData &dres,
                                                       bool skip_read_att,
                                                       const std::string& devnam) const
{
    CuTangoWorld w;
    return w.get_att_config(dev, attribute, dres, skip_read_att);
}
