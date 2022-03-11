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
        thread = nullptr;
        flags = 0;
        athread = pthread_self();
    }

    CuThreadInterface *thread;
    bool dispose;
    bool onExit;
    std::string thread_tok;
    const CuData token;
    int flags;

    pthread_t athread, bthread;
};

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

CuActivity::~CuActivity() {
    assert(d->athread == pthread_self());
    printf("\e[1;31m ~CuActivity %p deleted\e[0m from this thread 0x%lx\n", this, pthread_self());
    delete d;
}

void CuActivity::setThread(CuThreadInterface *thread) {
    assert(d->athread == pthread_self());
    d->thread = thread;
}

void CuActivity::doInit() {
    d->bthread = pthread_self();
    init();
}

void CuActivity::doExecute() {
    assert(d->bthread == pthread_self());
    execute();
}

void CuActivity::doOnExit() {
    assert(d->bthread == pthread_self());
    onExit();
    if(d->thread) {
        printf("[0x%lx] [activity's] CuActivity::doOnExit %p \e[0;35m calling postExitEvent \e[0m on thread %p\e[0m\n",
               pthread_self(), this, d->thread);
        d->thread->postExitEvent(this);
    }
}

void CuActivity::exitOnThreadQuit() {
    assert(d->bthread == pthread_self());
    onExit();
}

CuThreadInterface* CuActivity::thread() const {
    return d->thread;
}

/*! \brief returns the activity flags
 * @return the activity flags, a combination of values from CuActivity::Flags
 * @see setFlags
 */
int CuActivity::getFlags() const {
    return d->flags;
}

/*! \brief set the flags on the activity
 * @param f a combination of CuActivity::Flags <strong>or</strong>ed together.
 */
void CuActivity::setFlags(int f) {
    d->flags = f;
}

/*! \brief set a specific CuActivity::Flags flag true or false
 *
 * @param f a flag from CuActivity::Flags
 * @param on true or false to enable/disable a flag
 */
void CuActivity::setFlag(CuActivity::Flags f, bool on) {
    !on ? d->flags = d->flags & (~f) : d->flags = d->flags | f;
}

/** \brief Publish a result from the activity thread (whence the method is called) to
 *         the main thread.
 *  \note this method may be called even from a thread different from activity's execute.
 *        In fact, data is delivered through the event bridge / event loop.
 *        An example is cumbia-tango CuEventActivity, where publishResult is invoked from
 *        the Tango push_event callback's thread, different from the activity's.
 */
void CuActivity::publishResult(const CuData &data) {
    if(d->thread)
        d->thread->publishResult(this, data);
}

/*!
 * \brief Publish a result from the activity to the main thread. Datalist version
 * \param datalist a const reference vector of CuData
 *
 * \note see note in publishResult above
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
    if(d->thread)
        d->thread->publishProgress(this, step, total, data);
}

/*! \brief return the activity token
 *
 * The activity token can be used to identify or characterize an activity through a CuData bundle.
 * This *must not be confused with the thread token*, which is used to decide
 * whether a new activity is started in a new thread or an existing thread
 * with the same token is used.
 *
 * For that usage, see
 * \li CuThread::matches
 * \li \ref md_src_tutorial_cuactivity
 * \li CuActivity introductive documentation
 *
 * Activity token is const and set once in the class constructor, though this
 * method is lock free and safe to access from different threads
 *
 */
const CuData CuActivity::getToken() const {
    return d->token;
}

void CuActivity::setThreadToken(const std::string &tt) {
    assert(d->athread == pthread_self());
    d->thread_tok = tt;
}

const std::string CuActivity::threadToken() const {
    assert(d->bthread == pthread_self());
    return d->thread_tok;
}

