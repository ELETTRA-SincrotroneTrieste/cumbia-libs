#ifndef CUTHREADSEVENTBRIDGE_I_H
#define CUTHREADSEVENTBRIDGE_I_H
#include <cumacros.h>
class CuEventI;

class CuThreadsEventBridgeListener
{
public:
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
    virtual ~CuThreadsEventBridge_I() { cuprintf("\e[1;31mx deleting FAMIGERATED threadseventbirds %p\e[0m\n", this); }

    virtual void postEvent(CuEventI *e) = 0;

    virtual void setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener*) = 0;

};

#endif // CUTHREADSEVENTBRIDGE_I_H
