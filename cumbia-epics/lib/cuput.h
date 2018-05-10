#ifndef CUTWRITER_H
#define CUTWRITER_H

#include <cuepactioni.h>

class CuEpWriterPrivate;

class CuPut : public CuEpicsActionI
{
public:
    CuPut(const EpSource &src, CumbiaEpics *ct);
    virtual ~CuPut();

    void setWriteValue(const CuVariant& wval);

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    CuData getToken() const;

    // CuEpicsActionI interface
public:
    EpSource getSource() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();
    void start();
    void stop();

    void sendData(const CuData &data);
    void getData(CuData &d_inout) const;
private:
    CuEpWriterPrivate *d;
};

#endif // CUTWRITER_H
