#include "qthreadseventbridgefactory.h"
#include "qthreadseventbridge.h"

CuThreadsEventBridge_I *QThreadsEventBridgeFactory::createEventBridge(const CuServiceProvider *sp) const
{
    return new QThreadsEventBridge(sp);
}
