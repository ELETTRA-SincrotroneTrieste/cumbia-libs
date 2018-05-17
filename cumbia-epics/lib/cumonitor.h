#ifndef CUMONITOR_H
#define CUMONITOR_H

#include <string>
#include <cuthreadlistener.h>
#include <epsource.h>
#include <cuepactioni.h>

class CuMonitorPrivate;
class CuActivityManager;
class CuDataListener;

class CuMonitor: public CuEpicsActionI /* CuEpicsActionI implements CuThreadListener */
{
public:

    enum RefreshMode { MonitorRefresh = 0, Manual };

    CuMonitor(const EpSource& src, CumbiaEpics *ct);

    ~CuMonitor();

    // CuThreadListener interface
    void onProgress(int step, int total, const CuData &data);

    void onResult(const CuData &data);

    CuData getToken() const;

    EpSource getSource() const;

    CuEpicsActionI::Type getType() const;

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

    virtual bool exiting() const;

private:
    CuMonitorPrivate *d;

    void m_startEventActivity();

    void m_startMonitorActivity();
};


#endif // CuMonitor_H
