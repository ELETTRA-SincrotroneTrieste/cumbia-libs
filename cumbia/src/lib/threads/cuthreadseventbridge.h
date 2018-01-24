#ifndef CUTHREADSEVENTBRIDGE_H
#define CUTHREADSEVENTBRIDGE_H

#include <cuthreadseventbridgefactory_i.h>
#include <cuthreadseventbridge_i.h>
#include <cumacros.h>
#include <cueventloop.h>

class CuThreadsEventBridgeListener;
class CuServiceProvider;
class CuEventI;

class CuThreadsEventBridge : public CuThreadsEventBridge_I, public CuEventLoopListener
{
public:
    CuThreadsEventBridge(const CuServiceProvider *sp);

    // CuThreadsEventBridge_I interface
    void postEvent(CuEventI *e);

    void setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener *l);

    // CuEventLoopListener interface
    void onEvent(CuEventI *e);

private:
    const CuServiceProvider *m_service_provider;
    CuThreadsEventBridgeListener *m_bridgeListener;
    CuEventLoopService *m_event_loop;
};

class CuThreadsEventBridgeFactory : public CuThreadsEventBridgeFactory_I
{
    // CuThreadsEventBridgeFactory_I interface
public:
    CuThreadsEventBridge_I *createEventBridge(const CuServiceProvider *sp) const;
};

#endif // CUTTHREADSEVENTBRIDGE_H
