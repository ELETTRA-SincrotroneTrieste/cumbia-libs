#ifndef CUTHREADSEVENTBRIDGE_I_H
#define CUTHREADSEVENTBRIDGE_I_H
#include <cumacros.h>
class CuEventI;

class CuThreadsEventBridgeListener
{
public:
    virtual void onEventPosted(CuEventI *e) = 0;
};

class CuThreadsEventBridge_I
{
public:
    virtual ~CuThreadsEventBridge_I() { cuprintf("\e[1;31mx deleting FAMIGERATED threadseventbirds %p\e[0m\n", this); }

    virtual void postEvent(CuEventI *e) = 0;

    virtual void setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener*) = 0;

};

#endif // CUTHREADSEVENTBRIDGE_I_H
