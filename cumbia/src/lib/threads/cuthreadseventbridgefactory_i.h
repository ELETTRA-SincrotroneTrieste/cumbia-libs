#ifndef CUTHREADSEVENTBRIDGEFACTORY_I_H
#define CUTHREADSEVENTBRIDGEFACTORY_I_H

class CuThreadsEventBridge_I;
class CuServiceProvider;

class CuThreadsEventBridgeFactory_I
{
public:

    virtual ~CuThreadsEventBridgeFactory_I() {}

    virtual CuThreadsEventBridge_I *createEventBridge(const CuServiceProvider *sp) const = 0;
};

#endif // CUTHREADSEVENTBRIDGEFACTORY_I_H
