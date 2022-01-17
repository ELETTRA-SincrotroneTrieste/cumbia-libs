#ifndef CUTATTCONFIGURATION_H
#define CUTATTCONFIGURATION_H

#include "cutangoactioni.h"

class CuTAttConfigurationPrivate;
class CuTConfigActivityFactory;

/*!
 * \brief Get command or attribute configuration from the Tango database.
 *
 * This action performs attribute info and command info database fetch.
 * Optionally, if configured with the option *value-only*, either a *one shot*
 * read attribute or command inout are performed, with no database access.
 *
 * Once the operation completes, both the associated activity and CuTConfiguration
 * instances are deleted.
 *
 * \par Configuration options
 * \li *fetch_props* (vector of std::string): executes Tango database *get_device_attribute_property* for the list
 *     of supplied properties
 * \li *value-only* do not actually neither call *get_attribute_config* on the Tango Device nor *command_query*.
 *     Instead, just read the attribute value or get the output from *command_inout*
 *
 *
 */
class CuTConfiguration : public CuTangoActionI
{
public:

    CuTConfiguration(const TSource& src,
                     CumbiaTango *ct,
                     CuTangoActionI::Type t,
                     const CuData &options,
                     const CuData &tag);

    virtual ~CuTConfiguration();

    void setDesiredAttributeProperties(const std::vector<std::string> props);
    void setOptions(const CuData& options);
    void setTag(const CuData& tag);
    void setConfigActivityFactory(const CuTConfigActivityFactory* caf);

    // CuThreadListener interface
public:
    void onProgress(int, int, const CuData &);
    void onResult(const CuData &data);
    void onResult(const std::vector<CuData> &datalist);

    CuData getToken() const;

    // CuTangoActionI interface
public:
    TSource getSource() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    void sendData(const CuData& data);
    void getData(CuData& d_inout) const;
    size_t dataListenersCount();
    void start();
    void stop();
    bool exiting() const;

private:
    CuTAttConfigurationPrivate *d;
};

#endif // CUTATTCONFIGURATION_H
