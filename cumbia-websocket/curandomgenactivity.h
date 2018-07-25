#ifndef CURANDOMGENACTIVITY_H
#define CURANDOMGENACTIVITY_H

#include <cucontinuousactivity.h>
#include <cuactivityevent.h>
#include <list>
#include <cuwsactioni.h>
#include <ws_source.h>

class CuData;
class CuRandomGenActivityPrivate;
class CmdData;

class CuAddPollActionEvent : public CuActivityEvent
{
public:
    enum MyType { AddPollAction = CuActivityEvent::User + 9 };

    CuAddPollActionEvent(const WSSource &t, CuWSActionI * a);

    WSSource tsource;
    CuWSActionI *action;

    // CuActivityEvent interface
public:
    virtual Type getType() const;
};

class CuRemovePollActionEvent : public CuActivityEvent
{
public:
    enum MyType { RemovePollAction = CuActivityEvent::User + 10 };

    CuRemovePollActionEvent(const WSSource &t);

    WSSource tsource;

    // CuActivityEvent interface
public:
    virtual Type getType() const;
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
 * CuRandomGenActivity *must stay in execution (must not exit)* until CuTReader is stopped (CuTReader::stop).
 * For this reason, CuAUnregisterAfterExec is set to false. When the Tango device is not defined into the
 * database (in CuRandomGenActivity::execute the Tango::DeviceProxy is NULL) the repeat time is set to a negative
 * number. This suspends the continuous activity without exiting. It will be asked to exit from CuTReader::stop
 * through Cumbia::unregisterActivity.
 *
 * CuADeleteOnExit is left to true in order to let the activity be deleted by CuThread after onExit.
 *
 * This combination of the flags guarantees the availability of the CuRandomGenActivity when CuTReader::stop is called
 * (activity will be either executing the polling loop or suspended) and lets CuTReader not worry about the activity deletion.
 *
 * @see CuTReader
 * @see CuTReader::stop
 *
 */
class CuRandomGenActivity : public CuContinuousActivity
{
public:

    /*! \brief defines the Type of the activity, returned by getType
     */
    enum Type { CuRandomGenActivityType = CuActivity::User + 3 };

    CuRandomGenActivity(const CuData& token);

    ~CuRandomGenActivity();

    // CuActivity interface
public:
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuRandomGenActivityPrivate *d;

public:
    void event(CuActivityEvent *e);

    int getType() const;

    int repeat() const;
};

#endif // CUPOLLINGACTIVITY_H
