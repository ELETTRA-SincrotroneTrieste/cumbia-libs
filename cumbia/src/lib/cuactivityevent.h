#ifndef CUACTIVITYEVENT_H
#define CUACTIVITYEVENT_H

#include <cudata.h>

/** \brief Generic event to pass data to an activity
 *
 * Generic activity event that is posted from the main thread to the
 * background thread.
 *
 * Usages:
 * \li Cumbia::setActivityPeriod sends a CuTimeoutChangeEvent event
 * \li Cumbia::pauseActivity sends a CuPauseEvent
 * \li Cumbia::resumeActivity sends a CuResumeEvent
 * \li Cumbia::postEvent barely forwards a *user defined* CuActivityEvent
 *     to the specified activity
 *
 * @see Cumbia::postEvent (can be used by a client)
 * @see CuThread::postEvent (typically used internally by the library)
 *
 */
class CuActivityEvent
{
public:
    enum Type { Pause = 0, ///< CuPauseEvent: pause the thread's timer
                Resume, ///< CuResumeEvent: resume a thread's paused timer
                TimeoutChange, ///< CuTimeoutChangeEvent: change thread's timer period
                Execute, ///< CuExecuteEvent: trigger a call to CuActivity::execute
                Cu_Data,
                User = 100, ///< starting value for user defined events
                MaxUser = 2048 ///< maximum value for user defined events
              };

    /*! \brief class destructor
     *
     * class virtual destructor
     */
    virtual ~CuActivityEvent() {}

    /*! \brief returns the type of the event
     *
     * @return a value from the CuActivityEvent::Type enum, or a user defined
     *         one
     */
    virtual Type getType() const = 0;
};

/*! \brief event to pause an activity's timer
 *
 * This event can be sent to pause the thread's timer
 *
 * @implements CuActivityEvent
 */
class CuPauseEvent : public CuActivityEvent
{
public:
    CuActivityEvent::Type getType() const;
};

/*! \brief event to resume an activity's timer
 *
 * This event resumes the activity's thread timer
 *
 * @implements CuActivityEvent
 */
class CuResumeEvent : public CuActivityEvent
{
public:
    CuActivityEvent::Type getType() const;
};

/*! \brief triggers a call to CuActivity::execute
 *
 * This event triggers a call to CuActivity::execute in the background
 * thread
 *
 * @implements CuActivityEvent
 */
class CuExecuteEvent : public CuActivityEvent
{
public:
    CuActivityEvent::Type getType() const;
};

/*! \brief this event changes the timeout of the activity's thread timer
 *
 * This event contains a new timeout for the activity's thread timer
 *
 * @implements CuActivityEvent
 */
class CuTimeoutChangeEvent : public CuActivityEvent
{
public:
    CuTimeoutChangeEvent(int timeout);

    CuActivityEvent::Type getType() const;

    int getTimeout() const;

private:
    int m_timeout;
};

/*! \brief An event that can be used to forward data through Cumbia::postEvent
 *
 */
class CuDataEvent : public CuActivityEvent {
public:
    CuDataEvent(const CuData& errdat);

    CuActivityEvent::Type getType() const;

    CuData getData() const;

private:
    CuData m_data;
};


#endif // CUACTIVITYEVENT_H
