#ifndef CUACTIVITY_H
#define CUACTIVITY_H

class CuThread_Qt;
class CuActivityPrivate;
class CuActivityManager;
class CuActivityEvent;

#include <cudata.h>

/*! \brief a class with hook functions where work can be done in a separate thread and wherefrom
 *         results are *published* to the main thread.
 *
 * \section hook_fun hook functions
 *
 * Three pure virtual methods are defined by CuActivity abstract class. The code that subclasses
 * implement within those methods are executed in a background thread. When the task is over,
 * the results can be safely delivered to the main thread through the publishResult function.
 * The progress of the ongoing work can also be notified to the main thread with publishProgress.
 *
 * \li init
 * \li execute
 * \li onExit
 *
 * \subsection init_hk The init hook
 * Here subclasses will place initialisation code, for example a *tcp* connection can be set up
 * The state of the connection or connection errors can be notified on the main thread calling
 * publishResult.
 *
 * \subsection ex_hk The execute hook
 * Subclasses will put the main part of the work here. If in the CuActivity::init hook a *tcp
 * connection* has been successfully established, in the *execute* hook data can be downloaded
 * from the remote host. The progress of the download can be notified through the publishProgress
 * method. The cuurent step, the total steps and data can be delivered from the background thread
 * to the main thread, where a progress bar on a graphical user interface shows the percentage
 * of the download progress. Activities intended to be executed once are called *isolated activities*
 * (see below).
 *
 * The *execute* method can also be called periodically. For instance, a temperature can be read
 * at regular intervals from a device through the network (see continuous activities, such as
 * CuContinuousActivity). In this case, subclasses will return a timeout in milliseconds from the
 * CuActivity::repeat function.
 *
 * \subsection exit_hk The onExit hook
 * After *init* and *execute* finish, the onExit hook is invoked. The code inside onExit, as well as in
 * the two other mentioned hooks, is run in a separate thread. There you would clean up the
 * resources allocated in *init* and exploited in the *execute* method. In our example, the *tcp
 * connection* may be closed.
 * See the \ref cont_act section for further details about onExit invocation in *continuous
 * activities*.
 *
 * \subsection post_data Post data from the background thread to the main
 *
 * The means to deliver results from the background thread where init is executed
 * and the main thread is the CuData class, that is a *key/value* bundle.
 * These are the three methods executed in background. They must be implemented in subclasses.
 *
 *
 * \section isol_act Isolated activities
 *
 * Isolated activities are designed to be executed once. This means *execute is called only once*
 * after *init*.
 * CuActivity::Flags can be set in order to customise the behavior of the activity. In particular,
 * an activity
 * \li can be automatically unregistered after CuActivity::onExit (CuActivity::CuAUnregisterAfterExec
 *     flag)
 * \li can be automatically deleted when no longer necessary (CuActivity::CuADeleteOnExit)
 *
 * CuIsolatedActivity by default enables CuActivity::CuADeleteOnExit and CuActivity::CuAUnregisterAfterExec
 *
 * \section cont_act Continuous activities
 *
 * In a *continuous activity* the *execute* method is called periodically by a timer. To exit
 * the timer, the activity is normally unregistered through Cumbia::unregisterActivity. An exit
 * event interrupts the timer and onExit is invoked at last.
 *
 * CuActivity::Flags can be set in order to customise the behavior of the activity.
 * CuContinuousActivity by default enables CuActivity::CuADeleteOnExit and CuActivity::CuAUnregisterAfterExec
 * See the section \ref isol_act above for further information about the *activity flags*.
 *
 *
 */
class CuActivity
{
public:
    enum Flags { CuAUnregisterAfterExec = 0x01, CuADeleteOnExit = 0x02, CuAUserStartFlags = 0x0100, MaxUserFlags = 0x8000 };

    enum StateFlags { CuAStateInit = 0x01, CuAStateExecute = 0x02, CuAStateOnExit = 0x04 };

    CuActivity(CuActivityManager *activityManager, const CuData& token = CuData());

    CuActivity(const CuData& token = CuData());

    virtual ~CuActivity();

    enum ActivityType { Isolated = 0, Continuous, User = 100 };

    void setActivityManager(CuActivityManager *am);

    virtual int getType() const = 0;

    /** \brief Receive events from the main thread in the CuActivity thread of execution.
     *
     * @param e the event. Activity reimplementations can extend CuActivityEvent to provide custom events
     *        Do not delete e after use. Cumbia will delete it after this method invocation.
     */
    virtual void event(CuActivityEvent *e) = 0;

    virtual int getFlags() const;

    int getStateFlags() const;

    void setFlags(int f);

    void setFlag(Flags f, bool on);

    virtual bool matches(const CuData& token) const = 0;

    virtual void dispose(bool disposable = true);

    virtual bool isDisposable() const;

    /*! \brief must return the interval of time, in milliseconds, before the next CuActivity::execute call,
     *         or zero or negative in order to not execute again
     *
     * @return an integer greater than zero representing the interval, in milliseconds, before next execution
     *         or zero (or negative) to prevent CuActivity::execute to be called again
     */
    virtual int repeat() const = 0;

    void publishResult(const CuData &data);

    void publishProgress(int step, int total, const CuData& data);

    void publishExitResult(const CuData* data);

    CuActivityManager *getActivityManager() const;

    CuData getToken() const;

    /* template method: sets state and calls init */
    void doInit();
    /* template method: sets state and calls execute */
    void doExecute();
    /* template method: sets state, calls onExit and posts an exit event on the main thread */
    void doOnExit();

    void exitOnThreadQuit();

protected:

    virtual void init() = 0;

    virtual void execute() = 0;

    virtual void onExit() = 0;

private:

    CuActivityPrivate *d;



};

#endif // CUACTIVITY_H
