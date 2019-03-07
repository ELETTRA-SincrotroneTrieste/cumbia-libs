#include "cudevicefactoryservice.h"
#include <mutex>
#include <cumacros.h>
#include "tdevice.h"
#include "tsource.h"

/*! \brief the class destructor
 *
 * the class destructor.
 *
 * Cumbia::finish deletes services
 */
CuDeviceFactoryService::~CuDeviceFactoryService()
{
    pdelete("~CuDeviceFactoryService deleted %p", this);
}

/** \brief Get a device for the given name.
 *
 * @param name the name of the device to find or create
 * @return a pointer to the device with the given name.
 *
 * If a device with the given name already exists, it is returned. If a device with that name
 * does not exist yet, then a new one is created and returned.
 * TDevice tries to create a Tango::DeviceProxy: check for TDevice::isValid to test for Tango errors.
 *
 * @see findDevice
 * @see TDevice::isValid
 * @see TDevice::getError
 */
TDevice *CuDeviceFactoryService::getDevice(const std::__cxx11::string &name)
{
    pr_thread();
    TDevice *td = NULL;
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<std::string, TDevice *>::iterator it = m_devmap.find(name);
    if(it != m_devmap.end())
        td = it->second;
    else
    {
        td = new TDevice(name);
        std::pair<std::string, TDevice *> p(name, td);
        m_devmap.insert(p);
    }
    return td;
}

/** \brief Find a device with the given name.
 *
 * @param name the name of the device to search
 * @return a pointer to the device with that name or NULL if a device with that name isn't found.
 *
 * @see getDevice
 */
TDevice *CuDeviceFactoryService::findDevice(const std::__cxx11::string &name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<std::string, TDevice *>::iterator it = m_devmap.find(name);
    if(it != m_devmap.end())
        return it->second;
    return NULL;
}

#include <tango.h>

/*! \brief remove the device with the given name from the list and *delete the device*
 *
 * \par note
 * The list of devices is internally stored by a map associating a device name to a TDevice.
 * The destruction of TDevice implies the destruction of the wrapped Tango::DeviceProxy.
 *
 * \par thread safety
 * The body of this method is lock guarded, so that it's safe to call removeDevice from
 * different threads.
 */
void CuDeviceFactoryService::removeDevice(const std::__cxx11::string &name)
{
    pr_thread();
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<std::string, TDevice *>::iterator it = m_devmap.find(name);
    if(it != m_devmap.end())
    {
        delete it->second;
        m_devmap.erase(it);
    }
}

/*! \brief returns a string constant that identifies the name of this service
 *
 * @return the "DeviceFactoryService" string constant
 *
 * @see getType
 *
 * Implements CuTangoActionI::getName pure virtual method
 */
std::__cxx11::string CuDeviceFactoryService::getName() const
{
    return "DeviceFactoryService";
}

/*! \brief returns the service type
 *
 * @return the value defined in cuactionfactoryservice.h  by the *enum Type*
 * (CuActionFactoryServiceType = CuServices::User + 2)
 *
 * @see Type
 *
 * Implements CuTangoActionI::getType pure virtual method
 */
CuServices::Type CuDeviceFactoryService::getType() const
{
    return static_cast<CuServices::Type> (CuDeviceFactoryServiceType);
}
