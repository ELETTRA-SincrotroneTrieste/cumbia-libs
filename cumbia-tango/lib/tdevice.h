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
    ~TDevice();
    TDevice(const std::string& name);
    void addRef();
    int removeRef();

    Tango::DeviceProxy *getDevice() const;
    std::string getMessage() const;
    bool isValid() const;
    bool operator ==(const TDevice &other) const;
    std::string getName() const;
    int refCnt() const;
private:
    TDevicePrivate *d;


};

#endif // TDEVICE_H
