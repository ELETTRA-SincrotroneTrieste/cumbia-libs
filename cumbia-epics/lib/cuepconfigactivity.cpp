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

CuEpConfigActivity::CuEpConfigActivity(const CuData &tok, CuEpCAService *df) : CuActivity(tok) {
    d = new CuEpConfigActivityPrivate;
    d->ep_service = df;
    d->err = false;
//    setFlag(CuActivity::CuAUnregisterAfterExec, true);
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
    return CuEpConfigActivityType;
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
    CuEpicsWorld utils;
    utils.fillThreadInfo(ctrl_data, this); /* put thread and activity addresses as info */
    // utils.caget will call caget two times: one with DbrTime and the other with DbrCtrl
    // the result will be merged into the ctrl_data passed as reference
    utils.caget(ctrl_data["pv"].toString(), ctrl_data);
    publishResult(ctrl_data);
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
