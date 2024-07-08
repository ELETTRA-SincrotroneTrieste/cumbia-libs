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

    /*!
     * \brief get_att_config return attribute configuration properties in dres
     * \param dev a pointer to a Tango::DeviceProxy
     * \param attribute the name of the attribute
     * \param dres reference to CuData that will store the results
     * \param skip_read_att if true, do not read_attribute (TTT::Value will not be in dres)
     * \param devnam the device name, provided for convenience (to avoid call to dev->name() )
     * \return true if the call is successful, false otherwise. Results are put in dres
     */
    virtual bool get_att_config(Tango::DeviceProxy *dev, const std::string &attribute, CuData &dres, bool skip_read_att, const std::string& devnam) const = 0;
};

#endif // CUTCONFIGACTIVITYEXECUTOR_H
