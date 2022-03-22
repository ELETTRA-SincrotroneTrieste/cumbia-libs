#ifndef CUTTHREADDEVICES_H
#define CUTTHREADDEVICES_H

#include "cudevicefactory_i.h"

class CuTThreadDevicesPrivate;

// lock free version of CuDeviceFactoryService meant to be used
// within the same thread, in combination with CuTThread
//
class CuTThreadDevices : public CuDeviceFactory_I
{
public:
    CuTThreadDevices();
    ~CuTThreadDevices();

    // CuDeviceFactory_I interface
public:
    TDevice *getDevice(const std::string &name, const std::string &thread_tok);
    int removeRef(const std::string &devname, const std::string &thread_tok);

private:
    CuTThreadDevicesPrivate *d;
};

#endif // CUTTHREADDEVICES_H
