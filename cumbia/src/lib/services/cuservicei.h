#ifndef CUSERVICE_I_H
#define CUSERVICE_I_H

#include <cuservices.h>
#include <string>

/*! \brief interface to write *cumbia services* that can be registered and fetched
 *         through the *cumbia service provider*, CuServiceProvider
 *
 * Cumbia::getServiceProvider returns a reference tu the CuServiceProvider.
 * CuServiceProvider is a place where CuServiceI instances can be registered,
 * unregistered and searched.
 *
 * Every *cumbia service* must implement two methods:
 * \li getName, returns a string with the name of the service
 * \li getType, returns a value from CuServices::Type to identify the service.
 *
 * CuServices::Type is used to register, unregister and get references to service
 * from CuServiceProvider
 */
class CuServiceI
{
public:
    virtual ~CuServiceI() {}

    /*! \brief return the name of the service
     *
     * @return a std::string with the name of the service
     */
    virtual std::string getName() const = 0;

    /*! \brief return the type of the service
     *
     * @return a CuServices::Type defining the type of service
     */
    virtual CuServices::Type getType() const = 0;
};

#endif // CUSERVICEI_H
