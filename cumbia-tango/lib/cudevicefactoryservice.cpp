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
TDevice *CuDeviceFactoryService::getDevice(const std::string &name, const CuData &thread_tok)
{
    pr_thread();
    TDevice *td = NULL;
    std::lock_guard<std::mutex> lock(m_mutex);
    std::pair<std::multimap <std::string, TDevData>::const_iterator, std::multimap <std::string, TDevData>::iterator > ret;
    ret = m_devmap.equal_range(name);
    for(std::multimap<std::string, TDevData>::const_iterator it = ret.first; !td && it != ret.second; ++it) {
        if(it->second.thread_token == thread_tok)
            td = it->second.tdevice;
    }
    if(!td)
    {
        td = new TDevice(name);
        TDevData devd(td, thread_tok);
        std::pair<std::string, TDevData > p(name, devd);
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
TDevice *CuDeviceFactoryService::findDevice(const std::string &name, const CuData& thread_tok)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::pair<std::multimap <std::string, TDevData>::const_iterator, std::multimap <std::string, TDevData>::iterator > ret;
    ret = m_devmap.equal_range(name);
    for(std::multimap<std::string, TDevData>::const_iterator it = ret.first; it != ret.second; ++it) {
        if(it->second.thread_token == thread_tok)
            return it->second.tdevice;
    }
    return NULL;
}

void CuDeviceFactoryService::addRef(const string &devname, const CuData &thread_tok)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::pair<std::multimap <std::string, TDevData>::const_iterator, std::multimap <std::string, TDevData>::iterator > ret;
    ret = m_devmap.equal_range(devname);
    for(std::multimap<std::string, TDevData>::const_iterator it = ret.first; it != ret.second; ++it) {
        if(it->second.thread_token == thread_tok) {
            it->second.tdevice->addRef();
            break;
        }
    }
}

int CuDeviceFactoryService::removeRef(const string &devname, const CuData &thread_tok)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    int refcnt = -1;
    std::multimap< std::string, TDevData>::iterator it = m_devmap.begin();
    while(it != m_devmap.end())
    {
        if(it->first == devname && it->second.thread_token == thread_tok) {
            TDevice *d = it->second.tdevice;
            refcnt = d->removeRef();
            if(refcnt == 0) { // no more references for that device in that thread
                delete d;
                it = m_devmap.erase(it);
            }
            break; // removeRef once!
        }
        else
            ++it;
    }
    return refcnt;
}


/*! \brief returns a string constant that identifies the name of this service
 *
 * @return the "DeviceFactoryService" string constant
 *
 * @see getType
 *
 * Implements CuTangoActionI::getName pure virtual method
 */
std::string CuDeviceFactoryService::getName() const
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
