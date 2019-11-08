#include "cuactivity.h"
#include "cuthreadinterface.h"
#include "cumacros.h"
#include "cuthreadservice.h"
#include "cuactivitymanager.h"
#include "cudata.h"

#include <assert.h>

/*! @private */
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
    CuData token, thread_tok;
    int flags, stateFlags;
};

CuActivity::CuActivity(CuActivityManager *activityManager, const CuData &token)
{
    d = new CuActivityPrivate(token);
    d->activityManager = activityManager;
    d->flags = d->stateFlags = 0;
    d->token["ptr"] = CuVariant(static_cast<void *>(this));
}

CuActivity::CuActivity(const CuData &token)
{
    d = new CuActivityPrivate(token);
    d->token["ptr"] = CuVariant(static_cast<void *>(this));
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

/*! \brief calls the pure virtual method onExit, gets the thread it belongs
 *         and calls CuThreadInterface::publishExitEvent
 *
 * \note called from the background thread.
 * \note Used internally.
 *
 * \li set the state flags to CuActivity::CuAStateOnExit
 * \li call CuActivity::onExit, implemented in subclasses
 * \li call CuActivityManager::getThread to get the thread associated to this activity
 * \li call CuThreadInterface::publishExitEvent on the CuActivity's thread
 */
void CuActivity::doOnExit()
{
    d->stateFlags |= CuActivity::CuAStateOnExit;
    onExit();
    CuThreadInterface *thread = d->activityManager->getThread(this);
    if(thread) /* may be removed while activity is in execute() */
        thread->publishExitEvent(this);
}

/*! \brief template method. Calls onExit only, without publishing exit event,
 *         because the thread is leaving its loop.
 *
 * \note Used internally
*/
void CuActivity::exitOnThreadQuit()
{
    d->stateFlags |= CuActivity::CuAStateOnExit;
    onExit();
}

/*! \brief returns the activity flags
 *
 * @return the activity flags, a combination of values from CuActivity::Flags
 *
 * @see setFlags
 *
 */
int CuActivity::getFlags() const
{
    return d->flags;
}

/*! \brief returns the activity *state* flags
 *
 * @return the activity state flags, a combination of values defined in
 *         CuActivity::StateFlags
 */
int CuActivity::getStateFlags() const
{
    return d->stateFlags;
}

/*! \brief set the flags on the activity
 *
 * @param f a combination of CuActivity::Flags <strong>or</strong>ed together.
 */
void CuActivity::setFlags(int f)
{
    d->flags = f;
}

/*! \brief set a specific CuActivity::Flags flag true or false
 *
 * @param f a flag from CuActivity::Flags
 * @param on true or false to enable/disable a flag
 */
void CuActivity::setFlag(CuActivity::Flags f, bool on)
{
    if(!on)
        d->flags = d->flags & (~f);
    else
        d->flags = d->flags | f;
}

/** \brief Publish a result from the activity thread (whence the method is called) to
 *         the main thread.
 *
 *  1. Find the thread in charge of this activity.
 *  2. If still there, ask it to post an event on the main thread with the data.
 */
void CuActivity::publishResult(const CuData &data)
{
    CuThreadInterface *thread = d->activityManager->getThread(this);
    if(thread) /* may be removed while activity is in execute() */
        thread->publishResult(this, data);
}

void CuActivity::publishResult(const std::vector<CuData> *datalist)
{
    CuThreadInterface *thread = d->activityManager->getThread(this);
    if(thread) /* may be removed while activity is in execute() */
        thread->publishResult(this, datalist);
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

/*! \brief return the activity token
 *
 * The activity token can be used to identify an activity or to gather
 * its characteristics together in a CuData bundle.
 * This *must not be confused with the thread token*, which is used to decide
 * whether a new activity is started in a new thread or an existing thread
 * with the same token is used.
 *
 * For that usage, see
 * \li CuThread::isEquivalent
 * \li \ref md_src_tutorial_cuactivity
 * \li CuActivity introductive documentation
 *
 */
CuData CuActivity::getToken() const
{
    return d->token;
}

void CuActivity::setThreadToken(const CuData &tt) {
    d->thread_tok = tt;
}

const CuData &CuActivity::threadToken() const {
    return d->thread_tok;
}

