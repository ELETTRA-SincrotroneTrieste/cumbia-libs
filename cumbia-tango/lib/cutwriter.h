#ifndef CUTWRITER_H
#define CUTWRITER_H

#include <cutangoactioni.h>

class CuTWriterPrivate;

class CuTWriter : public CuTangoActionI
{
public:
    CuTWriter(const TSource &src, CumbiaTango *ct);
    virtual ~CuTWriter();

    void setWriteValue(const CuVariant& wval);

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
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
    bool exiting() const;

    void sendData(const CuData &data);
    void getData(CuData &d_inout) const;
private:
    CuTWriterPrivate *d;
};

#endif // CUTWRITER_H
