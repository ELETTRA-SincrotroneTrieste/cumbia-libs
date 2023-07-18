#include "cuactivity.h"
#include "cuthreadinterface.h"
#include "cumacros.h"
#include "cuthreadservice.h"
#include "cuactivitymanager.h"
#include "cudata.h"

#include <assert.h>

/*! @private */
class CuActivityPrivate {
public:
    CuActivityPrivate(const CuData& tok) : token(tok) {
        thread = nullptr;
        flags = 0;
        athread = pthread_self();
    }

    CuThreadInterface *thread;
    std::string thread_tok;
    const CuData token;
    int flags;
    pthread_t athread, bthread;
};

CuActivity::CuActivity(const CuData &token) {
    d = new CuActivityPrivate(token);
}


CuActivity::~CuActivity() {
    assert(d->athread == pthread_self());
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
 *         the main thread. Single (scalar) data version
 *  \note this method may be called even from a thread different from activity's execute.
 *        In fact, data is delivered through the event bridge / event loop.
 *        An example is cumbia-tango CuEventActivity, where publishResult is invoked from
 *        the Tango push_event callback's thread, different from the activity's.
 *
 *  \see CuActivity::publishResult(const CuData *p, int siz)
 */
void CuActivity::publishResult(const CuData &data) {
    if(d->thread)
        d->thread->publishResult(this, data);
}

/*!
 * \brief publish a result as a vector of data
 * \param p pointer to a *malloc'd* vector of CuData of size siz
 * \param siz the size of data
 *
 * This version replaces cumbia 1.x *publishResult(const std::vector<CuData> &)*
 * to improve performance.
 * Where before an entire vector copy was involved, now a sole pointer
 * is exchanged between threads.
 *
 * \par Important
 * The data pointed by p shall be allocated by the user. The memory is freed by
 * the library after data is delivered to the listeners (CuThread::onEventPosted)
 *
 * \since 2.0
 */
void CuActivity::publishResult(const CuData *p, int siz) {
    if(d->thread) /* may be removed while activity is in execute() */
        d->thread->publishResult(this, p, siz);
}

/*!
 * \brief publish result, custom data version
 * \param custom data derived from CuUserData
 */
void CuActivity::publishResult(const CuUserData *data) {
    if(d->thread) d->thread->publishResult(this, data);
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
const CuData &CuActivity::getToken() const {
    return d->token;
}

void CuActivity::setThreadToken(const std::string &tt) {
    assert(d->athread == pthread_self());
    d->thread_tok = tt;
}

const std::string& CuActivity::threadToken() const {
    assert(d->bthread == pthread_self());
    return d->thread_tok;
}

