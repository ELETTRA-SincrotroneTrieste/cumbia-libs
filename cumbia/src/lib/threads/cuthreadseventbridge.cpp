#include "cuthreadseventbridge.h"
#include <cuthreadseventbridgefactory_i.h>
#include <cuthreadseventbridge_i.h>
#include <cumacros.h>
#include <cueventloop.h>
#include <cuevent.h>
#include <cuserviceprovider.h>
#include <assert.h>

/*! \brief the class constructor
 *
 * @param sp the CuServiceProvider that is used to get the event loop implementation
 *        offered by the *cumbia* base module
 *
 * A CuEventLoopService must have been previously created and registered to the CuServiceProvider
 */
CuThreadsEventBridge::CuThreadsEventBridge(const CuServiceProvider *sp)
{
    m_service_provider = sp;
    m_event_loop = static_cast<CuEventLoopService*>( sp->get(CuServices::EventLoop));
    if(!m_event_loop)
        perr("CuTThreadsEventBridge: must create a CuEventLoopService and register it as a Cumbia service before creating CuTThreadsEventBridge");
    assert(m_event_loop != NULL);
    m_event_loop->setCuEventLoopListener(this);
}

/*! \brief post the event to the event loop
 *
 * calls postEvent on the event loop service (CuEventLoopService)
 */
void CuThreadsEventBridge::postEvent(CuEventI *e)
{
    m_event_loop->postEvent(e);
}

/*! \brief sets the CuThreadsEventBridgeListener on this event bridge
 *
 * @param l an instance of CuThreadsEventBridgeListener
 */
void CuThreadsEventBridge::setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener *l)
{
    m_bridgeListener = l;
}

/*! \brief invokes onEventPosted on the bridge listener
 *
 * @param e an implementation of CuEventI
 *
 * The method simply calls CuThreadsEventBridgeListener::onEventPosted.
 * For example, CuThread implements CuThreadsEventBridgeListener through
 * CuThreadInterface, that in turn inherits from CuThreadsEventBridgeListener.
 * CuThread::onEventPosted gets the activity that generated the event
 * and the thread listener associated to that activity (when
 * Cumbia::registerActivity had been called)
 * CuThreadListener::onProgress and CuThreadListener::onResult are
 * called. See CuThread::onEventPosted
 */
void CuThreadsEventBridge::onEvent(CuEventI *e)
{
    m_bridgeListener->onEventPosted(e);
}

/*! \brief returns a new instance of CuThreadsEventBridge
 *
 * @param sp the CuServiceProvider
 * @return a new instance of CuThreadsEventBridge
 *
 */
CuThreadsEventBridge_I *CuThreadsEventBridgeFactory::createEventBridge(const CuServiceProvider *sp) const
{
    return new CuThreadsEventBridge(sp);
}

