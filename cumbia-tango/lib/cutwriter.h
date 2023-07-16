#ifndef CUTWRITER_H
#define CUTWRITER_H

#include <cutangoactioni.h>
#include <vector>

class CuTWriterPrivate;

class CuTWriter : public CuTangoActionI
{
public:
    CuTWriter(const TSource &src, CumbiaTango *ct,
              const CuData& conf,
              const CuData& options = CuData(),
              const CuData& tag = CuData());
    virtual ~CuTWriter();

    void setWriteValue(const CuVariant& wval);
    void setConfiguration(const CuData &db_conf);
    void setOptions(const CuData& options);
    void setTag(const CuData& tag);

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    void onResult(const std::vector<CuData> &datalist);
    void onResult(const std::vector<CuData> *datalist);
    CuData getToken() const;

    // CuTangoActionI interface
public:
    TSource getSource() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();
    void start();
    void stop();

    void sendData(const CuData &data);
    void getData(CuData &d_inout) const;
private:
    CuTWriterPrivate *d;

};

#endif // CUTWRITER_H
