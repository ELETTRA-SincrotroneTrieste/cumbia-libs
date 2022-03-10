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
        thread = NULL;
        flags = 0;
    }

    CuThreadInterface *thread;
    bool dispose;
    bool onExit;
    std::string thread_tok;
    CuData token;
    int flags;
};

CuActivity::CuActivity(CuThreadInterface *activityManager, const CuData &token)
{
    d = new CuActivityPrivate(token);
    d->thread = activityManager;
}

CuActivity::CuActivity(const CuData &token)
{
    d = new CuActivityPrivate(token);
}

void CuActivity::dispose(bool disposable) {
    d->dispose = disposable;
}

bool CuActivity::isDisposable() const {
    return d->dispose;
}

CuActivity::~CuActivity()
{
    pdelete("~CuActivity %p deleted from this thread 0x%lx", this, pthread_self());
    delete d;
}

void CuActivity::setThread(CuThreadInterface *thread) {
    d->thread = thread;
}

void CuActivity::doInit() {
    init();
}

void CuActivity::doExecute() {
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
void CuActivity::doOnExit() {
    onExit();
    if(d->thread)
        d->thread->publishExitEvent(this);
}

/*! \brief template method. Calls onExit only, without publishing exit event,
 *         because the thread is leaving its loop.
 *
 * \note Used internally
*/
void CuActivity::exitOnThreadQuit() {
    onExit();
}

/*! \brief returns the activity flags
 *
 * @return the activity flags, a combination of values from CuActivity::Flags
 *
 * @see setFlags
 *
 */
int CuActivity::getFlags() const {
    return d->flags;
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
void CuActivity::publishResult(const CuData &data) {
    if(d->thread)
        d->thread->publishResult(this, data);
}


/*!
 * \brief Publish a result from the activity to the main thread. Datalist version
 * \param datalist a *heap allocated* vector of CuData
 *
 * \note datalist is deleted by the library when no more needed.
 */
void CuActivity::publishResult(const std::vector<CuData> &datalist) {
    if(d->thread) /* may be removed while activity is in execute() */
        d->thread->publishResult(this, datalist);
}

/** \brief Publish a progress from the activity thread (whence the method is called) to the
 *         main thread.
 *
 * @see publishResult
 */
void CuActivity::publishProgress(int step, int total, const CuData &data) {
    cuprintf("\e[1;33mcalling publishProgress on thread %p from this thread 0x%lx\e[0m\n", thread, pthread_self());
    if(d->thread)
        d->thread->publishProgress(this, step, total, data);
}

/*!
 * \brief Returns the reference to the activity manager
 * \return a pointer to the CuActivityManager used by this instance
 */
CuActivityManager *CuActivity::getActivityManager() const
{
    return d->thread;
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
CuData CuActivity::getToken() const {
    return d->token;
}

void CuActivity::setThreadToken(const std::string &tt) {
    d->thread_tok = tt;
}

const std::string CuActivity::threadToken() const {
    return d->thread_tok;
}

