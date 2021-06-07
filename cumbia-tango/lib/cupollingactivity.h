#ifndef CUPOLLINGACTIVITY_H
#define CUPOLLINGACTIVITY_H

#include <cucontinuousactivity.h>
#include <cuactivityevent.h>
#include <list>
#include <cutangoactioni.h>
#include <tsource.h>
#include <map>

class CuData;
class CuPollingActivityPrivate;
class CuDeviceFactoryService;
class CmdData;

class CuAddPollActionEvent : public CuActivityEvent
{
public:
    enum MyType { AddPollAction = CuActivityEvent::User + 9 };

    CuAddPollActionEvent(const TSource &t, CuTangoActionI * a);

    TSource tsource;
    CuTangoActionI *action;

    // CuActivityEvent interface
public:
    virtual Type getType() const;
};

class CuRemovePollActionEvent : public CuActivityEvent
{
public:
    enum MyType { RemovePollAction = CuActivityEvent::User + 10 };

    CuRemovePollActionEvent(const TSource &t);

    TSource tsource;

    // CuActivityEvent interface
public:
    virtual Type getType() const;
};


class CuArgsChangeEvent : public CuActivityEvent {

public:
    enum CuAChangeEvType { ArgsChangeEvent = CuActivityEvent::User + 14 };

    CuArgsChangeEvent(const TSource& _ts, const std::vector<std::string> &_args)
        : ts(_ts), args(_args) {}

    const TSource ts;
    const std::vector<std::string> args;
    virtual Type getType() const;
};

class ActionData {
public:
    ActionData(const TSource& ts, CuTangoActionI *a_ptr) : tsrc(ts), action(a_ptr) {}
    ActionData() { action = nullptr; }
    TSource tsrc;
    CuTangoActionI *action;
};

/*! \brief an activity to periodically read from Tango. Implements CuContinuousActivity
 *
 * Implementing CuActivity, the work is done in the background by the three methods
 *
 * \li init
 * \li execute
 * \li onExit
 *
 * They are called from a separate thread. In cumbia-tango, threads are grouped by device.
 *
 * Apart from the three execution hooks listed above, some other methods have been written
 * to comply with the CuActivityInterface:
 *
 * \li getType, returning CuEventActivityType
 * \li event, to receive CuActivityEvent events
 * \li matches, that returns true if the input token represents an activity like this
 * \li repeat, that indicates whether or not execute has to be called again
 *
 * \note
 * CuPollingActivity *must stay in execution (must not exit)* until CuTReader is stopped (CuTReader::stop).
 * For this reason, CuAUnregisterAfterExec is set to false. When the Tango device is not defined into the
 * database (in CuPollingActivity::execute the Tango::DeviceProxy is NULL) the repeat time is set to a negative
 * number. This suspends the continuous activity without exiting. It will be asked to exit from CuTReader::stop
 * through Cumbia::unregisterActivity.
 *
 * CuADeleteOnExit is left to true in order to let the activity be deleted by CuThread after onExit.
 *
 * This combination of the flags guarantees the availability of the CuPollingActivity when CuTReader::stop is called
 * (activity will be either executing the polling loop or suspended) and lets CuTReader not worry about the activity deletion.
 *
 * @see CuTReader
 * @see CuTReader::stop
 *
 */
class CuPollingActivity : public CuContinuousActivity
{
public:

    /*! \brief defines the Type of the activity, returned by getType
     */
    enum Type { CuPollingActivityType = CuActivity::User + 3 };

    CuPollingActivity(const CuData& token, CuDeviceFactoryService *df, const CuData& options);
    ~CuPollingActivity();

    void setArgins(const CuVariant &argins);
    size_t actionsCount() const;
    size_t srcCount() const;
    void setSlowDownRate(const std::map<int, int> &sr_millis);
    const std::map<int, int> &slowDownRate() const;
    void decreasePolling();
    int successfulExecCnt() const;
    int consecutiveErrCnt() const;

    const std::multimap<const std::string, ActionData > actionsMap() const;

    // CuActivity interface
public:
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuPollingActivityPrivate *d;

    void m_registerAction(const TSource &ts, CuTangoActionI *a);
    void m_unregisterAction(const TSource &ts);
    void m_edit_args(const TSource& src, const std::vector<std::string> &args);

    // CuActivity interface
public:
    void event(CuActivityEvent *e);

    int getType() const;

    int repeat() const;
};

#endif // CUPOLLINGACTIVITY_H
