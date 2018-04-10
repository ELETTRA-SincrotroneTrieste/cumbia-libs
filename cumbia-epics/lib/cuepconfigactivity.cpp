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
    cuprintf("CuTAttConfigActivity::init: enter\n");
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();
    /* get a reference and increment */
}

void CuEpConfigActivity::execute()
{
    cuprintf("CuTAttConfigActivity::execute: enter\n");
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */
    d->err = false /* .. fill correctly */;
    std::string point = at["point"].toString();
    bool is_pv = at["is_pv"].toBool();
    at["properties"] = std::vector<std::string>();
    at["mode"] = "ATTCONF";

    if(!d->err && is_pv)
    {
        cuprintf("CuEpConfigActivity::execute: GETTING  token %s\n", at.toString().c_str());
        CuEpicsWorld utils;
        utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */


        at["data"] = true;
        at["msg"] = utils.getLastMessage();
        at["err"] = utils.error();
        d->err = utils.error();
        d->msg = utils.getLastMessage();
        publishResult(at);
    }
}

void CuEpConfigActivity::onExit()
{
    cuprintf("CuTAttConfigActivity::onExit: enter\n");
    assert(d->my_thread_id == pthread_self());
    if(d->exiting)
    {
        printf("\e[1;31mCuTAttConfigActivity::onExit onExit already called\e[0m\n!!\n");
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
