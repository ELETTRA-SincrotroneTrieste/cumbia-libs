#ifndef CUTHREAD_H
#define CUTHREAD_H

#include <cuthreadinterface.h>
#include <thread>
#include <mutex>
#include <list>
#include <condition_variable>
#include <cuactivity.h>
#include <cutimerlistener.h>

class CuThreadPrivate;
class CuEventLoopService;
class CuServiceProvider;
class CuActivityEvent;
class CuTimerService;

/*! \brief *cumbia thread* implementation
 *
 * CuThread offers a C++11 thread implementation that can be used with cumbia.
 * Threads in *cumbia applications* are usually instantiated and managed by the
 * CuThreadService *cumbia service*. Please read the introduction to CuThreadService
 * documentation to understand how *cumbia* deals with them in an application
 * (\ref cuthread_service).
 *
 * \par Registering and unregistering activities
 * One or more *activities* are registered to the thread with CuThread::registerActivity.
 * Usually, CuThread::registerActivity is not called by the clients. Clients should rely
 * on Cumbia::registerActivity instead. The same goes for CuThread::unregisterActivity
 * and Cumbia::unregisterActivity.
 *
 * \note
 * CuThreads are used internally by the library. Clients should not directly deal with
 * them. They would rather write *activities*.
 *
 * \par Publishing results from the background to the foreground
 * CuThread::publishProgress and CuThread::publishResult, invoked in CuThread's thread,
 * post a *progress event* and a *result event* to the main thread, respectively.
 * They are called by clients through CuActivity::publishProgress and CuActivity::publishResult,
 * normally from the *init, execute* and *onExit* hooks (see CuActivity documentation)
 *
 * \par Thread management
 * The CuThread::isEquivalent method decides whether *this thread's token* is *equivalent*
 * to another *token*. If so, *Cumbia* assigns a new activity to this thread, otherwise
 * a new thread is created for the new activity (see also Cumbia::registerActivity,
 * \ref cuthread_service and CuThreadService::getThread ).
 * The thread token can be obtained by CuThread::getToken. A token is assigned in the
 * CuThread constructor and cannot be changed later.
 * The CuThread::start method instantiates a new C++11 std::thread.
 * To gracefully *terminate* a CuThread, CuThread::exit must be called, followed by
 * CuThread::wait, as done inside Cumbia::finish method.
 *
 * \par Thread disposal
 * When the *last activity* exits (there are no more activities registered with *this thread*),
 * the thread leaves its *run loop*, joins and *auto destroys itself*.
 *
 * \par See also
 * Please read the documentation of the CuThreadInterface for more details.
 *
 *
 * @implements CuThreadInterface
 * @implements CuTimerListener
 */
class CuThread : public CuThreadInterface, public CuTimerListener
{
public:
    enum Type { CuThreadType = 0 };

    CuThread(const std::string &token,
             CuThreadsEventBridge_I *threadEventsBridge,
             const CuServiceProvider *sp,
             std::vector<CuThreadInterface *> *thv_p);

    virtual ~CuThread();

    // CuThreadInterface interface
public:
    void registerActivity(CuActivity *l, CuThreadListener *tl);
    void unregisterActivity(CuActivity *l);
    void publishProgress(const CuActivity *activity, int step, int total, const CuData &data);
    void publishResult(const CuActivity *activity, const CuData &data);
    void publishResult(const CuActivity *activity, const CuData* p_d, int siz);
    void publishResult(const CuActivity *activity, const CuUserData *u);


    unsigned activityCount() const;

    bool matches(const std::string &other_thtok) const;
    std::string getToken() const;
    void cleanup();
    int type() const;
    void start();
    bool isRunning();
    void onTimeout(CuTimer *sender);
    void wait();
    void exit();
    void postEvent(CuActivity *a, CuActivityEvent *e);

public:
    void onEventPosted(CuEventI *event);

protected:
    virtual void run();

private:
    CuThreadPrivate *d;

    void mOnActivityExited(CuActivity *a);
    void m_zero_activities();
    void m_activity_disconnect(CuActivity *a);
};

#endif // CUTHREAD_H
