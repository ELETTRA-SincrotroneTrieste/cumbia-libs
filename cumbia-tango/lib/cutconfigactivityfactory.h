#ifndef CUTCONFIGACTIVITYFACTORY_H
#define CUTCONFIGACTIVITYFACTORY_H

#include <cudata.h>

class CuDeviceFactoryService;
class CuActivity;

/*!
 * \brief Create a custom configuration activity to use with CuTConfiguration
 *
 * Reimplement the create method in a subclass to provide a custom activity for cumbia-tango
 * configuration process (access to Tango database for attribute configuration and command info)
 *
 * By default the CuTConfigActivityFactoryDefault is used by CuTConfiguration
 *
 * \since 1.4.0
 */
class CuTConfigActivityFactory {
public:
    virtual ~CuTConfigActivityFactory() {}

    virtual CuActivity *create(const CuData &tok, CuDeviceFactoryService *df, int type, const CuData& options) const = 0;
};

#endif // CUTCONFIGACTIVITYFACTORY_H
