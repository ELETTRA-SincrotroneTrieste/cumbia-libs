#ifndef CUTHREADINTERFACE_H
#define CUTHREADINTERFACE_H

class CuActivity;
class CuActivityEvent;
class CuThreadListener;
class CuData;
class CuThreadService;

#include "cuthreadseventbridge_i.h"

class CuThreadInterface : public CuThreadsEventBridgeListener
{
public:

    virtual ~CuThreadInterface() {}

    virtual void registerActivity(CuActivity *l) = 0;

    virtual void unregisterActivity(CuActivity *l) = 0;

    virtual void publishProgress(const CuActivity* activity, int step, int total, const CuData& d)  = 0;

    virtual void publishResult(const CuActivity* activity,  const CuData& d) = 0;

    virtual void publishExitEvent(CuActivity *a)  = 0;

    /** \brief Used by the thread factory, this function, given an input data,
     *         determines whether this thread is ecuivalent to another thread
     *         of the same class for the data.
     *
     * @return true this thread is equivalent to another thread of the same implementation
     *              for the given data
     * @return false this thread is not ecuivalent to another thread dealing with other_thread_token
     *
     * @param other_thread_token generic data stored in a CuData container.
     *
     * CuThreadService uses this method to decide whether to instantiate a new thread or return an
     * existing one if the existing one can be reused according to other_thread_token.
     * For example, a string representing a network URL can be a key to determine if the same thread
     * can be reused for the same connection.
     */
    virtual bool isEquivalent(const CuData& other_thread_token) const = 0;

    virtual void cleanup() = 0;

    virtual int type() const = 0;

    virtual void start() = 0;

    virtual bool isRunning() = 0;

    virtual void wait() = 0;

    virtual void exit() = 0;

    virtual int getActivityTimerPeriod(CuActivity *a) const = 0;

    virtual void postEvent(CuActivity *, CuActivityEvent* ) = 0;


    // CuThreadsEventBridgeListener interface
public:
    virtual void onEventPosted(CuEventI *event) = 0;
};

#endif // CUTHREADINTERFACE_H
