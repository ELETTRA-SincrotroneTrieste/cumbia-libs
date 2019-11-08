#ifndef CUTATTCONFIGURATION_H
#define CUTATTCONFIGURATION_H

#include "cutangoactioni.h"

class CuTAttConfigurationPrivate;

class CuTConfiguration : public CuTangoActionI
{
public:

    CuTConfiguration(const TSource& src,
                        CumbiaTango *ct,
                     CuTangoActionI::Type t);

    virtual ~CuTConfiguration();

    void setDesiredAttributeProperties(const std::vector<std::string> props);

    void setOptions(const CuData& options);

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
