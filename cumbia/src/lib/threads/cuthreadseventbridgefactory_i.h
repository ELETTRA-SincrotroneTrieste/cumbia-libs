#ifndef CUTHREADSEVENTBRIDGEFACTORY_I_H
#define CUTHREADSEVENTBRIDGEFACTORY_I_H

class CuThreadsEventBridge_I;

class CuThreadsEventBridgeFactory_I
{
public:

    virtual ~CuThreadsEventBridgeFactory_I() {}

    virtual CuThreadsEventBridge_I *createEventBridge() const = 0;
};

#endif // CUTHREADSEVENTBRIDGEFACTORY_I_H
