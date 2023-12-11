#ifndef CUTTHREADSEVENTBRIDGE_H
#define CUTTHREADSEVENTBRIDGE_H

#include <cuthreadseventbridgefactory_i.h>
#include <cuthreadseventbridge_i.h>
#include <cuthreadseventbridge.h>
#include <cumacros.h>
#include <cueventloop.h>
#include <cuevent.h>
#include <cuserviceprovider.h>
#include <assert.h>

class CuThreadsEventBridgeListener;

class CuTThreadsEventBridge : public CuThreadsEventBridge
{
public:
    CuTThreadsEventBridge(const CuServiceProvider *sp)
    {
        m_service_provider = sp;
        m_event_loop = static_cast<CuEventLoopService*>( sp->get(CuServices::EventLoop));
        if(!m_event_loop)
            perr("CuTThreadsEventBridge: must create a CuEventLoopService and register it as a Cumbia service before creating CuTThreadsEventBridge");
        assert(m_event_loop != NULL);
        m_event_loop->setCuEventLoopListener(this);
    }

    // CuThreadsEventBridge_I interface
    void postEvent(CuEventI *e)
    {
        /// TEST
        if(e->getType() == CuEventI::Result)
        {
            CuResultEvent *re = static_cast<CuResultEvent *>(e);
            printf("CuTThreadsEventBridge.postEvent: posting event. this thread 0x%lx.................---->\n", pthread_self());
            printf("===\nDATA from activity %p: %s\n", re->getActivity(), re->getData()[CuDType::Value].toString().c_str());
        }
        m_event_loop->postEvent(e);
    }

    void setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener *l)
    {
        m_bridgeListener = l;
    }

    // CuEventLoopListener interface
    void onEvent(CuEventI *e)
    {
        pbblue("CuTangoEventLoopListener: GOT DATAAAAAAAAAAAAAAAAAAAA YEAAAA IN WHICH THREAD?? IN THIS: \e[1;31m0x%lx\e[0m", pthread_self());
        if(e->getType() == CuEventI::Result || e->getType() == CuEventI::Progress)
        {
            CuResultEvent *re = static_cast<CuResultEvent *>(e);
            m_bridgeListener->onEventPosted(e);
        }
    }

private:
    const CuServiceProvider *m_service_provider;
    CuThreadsEventBridgeListener *m_bridgeListener;
    CuEventLoopService *m_event_loop;
};

class CuTThreadsEventBridgeFactory : public CuThreadsEventBridgeFactory_I
{
    // CuThreadsEventBridgeFactory_I interface
public:
    CuThreadsEventBridge_I *createEventBridge(const CuServiceProvider *sp) const
    {
        return new CuTThreadsEventBridge(sp);
    }
};

#endif // CUTTHREADSEVENTBRIDGE_H
