#ifndef CUTATTCONFIGURATION_H
#define CUTATTCONFIGURATION_H

#include <cuepactioni.h>

class CuEpConfigurationPrivate;

class CuEpConfiguration : public CuEpicsActionI
{
public:

    CuEpConfiguration(const EpSource& src,
                        CumbiaEpics *ct);

    virtual ~CuEpConfiguration();

    void setDesiredPVProperties(const std::vector<std::string> props);

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
    bool exiting() const;
private:
    CuEpConfigurationPrivate *d;

    // CuThreadListener interface
public:
    virtual void onResult(const std::vector<CuData> &datalist);
};

#endif // CUTATTCONFIGURATION_H
