#ifndef CUTCONFIGACTIVITYEXECUTOR_H
#define CUTCONFIGACTIVITYEXECUTOR_H

#include <cudata.h>

class CuDeviceFactoryService;
class CuActivity;

namespace Tango {
class DeviceProxy;
}

class CuTConfigActivityExecutor_I {
public:
    virtual ~CuTConfigActivityExecutor_I() {}
    virtual bool get_command_info(Tango::DeviceProxy *dev, const std::string& cmd, CuData &cmd_info)  const = 0;
    virtual bool get_att_config(Tango::DeviceProxy *dev, const std::string &attribute, CuData &dres, bool skip_read_att) const = 0;
};

#endif // CUTCONFIGACTIVITYEXECUTOR_H
