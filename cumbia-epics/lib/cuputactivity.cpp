#include "cuputactivity.h"
#include "cuepactioni.h"
#include "cuepics-world.h"
#include "cuepcaservice.h"
#include <cumacros.h>

class CuPutActivityPrivate
{
public:
    CuEpCAService *epics_service;
    std::string msg;
    bool err;
    pthread_t my_thread_id, other_thread_id;
    CuData point_info; /* attribute or command info */
};

CuPutActivity::CuPutActivity(const CuData &token,
                                 CuEpCAService *df)
    : CuIsolatedActivity(token)
{
    d = new CuPutActivityPrivate;
    d->epics_service = df;
    d->err = false;
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
    setFlag(CuActivity::CuADeleteOnExit, true);
    d->other_thread_id = pthread_self();
}

CuPutActivity::~CuPutActivity()
{
    pdelete("~CuWriteActivity %p", this);
    delete d;
}

void CuPutActivity::event(CuActivityEvent *e)
{

}

bool CuPutActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"] && mytok["activity"] == token["activity"];
}

void CuPutActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    CuData tk = getToken();

}

void CuPutActivity::execute()
{
    assert(d->my_thread_id == pthread_self());
    CuData at = getToken(); /* activity token */

    /* get configuration and then write */
    printf("\e[1;32mCuPutActivity execute enter...[%s]\e[0m\n", at.toString().c_str());

    printf("\e[1;36mwould write %s into %s\e[0m\n", at["write_value"].toString().c_str(), at["src"].toString().c_str());

    d->msg = "...";
    d->err = false;
}

void CuPutActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    at["msg"] = d->msg;
    at["mode"] = "WRITE";
    at["err"] = d->err;
    CuEpicsWorld utils;
    utils.fillThreadInfo(at, this); /* put thread and activity addresses as info */
    /* remove reference to this activity ? */

    cuprintf("\e[1;31mrefcnt = %d called actionRemove for device %s att %s\e[0m\n",
           refcnt, at["device"].toString().c_str(), at["src"].toString().c_str());
    if(refcnt == 0)
    {
        // d->epics_service->remove...
    }
    at["exit"] = true;
    publishResult(at);
}
