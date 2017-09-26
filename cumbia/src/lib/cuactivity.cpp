#include "cuactivity.h"
#include "cuthreadinterface.h"
#include "cumacros.h"
#include "cuthreadservice.h"
#include "cuactivitymanager.h"
#include "cudata.h"

#include <assert.h>

class CuActivityPrivate
{
public:
    CuActivityPrivate(const CuData& tok) : token(tok)
    {
        dispose = false;
        activityManager = NULL;
    }

    CuActivityManager *activityManager;
    bool dispose;
    bool onExit;
    const CuData token;
    int flags, stateFlags;
};

CuActivity::CuActivity(CuActivityManager *activityManager, const CuData &token)
{
    d = new CuActivityPrivate(token);
    d->activityManager = activityManager;
    d->flags = d->stateFlags = 0;
}

CuActivity::CuActivity(const CuData &token)
{
    d = new CuActivityPrivate(token);
}

void CuActivity::dispose(bool disposable)
{
    d->dispose = disposable;
}

bool CuActivity::isDisposable() const
{
    return d->dispose;
}

CuActivity::~CuActivity()
{
    pdelete("~CuActivity %p deleted from this thread 0x%lx", this, pthread_self());
    delete d;
}

void CuActivity::setActivityManager(CuActivityManager *am)
{
    d->activityManager = am;
}

void CuActivity::doInit()
{
    d->stateFlags = CuActivity::CuAStateInit;
    init();
}

void CuActivity::doExecute()
{
    d->stateFlags |= CuActivity::CuAStateExecute;
    execute();
}

void CuActivity::doOnExit()
{
    d->stateFlags |= CuActivity::CuAStateOnExit;
    onExit();
    CuThreadInterface *thread = d->activityManager->getThread(this);
    if(thread) /* may be removed while activity is in execute() */
        thread->publishExitEvent(this);
}

/* template method. Calls onExit only, without publishing exit event,
 * because the thread is leaving its loop.
*/
void CuActivity::exitOnThreadQuit()
{
    d->stateFlags |= CuActivity::CuAStateOnExit;
    onExit();
}

int CuActivity::getFlags() const
{
    return d->flags;
}

int CuActivity::getStateFlags() const
{
    return d->stateFlags;
}

void CuActivity::setFlags(int f)
{
    printf("\e[1;31m*\n*\n* setFlags called with %d\n*\n*\n*\e[0m\n", f);
    d->flags = f;
}

void CuActivity::setFlag(CuActivity::Flags f, bool on)
{
    printf("\e[0;31mCuActivity %p flags prima %d\e[0m\n", this, d->flags);
    if(!on)
        d->flags = d->flags & (~f);
    else
        d->flags = d->flags | f;
    printf("\e[0;31mCuActivity %p flags dopo %d\e[0m\n", this, d->flags);
}

/** \brief Publish a result from the activity thread (whence the method is called) to
 *         the main thread.
 *
 *  1. Find the thread in charge of this activity.
 *  2. If still there, ask it to post an event on the main thread with the data.
 */
void CuActivity::publishResult(const CuData &data)
{
    cuprintf("\e[1;33mcalling publishResult on thread from activity %p from this thread 0x%lx data %s\e[0m\n",
             this, pthread_self(), data.toString().c_str());
    CuThreadInterface *thread = d->activityManager->getThread(this);
    if(thread) /* may be removed while activity is in execute() */
        thread->publishResult(this, data);
}

/** \brief Publish a progress from the activity thread (whence the method is called) to the
 *         main thread.
 *
 * @see publishResult
 */
void CuActivity::publishProgress(int step, int total, const CuData &data)
{
    CuThreadInterface *thread = d->activityManager->getThread(this);
    cuprintf("\e[1;33mcalling publishProgress on thread %p from this thread 0x%lx\e[0m\n", thread, pthread_self());
    if(thread)
        thread->publishProgress(this, step, total, data);
}

CuActivityManager *CuActivity::getActivityManager() const
{
    return d->activityManager;
}

CuData CuActivity::getToken() const
{
    return d->token;
}

