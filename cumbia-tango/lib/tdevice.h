#ifndef TDEVICE_H
#define TDEVICE_H

#include <string>

namespace Tango {
    class DeviceProxy;
}

class TDevicePrivate;

class TDevice
{
public:
    TDevice(const std::string& name);

    ~TDevice();

    Tango::DeviceProxy *getDevice() const;

    std::string getMessage() const;

    bool isValid() const;

    bool operator ==(const TDevice &other) const;

    std::string getName() const;

    void addRef();

    int removeRef();

    int refCnt() const;

private:
    TDevicePrivate *d;
};

#endif // TDEVICE_H
