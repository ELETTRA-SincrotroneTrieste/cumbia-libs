#ifndef SERVICES_H
#define SERVICES_H

/** \brief This class provides an enumeration to define the available
 *         type of services available for the cumbia framework
 *
 * This class defines the type of available services for the *cumbia framework*.
 * This is the list:
 * \li Thread the thread service: CuThreadService
 * \li ActivityManager the activity manager service: CuActivityManager
 * \li EventLoop a service implementing an event loop: CuEventLoopService
 * \li Log a logging facility: CuLog
 *
 * Services are registered through the CuServiceProvider and must implement the
 * CuServiceI interface, in order to be obtained back from the service provider
 * after registration.
 * User defined services complying with the CuServiceI interface can be
 * registered.
 *
 * See also CuServiceProvider::registerService, CuServiceProvider::unregisterService
 * and CuServiceProvider::get.
 */
class CuServices
{
public:

    /*! \brief enumerates the type of available services
     *
     * User defined services can be added. In this case, their type
     * should be greater than or equal to Type::User.
     *
     * @see CuServiceProvider::registerService
     */
    enum Type { Thread = 0, ActivityManager, EventLoop, Log, User = 100 };
};

#endif // SERVICES_H
