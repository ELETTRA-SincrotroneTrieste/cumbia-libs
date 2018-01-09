#include "cuthreadseventbridge.h"
#include <cuthreadseventbridgefactory_i.h>
#include <cuthreadseventbridge_i.h>
#include <cumacros.h>
#include <cueventloop.h>
#include <cuevent.h>
#include <cuserviceprovider.h>
#include <assert.h>

CuThreadsEventBridge::CuThreadsEventBridge(const CuServiceProvider *sp)
{
    m_service_provider = sp;
    m_event_loop = static_cast<CuEventLoopService*>( sp->get(CuServices::EventLoop));
    if(!m_event_loop)
        perr("CuTThreadsEventBridge: must create a CuEventLoopService and register it as a Cumbia service before creating CuTThreadsEventBridge");
    assert(m_event_loop != NULL);
    m_event_loop->setCuEventLoopListener(this);
}

void CuThreadsEventBridge::postEvent(CuEventI *e)
{
    m_event_loop->postEvent(e);
}

void CuThreadsEventBridge::setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener *l)
{
    m_bridgeListener = l;
}

void CuThreadsEventBridge::onEvent(CuEventI *e)
{
    m_bridgeListener->onEventPosted(e);
}

CuThreadsEventBridge_I *CuThreadsEventBridgeFactory::createEventBridge(const CuServiceProvider *sp) const
{
    return new CuThreadsEventBridge(sp);
}
