#include "curandomgenactivity.h"
#include "rnd_source.h"
#include "curndactioni.h"
#include <cumacros.h>
#include <vector>
#include <map>
#include <iostream>
#include <QtAlgorithms>
#include <QtDebug>
#include <QThread> // for current thread

/* @private */
class CuRandomGenActivityPrivate
{
public:
    int repeat, errCnt;
    int exec_cnt;
    std::string message;
    pthread_t my_thread_id, other_thread_id;
    bool exiting;
};

/*! \brief the class constructor that sets up a Tango polling activity
 *
 * @param token a CuData that will describe this activity
 * @param df a pointer to a CuDeviceFactoryService that is used by init and onExit to create/obtain and
 *        later get rid of a Tango device, respectively
 * @param argins input arguments that can optionally be passed to Tango commands as argins
 *
 * \par notes
 * \li the default polling period is 1000 milliseconds
 * \li if the "period" key is set on the token, then it is converted to int and it will be used
 *     to set up the polling period
 * \li CuADeleteOnExit is active.
 * \li CuAUnregisterAfterExec is disabled because if the Tango device is not defined into the database
 *     the poller is not started and the activity is suspended (repeat will return -1).
 */
CuRandomGenActivity::CuRandomGenActivity(const CuData &token)
    : CuContinuousActivity(token)
{
    d = new CuRandomGenActivityPrivate;
    d->repeat = 1000;
    d->errCnt = 0;
    d->other_thread_id = pthread_self();
    d->exiting = false;
    d->exec_cnt = 0;

    int period = 5;
    if(token.containsKey("period"))
        period = token["period"].toInt();
    d->repeat = period;
    setInterval(period);
    //  flag CuActivity::CuADeleteOnExit is true
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
}

/*! \brief the class destructor
 *
 * deletes the internal data
 */
CuRandomGenActivity::~CuRandomGenActivity()
{
    qDebug() << __FUNCTION__ << "deleted CuRandomGenActivity" << this;
    delete d;
}


/** \brief returns true if the passed token's *device* *activity* and *period* values matche this activity token's
 *         *device* and *activity* and *period* values.
 *
 * @param token a CuData containg key/value pairs of another activity's token
 * @return true if the input token's "period" *device* and *activity* values match this token's
 * "period" *device* and *activity* values
 *
 * Two CuRandomGenActivity match if they refer to the same device and have the same period.
 *
 * @implements CuActivity::matches
 */
bool CuRandomGenActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"];
}

/*! \brief the implementation of the CuActivity::init hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \par Notes
 * \li in cumbia-tango, threads are grouped by device
 * \li CuDeviceFactoryService::getDevice is called to obtain a reference to a Tango device (in the form
 *     of TDevice)
 * \li TDevice's user refrence count is incremented with TDevice::addRef
 *
 * See also CuActivity::init, execute and onExit
 *
 * @implements CuActivity::init
 *
 */
void CuRandomGenActivity::init()
{
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    printf("CuRandomGenActivity %p init complete", this);
}

/*! \brief the implementation of the CuActivity::execute hook
 *
 * This is repeatedly called in the CuActivity's thread of execution, according to the
 * period chosen for the reader.
 *
 *
 * See also CuActivity::execute and CuEventActivity
 *
 * @implements CuActivity::execute
 */
void CuRandomGenActivity::execute()
{
    assert(d->my_thread_id == pthread_self());
    d->exec_cnt++;
    int qdebug_rate = 10000 / d->repeat;
    CuData res = getToken();
    if(d->exec_cnt % qdebug_rate == 0)
        qDebug() << "CuRandomGenActivity::execute" <<this<< d->exec_cnt << res["src"].toString().c_str() << "thread" << pthread_self() << QThread::currentThread();
    time_t tp;
    time(&tp);
    qsrand(tp);
    if(res["src"].toString().find("spectrum") != std::string::npos ||
            res["src"].toString().find("vector") != std::string::npos) {
        std::vector<double> vd;

        for(int i = 0; i < 1000; i++) {
            time(&tp);
            qsrand(tp);
            vd.push_back(rand() % 100);
        }
        res["value"] = vd;
    }
    else
        res["value"] = qrand() % 100;

    res.putTimestamp(); // timestamp_ms and timestamp_us

//    printf("CuRandomGenActivity.execute: period %d *** data %s\n", getTimeout(), res.toString().c_str());
    publishResult(res);

}



/*! \brief the implementation of the CuActivity::onExit hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \li client reference counter is decreased on the TDevice (TDevice::removeRef)
 * \li CuDeviceFactoryService::removeDevice is called to remove the device from the device factory
 *     if the reference count is zero
 * \li the result of the operation is *published* to the main thread through publishResult
 *
 * See also CuActivity::onExit
 *
 * @implements CuActivity::onExit
 */
void CuRandomGenActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    d->exiting = true;
    int refcnt = -1;
    CuData at = getToken(); /* activity token */
    qDebug() << __FUNCTION__ << "exiting for source after "  << d->exec_cnt << "executions" << at["src"].toString().c_str();
    at["msg"] = "EXITED";
    at["mode"] = "RANDOM";

    cuprintf("\e[1;31mCuRandomGenActivity::onExit(): refcnt = %d called actionRemove for device %s att %s\e[0m\n",
             refcnt, at["device"].toString().c_str(), at["src"].toString().c_str());
    at["exit"] = true;
    // do not publishResult because CuPoller (which is our listener) may be deleted by CuPollingService
    // from the main thread when its action list is empty (see CuPollingService::unregisterAction)
    publishResult(at);
}

/** \brief Receive events *from the main thread to the CuActivity thread*.
 *
 * @param e the event. Do not delete e after use. Cumbia will delete it after this method invocation.
 *
 * @see CuActivity::event
 *
 * \note the CuActivityEvent is forwarded to CuContinuousActivity::event
 */
void CuRandomGenActivity::event(CuActivityEvent *e)
{
    assert(d->my_thread_id == pthread_self());
    CuContinuousActivity::event(e);
}

/*! \brief returns the type of the polling activity
 *
 * @return the CuRandomGenActivityType value defined in the Type enum
 */
int CuRandomGenActivity::getType() const
{
    return CuRandomGenActivityType;
}

/*! \brief returns the polling period, in milliseconds
 *
 * @return the polling period, in milliseconds
 *
 * @implements CuActivity::repeat
 */
int CuRandomGenActivity::repeat() const
{
    assert(d->my_thread_id == pthread_self());
    int ret;
    d->exiting ? ret = -1 : ret = d->repeat;
    return ret;
}

