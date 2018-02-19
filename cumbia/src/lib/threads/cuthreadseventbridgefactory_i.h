#ifndef CUTHREADSEVENTBRIDGEFACTORY_I_H
#define CUTHREADSEVENTBRIDGEFACTORY_I_H

#include <stdlib.h>

class CuServiceProvider;

class CuThreadsEventBridge_I;

class CuThreadsEventBridgeFactory_I
{
public:

    virtual ~CuThreadsEventBridgeFactory_I() {}

    virtual CuThreadsEventBridge_I *createEventBridge(const CuServiceProvider *sp = NULL) const = 0;
};

#endif // CUTHREADSEVENTBRIDGEFACTORY_I_H
