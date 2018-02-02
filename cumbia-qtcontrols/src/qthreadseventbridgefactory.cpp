#include "qthreadseventbridgefactory.h"
#include "qthreadseventbridge.h"

CuThreadsEventBridge_I *QThreadsEventBridgeFactory::createEventBridge() const
{
    return new QThreadsEventBridge();
}
