#ifndef CUTREADER_H
#define CUTREADER_H

#include <string>
#include <cuthreadlistener.h>
#include <tsource.h>
#include <cutangoactioni.h>

class CuTReaderPrivate;
class CuActivityManager;
class CuDataListener;

class CuTReader: public CuTangoActionI /* CuTangoActionI implements CuThreadListener */
{
public:

    enum RefreshMode { PolledRefresh = 0, ChangeEventRefresh, ArchiveEventRefresh, PeriodicEventRefresh, Manual };

    CuTReader(const TSource& src, CumbiaTango *ct);

    ~CuTReader();

    // CuThreadListener interface
    void onProgress(int step, int total, const CuData &data);

    void onResult(const CuData &data);

    CuData getToken() const;

    TSource getSource() const;

    CuTangoActionI::Type getType() const;

    void sendData(const CuData& data);

    void getData(CuData &inout) const;

    void setRefreshMode(RefreshMode rm);

    RefreshMode refreshMode() const;

    void setPeriod(int millis);

    int period() const;

    void start();

    void stop();

    void addDataListener(CuDataListener *l);

    void removeDataListener(CuDataListener *l);

    size_t dataListenersCount();

    bool stopping() const;

private:
    CuTReaderPrivate *d;

    void m_startEventActivity();

    void m_startPollingActivity(bool fallback);

};


#endif // CUTREADER_H
