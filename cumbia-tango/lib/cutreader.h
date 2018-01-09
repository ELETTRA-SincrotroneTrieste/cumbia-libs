#ifndef CUTREADER_H
#define CUTREADER_H

#include <string>
#include <cuthreadlistener.h>
#include <tsource.h>
#include <cutangoactioni.h>

class CuTReaderPrivate;
class CuActivityManager;
class CuDataListener;

/** \brief Low level class to read tango attributes and commands with output arguments.
 *
 * This class implements the CuTangoActionI interface, which in turn implements CuThreadListener.
 *
 * Programmers of graphical interfaces should employ cumbia-tango-controls CuTControlsReader higher level
 * class to create and communicate with CuTReader.
 */
class CuTReader: public CuTangoActionI /* CuTangoActionI implements CuThreadListener */
{
public:

    /** \brief The reader refresh mode.
     *
     * Each refresh mode reflects the way a tango attribute can be read.
     * Commands are always <em>polled</em> with a given period (default: 1000ms).
     * The preferred read mode for attributes is by events (ChangeEventRefresh). If they are not available,
     * the second choice mode is PolledRefresh.
     *
     * Manual mode starts (or switches to) a polling activity that is immediately paused. A specific argument
     * must be specified to sendData to trigger a manual reading.
     *
     */
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

    std::string refreshModeStr() const;

    RefreshMode refreshMode() const;

    void setPeriod(int millis);

    int period() const;

    void start();

    void stop();

    void addDataListener(CuDataListener *l);

    void removeDataListener(CuDataListener *l);

    size_t dataListenersCount();

    bool exiting() const;

private:
    CuTReaderPrivate *d;

    void m_startEventActivity();

    void m_startPollingActivity(bool fallback);

};


#endif // CUTREADER_H
