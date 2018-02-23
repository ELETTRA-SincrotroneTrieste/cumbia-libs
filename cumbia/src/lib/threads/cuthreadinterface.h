#ifndef CUTHREADINTERFACE_H
#define CUTHREADINTERFACE_H

class CuActivity;
class CuActivityEvent;
class CuThreadListener;
class CuData;
class CuThreadService;

#include "cuthreadseventbridge_i.h"

/*! \brief interface to write thread implementations that can be used with the *cumbia*
 *         library
 *
 * This interface defines the necessary functionalities required to write thread implementations
 * compatible with the *cumbia* library.
 * CuThread adheres to this interface and is the default thread implementation in cumbia.
 *
 * CuThread starts an *activity* (CuActivity), calls the CuActivity::init, CuActivity::execute
 * and CuActivity::onExit *hooks* on the activity *in a background thread (c++ 11 std::thread)*.
 * Publishes *progress* and *result* events to the *main thread* when required by the
 * *activity* (CuActivity::publishProgress and CuActivity::publishResult).
 *
 * The required methods are as follows:
 * \li registerActivity: start an activity. See CuThread::registerActivity
 * \li unregisterActivity: stop an activity and delete it, if required. See CuThread::unregisterActivity
 * \li publishProgress: deliver an event accounting for a progress on the background
 *     work (see CuThread::publishProgress)
 * \li publishResult: deliver an event accounting for a result computed in the background
 *     thread
 * \li publishExitEvent: deliver an *activity exit* event to the activity
 * \li isEquivalent: determines whether a thread is *equivalent* to another thread.
 *     *Equivalent* threads run activities in the same thread.
 * \li cleanup: clean thread objects
 * \li type: return an integer describing the type of thread (for rtti conversion, if needed)
 * \li start: start the thread (allocate a new std::thread)
 * \li isRunning: return whether a thread is running or not
 * \li wait: join the std::thread
 * \li exit: use a proper event to exit the background thread
 * \li getActivityTimerPeriod: return the period of the timer running for the given activity
 * \li postEvent: post an event from the thread to an activity
 *
 * As a CuThreadsEventBridgeListener, CuThreadInterface implementations will have to write
 * an *onEventPosted* method from which all registered CuThreadListener instances will
 * receive data (CuThreadListener::onProgress, CuThreadListener::onResult).
 * The list of CuThreadListener objects is obtained calling CuActivityManager::getThreadListeners
 *
 * \par thr_disposal Thread disposal
 * The interface does not define a specific behavior for thread disposal.
 * CuThread implementation auto destroys itself when the last activity is unregistered.
 *
 * @implements CuThreadsEventBridgeListener
 */
class CuThreadInterface : public CuThreadsEventBridgeListener
{
public:

    /*! \brief interface virtual destructor
     *
     * virtual interface destructor
     */
    virtual ~CuThreadInterface() {}

    /*! \brief register and start a new activity
     *
     * See CuThread::registerActivity for details
     */
    virtual void registerActivity(CuActivity *l) = 0;

    /*! \brief unregister and, if required, automatically delete an activity
     *
     * See CuThread::unregisterActivity for details
     */
    virtual void unregisterActivity(CuActivity *l) = 0;

    /*! \brief post a *progress* event from the background thread to the main thread
     *
     * This lets you send events to the main thread that account for an accomplished
     * step over a total number of stages foreseen in the background
     *
     * See CuThread::publishProgress for details
     */
    virtual void publishProgress(const CuActivity* activity, int step, int total, const CuData& d)  = 0;

    /*! \brief post a *result* event from the background thread to the main thread
     *
     * This lets you send events to the main thread that account for a
     * result computed in the background
     *
     * See CuThread::publishProgress for details
     */
    virtual void publishResult(const CuActivity* activity,  const CuData& d) = 0;

    /*! \brief publish an exit event from the background thread to the main thread
     *
     * The activity
     */
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

    /*! \brief perform clean operations on the class members
     *
     * delete class members
     */
    virtual void cleanup() = 0;

    /*! \brief return an integer to allow *rtti*
     *
     * @return an integer identifying the kind of thread implementation
     */
    virtual int type() const = 0;

    /*! \brief method to start the thread in the background
     *
     * starts the background thread where activities are run
     */
    virtual void start() = 0;

    /*! \brief returns true if the thread is running, false otherwise
     *
     * @return true the thread is running, false otherwise
     */
    virtual bool isRunning() = 0;

    /*! \brief wait for the thread to exit
     *
     * call join on the thread
     */
    virtual void wait() = 0;

    /*! \brief gracefully exit the thread
     *
     * exit the thread loop gracefully
     *
     * See also CuThread::exit
     */
    virtual void exit() = 0;

    /*! \brief return the period of the timer running for the given activity
     *
     * see CuThread::getActivityTimerPeriod for a more complete description
     */
    virtual int getActivityTimerPeriod(CuActivity *a) const = 0;

    /*! \brief forward an *event* to an *activity*
     *
     * This method must send an event of type CuActivityEvent to a CuActivity
     * from the
     */
    virtual void postEvent(CuActivity *, CuActivityEvent* ) = 0;


    // CuThreadsEventBridgeListener interface
public:

    /*! \brief onEventPosted from CuThreadsEventBridgeListener interface.
     */
    virtual void onEventPosted(CuEventI *event) = 0;
};

#endif // CUTHREADINTERFACE_H
