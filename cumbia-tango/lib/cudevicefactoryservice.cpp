#include "cudevicefactoryservice.h"
#include <mutex>
#include <cumacros.h>
#include "tdevice.h"
#include "tsource.h"

CuDeviceFactoryService::CuDeviceFactoryService()
{

}

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

void CuDeviceFactoryService::removeDevice(const std::__cxx11::string &name)
{
    pr_thread();
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<std::string, TDevice *>::iterator it = m_devmap.find(name);
    if(it != m_devmap.end())
    {
        printf("CuDeviceFactoryService.removeDevice: deleting device with name \"%s\"\n", name.c_str());
        delete it->second;
        printf("CuDeviceFactoryService.deleted erasing from map\n");
        m_devmap.erase(it);
        printf("CuDeviceFactoryService.erased from map\n");
    }
}

std::__cxx11::string CuDeviceFactoryService::getName() const
{
    return "DeviceFactoryService";
}

CuServices::Type CuDeviceFactoryService::getType() const
{
    return static_cast<CuServices::Type> (CuDeviceFactoryServiceType);
}
