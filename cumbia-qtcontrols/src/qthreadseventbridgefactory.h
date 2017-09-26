#ifndef QTHREADSEVENTBRIDGEFACTORY_H
#define QTHREADSEVENTBRIDGEFACTORY_H

#include "cuthreadseventbridgefactory_i.h"

class QThreadsEventBridgeFactory : public CuThreadsEventBridgeFactory_I
{
public:

    // CuThreadsEventBridgeFactory_I interface
public:
    CuThreadsEventBridge_I *createEventBridge(const CuServiceProvider *sp) const;
};

#endif // QTHREADSEVENTBRIDGEFACTORY_H
