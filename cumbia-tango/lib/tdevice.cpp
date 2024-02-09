#include "tdevice.h"
#include "cutango-world.h"
#include <tango.h>
#include <time.h>
#include <cumacros.h>

class TDevicePrivate
{
public:
    Tango::DeviceProxy *dev;
    std::string message;
    std::string name;
    int refCnt;
};

TDevice::TDevice(const std::string &name)
{
    d = new TDevicePrivate;
    d->name = name;
    d->refCnt = 0;
    d->dev = nullptr;
    try
    {
	std::string dname = name;
        printf("\e[1;32m+ DeviceProxy:\e[0m %s\n", dname.c_str());
        d->dev = new Tango::DeviceProxy(dname);
        time_t tp;
        time(&tp);
        d->message = "connect: device defined in Tango db: " + std::string(ctime(&tp));
        d->message.pop_back();
    }
    catch(Tango::DevFailed &e)
    {
        d->message = CuTangoWorld().strerror(e.errors);
    }
}

TDevice::~TDevice()
{
    if(d->dev) {
        printf("\e[1;31m- DeviceProxy:\e[0m %s\n", d->name.c_str());
        delete d->dev;
        d->dev = nullptr;
    }
    delete d;
}

Tango::DeviceProxy *TDevice::getDevice() const
{
    return d->dev;
}

std::string TDevice::getMessage() const
{
    return d->message;
}

bool TDevice::isValid() const
{
    return d->dev != NULL;
}

bool TDevice::operator ==(const TDevice &other) const
{
    return other.d->name == d->name;
}

std::string TDevice::getName() const
{
    return d->name;
}

void TDevice::addRef()
{
    d->refCnt++;
}

int TDevice::removeRef()
{
    return --(d->refCnt);
}

int TDevice::refCnt() const
{
    return d->refCnt;
}
