#ifndef CUTATTCONFIGURATION_H
#define CUTATTCONFIGURATION_H

#include <cuepactioni.h>

class CuTAttConfigurationPrivate;

class CuEpConfiguration : public CuEpicsActionI
{
public:

    CuEpConfiguration(const EpSource& src,
                        CumbiaEpics *ct);

    virtual ~CuEpConfiguration();

    void setDesiredAttributeProperties(const std::vector<std::string> props);

    // CuThreadListener interface
public:
    void onProgress(int, int, const CuData &);
    void onResult(const CuData &data);
    CuData getToken() const;

    // CuEpicsActionI interface
public:
    EpSource getSource() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    void sendData(const CuData& data);
    void getData(CuData& d_inout) const;
    size_t dataListenersCount();
    void start();
    void stop();

private:
    CuTAttConfigurationPrivate *d;
};

#endif // CUTATTCONFIGURATION_H
