#include "cuepconfigactivity.h"
#include <cumacros.h>
#include "cuepics-world.h"

class CuEpConfigActivityPrivate
{
public:
    CuEpCAService *ep_service;
    std::string msg;
    bool err;
    pthread_t my_thread_id, other_thread_id;
    std::vector<std::string> props;
    bool exiting;
};

CuEpConfigActivity::CuEpConfigActivity(const CuData &tok, CuEpCAService *df) : CuIsolatedActivity(tok)
{
    d = new CuEpConfigActivityPrivate;
    d->ep_service = df;
    d->err = false;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->other_thread_id = pthread_self();
    d->exiting = false;
}

void CuEpConfigActivity::setDesiredAttributeProperties(const std::vector<std::string> &props)
{
    d->props = props;
}

CuEpConfigActivity::~CuEpConfigActivity()
{
    delete d;
}

int CuEpConfigActivity::getType() const
{
    return CuAttConfigActivityType;
}

void CuEpConfigActivity::event(CuActivityEvent *e)
{
    (void )e;
}

bool CuEpConfigActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

int CuEpConfigActivity::repeat() const
{
    return -1;
}

void CuEpConfigActivity::init()
{
    cuprintf("CuEpConfigActivity::init: enter\n");
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    /* get a reference and increment */
}

void CuEpConfigActivity::execute()
{
    cuprintf("CuEpConfigActivity::execute: enter\n");
    assert(d->my_thread_id == pthread_self());
    CuData ctrl_data = getToken(); /* activity token */
    ctrl_data["properties"] = std::vector<std::string>();
    ctrl_data["mode"] = "caget";
    CuData value_data(ctrl_data);

    cuprintf("CuEpConfigActivity::execute: GETTING  token %s\n", ctrl_data.toString().c_str());
    CuEpicsWorld utils;
    utils.fillThreadInfo(ctrl_data, this); /* put thread and activity addresses as info */

    utils.caget(ctrl_data["pv"].toString(), ctrl_data, value_data);

    printf("\e[0;33mCuEpConfigActivity.execute: \n%s:\n got \e[0m%s \n", ctrl_data["pv"].toString().c_str(), ctrl_data.toString().c_str());
    publishResult(ctrl_data);
    printf("publishResult called with %s\n", ctrl_data.toString().c_str());
    publishResult(value_data);
    printf("publishResult called with %s\n", value_data.toString().c_str());
}

void CuEpConfigActivity::onExit()
{
    cuprintf("CuEpConfigActivity::onExit: enter\n");
    assert(d->my_thread_id == pthread_self());
    if(d->exiting)
    {
        printf("\e[1;31mCuEpConfigActivity::onExit onExit already called\e[0m\n!!\n");
        return;
    }
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    at["msg"] = d->msg;
    at["mode"] = "ATTCONF";
    at["err"] = d->err;
    CuEpicsWorld utils;
    utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
    /* remove ref */

    if(refcnt == 0)
    {
        //  d->device_service->removeDevice(at["device"].toString());
    }
    at["exit"] = true;
    publishResult(at);
}
