#ifndef CURNDACTIONREADER_H
#define CURNDACTIONREADER_H

#include <string>
#include <cuthreadlistener.h>
#include <curndactioni.h>
#include <curndactionfactoryi.h>
#include <rnd_source.h>

class CuRNDActionReaderPrivate;
class CuActivityManager;
class CuDataListener;
class CumbiaWebSocket;



/** \brief Low level class to read tango attributes and commands with output arguments.
 *
 * This class implements the CuRNDActionI interface, which in turn implements CuThreadListener.
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
class CuRNDActionReader: public CuRNDActionI /* CuRNDActionI implements CuThreadListener */
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
    enum RefreshMode { RandomGenerator = 0 };

    CuRNDActionReader(const RNDSource& src, CumbiaWebSocket *ct);

    ~CuRNDActionReader();

    // CuThreadListener interface
    void onProgress(int step, int total, const CuData &data);

    void onResult(const CuData &data);

    void onResult(const std::vector<CuData> &datalist);

    CuData getToken() const;

    RNDSource getSource() const;

    CuRNDActionI::Type getType() const;

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

    bool isEventRefresh(RefreshMode rm) const;

private:
    CuRNDActionReaderPrivate *d;

    void m_startRandomGenActivity();

    void m_stopRandomGenActivity();

};


#endif // CUTREADER_H
