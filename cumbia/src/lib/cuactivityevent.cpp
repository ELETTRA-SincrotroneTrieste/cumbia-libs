#include "cuactivityevent.h"

/*! \brief returns CuActivityEvent::Pause
 * @return CuActivityEvent::Pause
 */
CuActivityEvent::Type CuPauseEvent::getType() const {
    return CuActivityEvent::Pause;
}

/*! \brief returns CuActivityEvent::Resume
 * @return CuActivityEvent::Resume
 */
CuActivityEvent::Type CuResumeEvent::getType() const
{
    return CuActivityEvent::Resume;
}

/*! \brief the class constructor for a timeout change event
 *
 * @param timeout the new timeout, in milliseconds
 */
CuTimeoutChangeEvent::CuTimeoutChangeEvent(int timeout)
{
    m_timeout = timeout;
}

/*! \brief returns CuActivityEvent::TimeoutChange
 * @return CuActivityEvent::TimeoutChange
 */
CuActivityEvent::Type CuTimeoutChangeEvent::getType() const
{
    return CuActivityEvent::TimeoutChange;
}

/*! \brief returns the timeout that was set in the constructor for the
 *         timeout change event
 *
 * @return the milliseconds timeout that was set in the constructor for the
 *         timeout change event
 */
int CuTimeoutChangeEvent::getTimeout() const
{
    return m_timeout;
}

/*! \brief returns CuActivityEvent::TimeoutChange
 * @return CuActivityEvent::TimeoutChange
 */
CuActivityEvent::Type CuExecuteEvent::getType() const
{
    return CuActivityEvent::Execute;
}
