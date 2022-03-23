#include "cutconfigactivity.h"
#include <tango.h>
#include <cumacros.h>
#include <functional>
#include "cudevicefactoryservice.h"
#include "tdevice.h"
#include "cutango-world.h"
#include "tsource.h"
#include "cutthread.h"

class CuTAttConfigActivityPrivate
{
public:
    CuDeviceFactory_I *devfa;
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
                                     CuDeviceFactory_I *df,
                                     Type t,
                                     const CuTConfigActivityExecutor_I *tx,
                                     const CuData& o,
                                     const CuData& tag)
    : CuActivity(CuData("activity", "property").set("src", ts.getName()))
{
    d = new CuTAttConfigActivityPrivate;
    d->devfa = df;
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

void CuTConfigActivity::init() {
    // get device, new or recycled. getDevice increases refcnt
    // after register activity, we have been assigned to a thread
    if(thread()->type() == CuTThread::CuTThreadType) // upgrade to CuTThread / lock free CuTThreadDevices
        d->devfa = static_cast<CuTThread *>(thread())->device_factory();
    d->tdev = d->devfa->getDevice(d->ts.getDeviceName(), threadToken());
}

void CuTConfigActivity::execute() {
    const CuData o(d->options); // thread local copy
    const CuData tag(d->tag);    // thread local copy
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
    o["value-only"].to<bool>(value_only);
    o["no-value"].to<bool>(skip_read);

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
        if(o.containsKey("fetch_props")) {
            const std::vector<std::string> &props = o["fetch_props"].toStringVector();
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
    d->devfa->removeRef(at["device"].toString(), threadToken());
    at.merge(std::move(o));
    at.merge(std::move(tag));
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
