#ifndef CUACTIVITY_H
#define CUACTIVITY_H

class CuThread_Qt;
class CuActivityPrivate;
class CuActivityManager;
class CuActivityEvent;

#include <cudata.h>

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

    /** \brief execute is called repeatedly while this method returns an integer greater than 0
     *
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
