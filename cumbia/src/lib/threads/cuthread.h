#ifndef CUTHREAD_H
#define CUTHREAD_H

#include <cuthreadinterface.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cuactivity.h>
#include <cutimerlistener.h>

class CuThreadPrivate;
class CuEventLoopService;
class CuServiceProvider;
class CuActivityEvent;

/*! \brief *cumbia thread* implementation
 *
 * CuThread offers a C++11 thread implementation that can be used with cumbia.
 * Threads in *cumbia applications* are usually instantiated and managed by the
 * CuThreadService *cumbia service*. Please read the introduction to CuThread's
 * documentation to understand how *cumbia* deals with them in an application
 * (\ref cuthread_service).
 *
 *
 *
 * @implements CuThreadInterface
 * @implements CuTimerListener
 */
class CuThread : public CuThreadInterface, public CuTimerListener
{
public:
    CuThread(const CuData &token, CuThreadsEventBridge_I *threadEventsBridge,
             const CuServiceProvider* service_provider);

    virtual ~CuThread();

    // CuThreadInterface interface
public:
    void registerActivity(CuActivity *l);
    void unregisterActivity(CuActivity *l);
    void publishProgress(const CuActivity *activity, int step, int total, const CuData &data);
    void publishResult(const CuActivity *activity, const CuData &data);
    void publishExitEvent(CuActivity *a);
    bool isEquivalent(const CuData &other_thread_token) const;
    CuData getToken() const;
    void cleanup();
    int type() const;
    void start();
    bool isRunning();
    void onTimeout(CuTimer *sender);
    void wait();
    void exit();

    void postEvent(CuActivity *a, CuActivityEvent *e);
    int getActivityTimerPeriod(CuActivity *a) const;

protected:
    virtual void run();

private:
    CuThreadPrivate *d;
    void mActivityInit(CuActivity *a);
    void mOnActivityExited(CuActivity *a);
    void mExitActivity(CuActivity *a, bool onThreadQuit);
    void mRemoveActivityTimer(CuActivity *a);
    CuTimer *mFindTimer(CuActivity *a) const;
    CuActivity *mFindActivity(CuTimer *t) const;

    // CuThreadsEventBridgeListener interface
public:
    void onEventPosted(CuEventI *event);

public:
};

#endif // CUTHREAD_H
