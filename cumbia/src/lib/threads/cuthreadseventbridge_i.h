#ifndef CUTHREADSEVENTBRIDGE_I_H
#define CUTHREADSEVENTBRIDGE_I_H
#include <cumacros.h>
class CuEventI;

/*! \brief defines a method that must be written by the interface implementations
 *         in order to deal with events sent from the *background thread* and
 *         delivered to the *main thread*
 *
 * The pure virtual method onEventPosted si a *callback* that must be implemented
 * by subclasses
 *
 * \note Interface hierarchy
 * CuThreadsEventBridgeListener <-- CuThreadInterface <-- CuThread
 */
class CuThreadsEventBridgeListener
{
public:
    /*! \brief callback on the *main thread* containing an event proceeding from
     *         the *background* thread
     *
     * @param e an instance of a CuEventI
     *
     * See CuThread::onEventPosted for an example and for a more complete description
     * of the method usage
     */
    virtual void onEventPosted(CuEventI *e) = 0;
};

/*! \brief interface to a threads event bridge
 *
 * This interface defines two methods that must be implemented by a threads
 * event bridge:
 *
 * \li postEvent, used to deliver an event from a background thread to the
 *     application event loop
 * \li setCuThreadsEventBridgeListener: set the CuThreadsEventBridgeListener
 *     on the CuThreadsEventBridge_I
 *
 * \par CuThreadsEventBridgeListener
 * This interface is implemented by the CuThreadInterface and then CuThread.
 * CuThread::onEventPosted receives the event in the main thread and calls
 * CuThreadListener::onProgress or CuThreadListener::onResult on the thread
 * listeners associated to the activity that generated the event.
 * Remember that Cumbia::registerActivity associates an activity (CuActivity)
 * to a thread listener (CuThreadListener).
 *
 *
 *
 */
class CuThreadsEventBridge_I
{
public:

    /*! \brief class destructor
     *
     * virtual class destructor
     */
    virtual ~CuThreadsEventBridge_I() { cuprintf("\e[1;31mx deleting FAMIGERATED threadseventbirds %p\e[0m\n", this); }

    /*! \brief bridge's method to forward an event from the background thread to the
     *         main thread.
     *
     * A *thread event bridge* provides an interface to deliver events from a
     * background thread to an event loop running in another thread.
     *
     * \par Example implementations:
     * \li CuThreadsEventBridge
     * \li QThreadsEventBridge (cumbia-qtcontrols module, used for Qt applications)
     *
     */
    virtual void postEvent(CuEventI *e) = 0;

    /*! \brief set a CuThreadsEventBridgeListener for this CuThreadsEventBridge_I
     *
     * @param l a CuThreadsEventBridgeListener
     *
     * \par CuThreadsEventBridgeListener
     * CuThreadInterface *is* a CuThreadsEventBridgeListener
     * CuThread *is* a CuThreadInterface and so *it is also* a CuThreadsEventBridgeListener
     *
     * Please read CuThread::onEventPosted for more details
     */
    virtual void setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener* l) = 0;

};

#endif // CUTHREADSEVENTBRIDGE_I_H
