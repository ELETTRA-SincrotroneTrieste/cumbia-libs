#ifndef CUACTIVITY_H
#define CUACTIVITY_H

class CuThread_Qt;
class CuActivityPrivate;
class CuThreadInterface;
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
 * \note
 * Calling publishResult from within onExit requires special attention.
 * You must be sure that listeners have not been destroyed by that time.
 *
 * \subsection execution_thred Thread of execution
 * When Cumbia::registerActivity is called to run a new activity, a so called
 * *thread token* is passed as input argument to the method.
 * The *thread token* is compared to all other *threads running in the same cumbia*
 * application using  CuThread::isEquivalent. If an *equivalent* thread is found,
 * it is reused for the new activity. A new thread is started otherwise.
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
 * If the flag CuAUnregisterAfterExec is set to false, then the activity remains *alive* after
 * the end of execute and it is possible to exploit this to let the activity wait for further
 * events that wake it up and trigger another execution. For example, the CuEventActivity
 * in the *cumbia-tango* module and the CuMonitorActivity in the *cumbia-epics* module set this
 * flag to *false* in order to wait for *events* from the Tango or Epics *control system*
 * engines, respectively.
 *
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
 * The period of a *continuous activity* can either be set before execution or at run time by
 * CuContinuousActivity::setInterval. The CuContinuousActivity::getTimeout returns the value of the
 * period. Special events can be sent from the main thread to the background thread in order to
 * temporarily change the state of a CuContinuousActivity. A *continuous activity*
 * \li can be paused
 * \li resumed
 * \li explicitly *execute*d
 * \li allows to change its period.
 * See CuContinuousActivity::event documentation for further details.
 *
 * \section act_implementations Examples
 *
 * \subsection cumbia_tango_module Implementations in the cumbia-tango module
 *
 * \subsubsection cupollact CuPollingActivity
 * The *cumbia-tango* module implements CuContinuousActivity in its CuPollingActivity class,
 * that periodically reads Tango attributes or commands.
 *
 * \subsubsection cuevact CuEventActivity
 * The *cumbia-tango* module implements CuActivity in CuEventActivity. CuEventActivity's
 * repeat method returns 0, so that execute is called only once. CuEventActivity could
 * have been derived from CuIsolatedActivity as well.
 *
 * \subsubsection cutdbpropact CuGetTDbPropActivity
 * CuGetTDbPropActivity from the *cumbia-tango* module inherits from CuIsolatedActivity
 * so that its *execute* method is run only once. In fact, CuGetTDbPropActivity connects
 * to the dtabase and reads from it only one time in the *execute* method.
 *
 * \subsubsection observ Observations
 * As you can see, CuActivity and its *isolated* and *continuous* flavours offer enough
 * flexibility to the clients: CuEventActivity directly derives from CuActivity and
 * determines its *one shot* nature through the *repeat* method. It could have
 * subclassed CuIsolatedActivity as well. CuPollingActivity benefits from the *periodic*
 * nature of CuContinuousActivity. Finally, CuGetTDbPropActivity is an example where only
 * the *execute* method is really doing work. CuGetTDbPropActivity's *init* and *onExit*
 * have empty bodies. See the cumbia-tango module for further reading.
 *
 * \subsubsection cua_evloop Suspend the background execution of an activity: wait for events and execute callbacks
 *
 * If the flag CuActivity::CuAUnregisterAfterExec is set to false, the execution of the
 * activity is *suspended* after *execute* finishes, as in an *event loop*. The activity
 * waits for the next event to wake it up and do some job. CuEventActivity from the
 * cumbia-tango module and CuMonitorActivity from the cumbia-epics module benefit from this
 * feature to listen for upcoming *events*. When an event arrives, the respective
 * callbacks are invoked, the data is extracted from the event and the result is published
 * on the main thread.
 * If CuActivity::CuAUnregisterAfterExec is set to false, Cumbia::unregisterActivity may
 * be needed to exit this activity "*loop*".
 *
 * \subsubsection write_simple_act Write a simple activity
 * \li \ref md_src_tutorial_cuactivity
 */
class CuActivity
{
public:
    enum Flags { CuAUnregisterAfterExec = 0x01, CuADeleteOnExit = 0x02, CuAUserStartFlags = 0x0100, MaxUserFlags = 0x8000 };

    CuActivity(const CuData& token);
    virtual ~CuActivity();

    enum ActivityType { Isolated = 0, Continuous, WorkQueue, User = 100 };

    void setThread(CuThreadInterface *thread);

    virtual int getType() const = 0;

    /** \brief Receive events from the main thread in the CuActivity thread of execution.
     *
     * @param e the event. Activity reimplementations can extend CuActivityEvent to provide custom events
     *        Do not delete e after use. Cumbia will delete it after this method invocation.
     */
    virtual void event(CuActivityEvent *e) = 0;

    virtual int getFlags() const;
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
    void publishResult(const std::vector<CuData> &datalist);

    const CuData getToken() const;
    void setThreadToken(const std::string &tt);
    const std::string threadToken() const;

    /* template method: sets state and calls init */
    void doInit();
    /* template method: sets state and calls execute */
    void doExecute();
    /* template method: sets state, calls onExit and posts an exit event on the main thread */
    void doOnExit();

    void exitOnThreadQuit();

    void setUpdatePolicyHints(int hints);
    int updatePolicyHints() const;

protected:
    virtual void init() = 0;
    virtual void execute() = 0;
    virtual void onExit() = 0;

    virtual CuThreadInterface *thread() const;

private:

    CuActivityPrivate *d;



};

#endif // CUACTIVITY_H
