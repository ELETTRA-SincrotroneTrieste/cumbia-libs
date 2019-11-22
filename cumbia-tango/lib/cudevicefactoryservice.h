#ifndef DEVICEFACTORYSERVICE_H
#define DEVICEFACTORYSERVICE_H

#include <cuservicei.h>
#include <map>
#include <string>
#include <shared_mutex>
#include <cudata.h>

class TDevice;

class TDevData {
public:
    TDevData(TDevice *td, const CuData& tk) {
        tdevice = td;
        thread_token = tk;
    }

    TDevice *tdevice;
    CuData thread_token;
};

/*! \brief implements CuServiceI interface and provides a service to create and store Tango
 *         devices (wrapped by the TDevice class), find, remove (and destroy) them.
 *
 * \par note
 * This service is internally used by the library.
 *
 * CuEventActivity, CuPollingActivity, CuAttConfigActivity and CuWriteActivity are clients
 * of this service.
 *
 * \li Within the init method (e.g. CuPollingActivity::init), getDevice is used
 * to get a new device (or reuse an already existing one)
 * \li within the onExit method (e.g. CuPollingActivity::onExit), removeDevice is called
 *     remove the device from the list and *delete* it.
 *
 * findDevice can be used to find an existing device with the given name.
 *
 * getDevice, findDevice and removeDevice are lock guarded, so that it's safe to call them
 * from different threads.
 *
 * \par Remember that
 * CuActivity::init, CuActivity::execute and CuActivity::onExit
 * (CuPollingActivity, CuEventActivity and so on implement CuActivity) are executed in
 * a secondary thread. Threads in *cumbia-tango* are grouped *by device*. Different devices
 * are read and written from different threads.
 */
class CuDeviceFactoryService : public CuServiceI
{
public:
    enum Type { CuDeviceFactoryServiceType = CuServices::User + 20 };

    virtual ~CuDeviceFactoryService();

    TDevice *getDevice(const std::string &name, const CuData &thread_tok);

    TDevice *findDevice(const std::string &name, const CuData &thread_tok);

    void addRef(const std::string& devname, const CuData &thread_tok);
    int removeRef(const std::string& devname, const CuData &thread_tok);


    // CuServiceI interface
public:
    std::string getName() const;
    CuServices::Type getType() const;

private:
    std::multimap<std::string, TDevData> m_devmap;
    std::shared_mutex m_shared_mutex;
};

#endif // DEVICEFACTORYSERVICE_H
