#include "curndactionreader.h"
#include "cumbiarandom.h"
#include "curndactionfactoryservice.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <set>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <curandomgenactivity.h>
#include "curndfunctiongenerators.h"

#include "curndactionreader.h"
#include <cumacros.h>

#include <QtDebug>


class CuRNDActionReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    RNDSource tsrc;
    CumbiaRandom *cumbia_rnd;
    bool exit;
    CuRandomGenActivity *randomgen_a;
    CuData options, value_d;
    int period;
    CuRNDActionReader::RefreshMode refresh_mode;
};

CuRNDActionReader::CuRNDActionReader(const RNDSource& src, CumbiaRandom *ct) : CuRNDActionI()
{
    d = new CuRNDActionReaderPrivate;
    d->tsrc = src;
    d->cumbia_rnd = ct;
    d->exit = false;  // set to true by stop
    d->period = 1000;
    d->refresh_mode = RandomGenerator;
    d->options["min"] = 0;
    d->options["max"] = 1000;
    d->options["size"] = 1;
}

CuRNDActionReader::~CuRNDActionReader()
{
    pdelete("~CuRNDActionReader %p\n", this);
    delete d;
}

/*! \brief progress notification callback
 *
 * @param step the completed steps in the background
 * @param total the total number of steps
 * @param data CuData with data from the background activity
 *
 * The current implementation does nothing
 */
void CuRNDActionReader::onProgress(int step, int total, const CuData &data) {
    (void) step;  (void) total;  (void) data;
}

void CuRNDActionReader::onResult(const std::vector<CuData> &datalist) {
    (void) datalist;
}

void CuRNDActionReader::setOptions(const CuData &options) {
    d->options = options;
}

/*
 * \brief delivers to the main thread the result of a task executed in background.
 *
 * See  \ref md_lib_cudata_for_tango
 *
 * The d->exit flag is true only if the CuRNDActionReader::stop has been called. (data listener destroyed
 * or reader disconnected ("unset source") )
 * Only in this case CuRNDActionReader auto deletes itself when data["exit"] is true.
 * data["exit"] true is not enough to dispose CuRNDActionReader because CuRNDActionReader handles two types of
 * activities (polling and event).
 *
 * If the error flag is set by the CuEventActivity because subscribe_event failed, the poller is started
 * and the error *is not* notified to the listener(s)
 *
 */
