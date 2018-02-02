#ifndef QTHREADSEVENTBRIDGEFACTORY_H
#define QTHREADSEVENTBRIDGEFACTORY_H

#include "cuthreadseventbridgefactory_i.h"

/** \brief a factory that instantiates a QThreadsEventBridge instance
 *
 * \ingroup core
 *
 * The createEventBridge returns a QThreadsEventBridge, which in turn is an
 * implementation of CuThreadsEventBridge_I.
 *
 * A CuThreadsEventBridge is a *bridge* that takes an event and delivers it
 * to a secondary thread.
 *
 * See CuThreadsEventBridge_I
 */
class QThreadsEventBridgeFactory : public CuThreadsEventBridgeFactory_I
{
public:

    // CuThreadsEventBridgeFactory_I interface
public:
    CuThreadsEventBridge_I *createEventBridge() const;
};

#endif // QTHREADSEVENTBRIDGEFACTORY_H
