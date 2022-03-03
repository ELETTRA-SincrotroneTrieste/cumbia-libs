#ifndef TDEVICE_H
#define TDEVICE_H

#include <string>

namespace Tango {
    class DeviceProxy;
}

class TDevicePrivate;

class TDevice
{
    friend class  CuDeviceFactoryService;
public:
    Tango::DeviceProxy *getDevice() const;
    std::string getMessage() const;
    bool isValid() const;
    bool operator ==(const TDevice &other) const;
    std::string getName() const;
    int refCnt() const;
private:
    TDevicePrivate *d;

    void addRef();
    int removeRef();

    ~TDevice();
    TDevice(const std::string& name);
};

#endif // TDEVICE_H