void CuRNDActionReader::onResult(const CuData &data)
{
//    bool err = data["err"].toBool();
    bool a_exit = data["exit"].toBool(); // activity exit flag
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::set<CuDataListener *> lis_copy = d->listeners;
    std::set<CuDataListener *>::iterator it;
    // if it's just subscribe_event failure, do not notify listeners
    for(it = lis_copy.begin(); it != lis_copy.end(); ++it) {
        (*it)->onUpdate(data);
    }

    /* remove last listener and delete this
     * - if d->exit is set to true (CuRNDActionReader has been stop()ped )
     */
    if(d->exit && a_exit)
    {
        CuRNDActionFactoryService * af = static_cast<CuRNDActionFactoryService *>(d->cumbia_rnd->getServiceProvider()
                                                                            ->get(static_cast<CuServices::Type>(CuRNDActionFactoryService::CuRNDActionFactoryServiceType)));
        af->unregisterAction(d->tsrc.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

/*! \brief returns the CuData storing the token that identifies this action
 *
 * @return a CuData with the following key/value pairs
 *
 * \li "source" --> the name of the source (string)
 * \li "type" --> a constant string: "reader"
 */
CuData CuRNDActionReader::getToken() const
{
    CuData da("source", d->tsrc.getName());
    da["type"] = std::string("reader");
    return da;
}

/*! returns the *tango* source (TSource)
 *
 * @return a TSource object that describes the Tango source
 */
RNDSource CuRNDActionReader::getSource() const
{
    return d->tsrc;
}

CuRNDActionI::Type CuRNDActionReader::getType() const
{
    return CuRNDActionI::Reader;
}

/** \brief Send data with parameters to configure the reader.
 *
 * @param data a CuData bundle with the settings to apply to the reader.
 *
 * \par Valid keys
 * \li "period": integer. Change the polling period, if the refresh mode is CuRNDActionReader::PolledRefresh
 * \li "refresh_mode". A CuRNDActionReader::RefreshMode value to change the current refresh mode.
 * \li "read" (value is irrelevant). If the read mode is CuRNDActionReader::PolledRefresh, a read will be
 *     performed.
 *
 * @see getData
 *
 */
void CuRNDActionReader::sendData(const CuData &data)
{
    printf("\e[1;35msendData sending %s\e[0m\n", data.toString().c_str());

    if(data.containsKey("refresh_mode"))
        d->refresh_mode = static_cast<CuRNDActionReader::RefreshMode>(data["refresh_mode"].toInt());
    if(data.containsKey("period")) {
        int period2 = data["period"].toInt();
    }
}

/** \brief Get parameters from the reader.
 *
 * @param d_inout a reference to a CuData bundle containing the parameter names
 *        as keys. getData will associate the values to the keys.
 *        Unrecognized keys are ignored.
 *
 * \par Valid keys
 * \li "period": returns an int with the polling period
 * \li "refresh_mode": returns a CuRNDActionReader::RefreshMode that can be converted to int
 * \li "mode": returns a string representation of the CuRNDActionReader::RefreshMode
 *
 * @see sendData
 */
void CuRNDActionReader::getData(CuData &d_inout) const
{
    if(d_inout.containsKey("period"))
        d_inout["period"] = d->period;
    if(d_inout.containsKey("refresh_mode"))
        d_inout["refresh_mode"] = d->refresh_mode;
    if(d_inout.containsKey("mode"))
        d_inout["mode"] = refreshModeStr();
}

/*! \brief set or change the reader's refresh mode
 *
 * If the reading activity hasn't been started yet, the mode is saved for later.
 * If an activity is already running and the requested mode is different, the current
 * activity is unregistered and a new one is started.
 *
 * @param rm a value chosen from CuRNDActionReader::RefreshMode.
 *
 *
 */
void CuRNDActionReader::setRefreshMode(CuRNDActionReader::RefreshMode rm)
{
    d->refresh_mode = rm;
}

string CuRNDActionReader::refreshModeStr() const
{
    switch(d->refresh_mode)
    {
    case CuRNDActionReader::RandomGenerator:
        return "RandomGenerator";
    default:
        return "InvalidRefreshMode";
    }
}

int CuRNDActionReader::period() const {
    return d->period;
}

CuRNDActionReader::RefreshMode CuRNDActionReader::refreshMode() const {
    return d->refresh_mode;
}

void CuRNDActionReader::setPeriod(int millis) {
    d->period = millis;
}

/*
 * main thread
 */
void CuRNDActionReader::stop()
{
//    if(d->exit)
//        d->log.write("CuRNDActionReader.stop", CuLog::LevelError, CuLog::CategoryRead, "stop called twice for reader %s", this->getToken()["source"].toString().c_str());
//    else
    if(!d->exit)
    {
        d->exit = true;
        m_stopRandomGenActivity();
    }
}

void CuRNDActionReader::addDataListener(CuDataListener *l)
{
    std::set<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuRNDActionReader::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuRNDActionReader::dataListenersCount()
{
    return d->listeners.size();
}

bool CuRNDActionReader::exiting() const
{
    return d->exit;
}

/*! \brief returns true if the parameter is an event driven RefreshMode, false otherwise.
 *
 * @param rm a value picked from CuRNDActionReader::RefreshMode enumeration
 * @return true if rm is ChangeEventRefresh, PeriodicEventRefresh or ArchiveEventRefresh,
 *         false otherwise
 *
 */
bool CuRNDActionReader::isEventRefresh(CuRNDActionReader::RefreshMode rm) const
{
    return false;
}

void CuRNDActionReader::start()
{
     m_startRandomGenActivity();
}

void CuRNDActionReader::m_startRandomGenActivity()
{
    CuData at("src", d->tsrc.getName()); /* activity token */
    at["activity"] = "random";
    at["rmode"] = refreshModeStr();
    at["period"] = d->period;
    if(d->options.containsKey("label"))
        at["label"] = d->options["label"].toString();
    CuData tt; // thread token
    d->options.containsKey("thread_token") ? tt["thtok"] = d->options["thread_token"] : tt["thtok"] = d->tsrc.getName();
    d->randomgen_a = new CuRandomGenActivity(at);
    double min, max; int siz = 1, period = 1000;
    d->options["min"].to<double>(min);
    d->options["max"].to<double>(max);
    d->options["size"].to<int>(siz);
    d->options["period"].to<int>(period);
    d->randomgen_a->setBounds(min, max);
    d->randomgen_a->setSize(siz);
    d->randomgen_a->setPeriod(period);
    if(d->options.containsKey("jsfile"))
        d->randomgen_a->setFunctionGenerator(
                    new CuRndJsFunctionGen(QString::fromStdString(d->options["jsfile"].toString())));
    else if(d->options.has("function", "sin"))
        d->randomgen_a->setFunctionGenerator(new CuRndSinFunctionGen());
    // else the default random generation will be selected by d->randomgen_a

    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_rnd->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_rnd->getThreadFactoryImpl());
    d->cumbia_rnd->registerActivity(d->randomgen_a, this, tt, fi, bf);
}

void CuRNDActionReader::m_stopRandomGenActivity()
{
    d->cumbia_rnd->unregisterActivity(d->randomgen_a);
    d->randomgen_a = NULL; // not safe to dereference henceforth
}

