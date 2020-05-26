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
 *
 * \par Function
 * Instantiates and registers (i.e. starts) either a CuEventActivity or a CuPollingActivity,
 * according to the desired RefreshMode. If the CuEventActivity fails subscribing to the Tango
 * event system, it is replaced by CuPollingActivity (in CuTReader::onResult). In that case,
 * no error event is delivered to CuDataListeners.
 *
 * \par Note
 * CuTReader holds a pointer to the current activity (CuTReaderPrivate: current_activity).
 * Activity and CuTReader's life are independent from each other: if the CuActivity::CuADeleteOnExit flag
 * is active (as in CuPollingActivity), the activity is deleted by CuThread.
 *
 * \par Important
 * As a consequence of the note above, activities *must not exit before CuTReader::stop is called*.
 * The proper way to stop the activity associated to the reader is through CuTReader::stop.
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
    enum RefreshMode { PolledRefresh = 0x01, ChangeEventRefresh=0x02,
                       ArchiveEventRefresh=0x04, PeriodicEventRefresh=0x08, Manual=0x10 };

    CuTReader(const TSource& src, CumbiaTango *ct);

    ~CuTReader();

    // CuThreadListener interface
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    void onResult(const std::vector<CuData> &datalist);

    CuData getToken() const;
    TSource getSource() const;
    CuTangoActionI::Type getType() const;

    void sendData(const CuData& data);
    void getData(CuData &inout) const;

    void setRefreshMode(RefreshMode rm, int period = -1);
    std::string refreshModeStr() const;
    RefreshMode refreshMode() const;
    bool isEventRefresh(RefreshMode rm) const;
    int period() const;
    void setPeriod(int millis);

    void setOptions(const CuData& options);

    void start();
    void stop();
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();
    bool exiting() const;

private:
    CuTReaderPrivate *d;

    void m_startEventActivity();
    void m_registerToPoller();
    void m_unregisterFromPoller();
    void m_unregisterEventActivity();
};


#endif // CUTREADER_H
