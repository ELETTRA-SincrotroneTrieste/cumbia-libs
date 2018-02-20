#ifndef CUTHREADSEVENTBRIDGEFACTORY_I_H
#define CUTHREADSEVENTBRIDGEFACTORY_I_H

#include <stdlib.h>

class CuServiceProvider;

class CuThreadsEventBridge_I;

/*! \brief interface for a factory to create a bridge binding a thread to an event loop
 *
 * The implementations will have to return a CuThreadsEventBridge_I instance in
 * the createEventBridge method.
 *
 */
class CuThreadsEventBridgeFactory_I
{
public:

    /*! \brief class destructor
     *
     * virtual destructor
     */
    virtual ~CuThreadsEventBridgeFactory_I() {}

    /*! \brief return an implementation of the CuThreadsEventBridge_I interface
     *
     * @param sp a pointer to the CuServiceProvider
     *
     * For example CuThreadsEventBridgeFactory::createEventBridge returns an
     * instance of a new CuThreadsEventBridge. See also CuThreadsEventBridge::CuThreadsEventBridge
     * to understand why CuServiceProvider is needed as input argument to the method.
     */
    virtual CuThreadsEventBridge_I *createEventBridge(const CuServiceProvider *sp = NULL) const = 0;
};

#endif // CUTHREADSEVENTBRIDGEFACTORY_I_H
