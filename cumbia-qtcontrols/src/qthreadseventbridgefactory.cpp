#include "qthreadseventbridgefactory.h"
#include "qthreadseventbridge.h"

/*! \brief returns a new instance of QThreadsEventBridge
 *
 * @return a new instance of  QThreadsEventBridge
 *
 */
CuThreadsEventBridge_I *QThreadsEventBridgeFactory::createEventBridge(const CuServiceProvider*) const
{
    return new QThreadsEventBridge();
}
