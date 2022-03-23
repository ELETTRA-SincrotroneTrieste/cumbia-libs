#ifndef CUDEVICEFACTORY_I_H
#define CUDEVICEFACTORY_I_H

class TDevice;

#include <string>

class CuDeviceFactory_I
{
public:
    virtual ~CuDeviceFactory_I() {};

    virtual TDevice *getDevice(const std::string &name, const std::string &thread_tok) = 0;
    virtual int removeRef(const std::string& devname, const std::string &thread_tok) = 0;
};

#endif // CUDEVICEFACTORY_I_H
