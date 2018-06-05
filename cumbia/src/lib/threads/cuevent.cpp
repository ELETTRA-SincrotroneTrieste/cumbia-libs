#include "cuevent.h"


/*!
 * \brief CuResultEvent::CuResultEvent the class constructor
 * \param sender the CuActivity that originates the event
 * \param data the data to be delivered
 * \param t the type of event
 */
CuResultEvent::CuResultEvent(const CuActivity* sender, const CuData &data, CuEventI::CuEventType t)
{
    d_p = new CuResultEventPrivate(data);
    d_p->type = t;
    d_p->activity = sender;
}


/*!
 * \brief CuResultEvent::CuResultEvent the class constructor
 * \param sender the CuActivity that originates the event
 * \param data_list a vector of data to be delivered
 * \param t the type of event
 */
CuResultEvent::CuResultEvent(const CuActivity* sender, const std::vector<CuData> &data_list, CuEventType t)
{
    d_p = new CuResultEventPrivate(data_list);
    d_p->type = t;
    d_p->activity = sender;
}

/*!
 * \brief CuResultEvent::CuResultEvent class constructor with progress information
 * \param sender the CuActivity that originated the event (sender)
 * \param step the current step of a multi-step data processing
 * \param total the total steps of a multi-step data processing
 * \param data the data to be delivered
 */
CuResultEvent::CuResultEvent(const CuActivity* sender, int step, int total, const CuData &data)
{
    d_p = new CuResultEventPrivate(data);
    d_p->step = step;
    d_p->total = total;
    d_p->type = Progress;
    d_p->activity = sender;
}

/*! \brief the class destructor
 *
 * \par Event destruction
 * The event is destroyed after QApplication::postEvent if QThreadsEventBridge
 * is used (see QThreadsEventBridge::postEvent and CuEvent_Qt::~CuEvent_Qt)
 */
CuResultEvent::~CuResultEvent()
{
    delete d_p;
}

/*!
 * \brief CuResultEvent::setType sets the event type
 *
 * \param t the event type as CuEventI::CuEventType
 */
void CuResultEvent::setType(CuEventI::CuEventType t)
{
    d_p->type = t;
}

/*!
 * \brief CuResultEvent::getType returns the event type
 *
 * \return the event type as CuEventI::CuEventType
 */
CuEventI::CuEventType CuResultEvent::getType() const
{
    return d_p->type;
}

/*! \brief returns the current step, if this event represents a progress event
 *
 * @return the current step if the type is CuResultEvent::Progress
 */
int CuResultEvent::getStep() const
{
    return d_p->step;
}

/*! \brief returns the total number of steps, if this event represents a progress event
 *
 * @return the number of total steps if the type is CuResultEvent::Progress
 */
int CuResultEvent::getTotal() const
{
    return d_p->total;
}

/*! \brief returns the CuData stored in the event
 *
 * @return CuData bundle carrying the event data
 */
const CuData CuResultEvent::getData() const
{
    return d_p->data;
}

/*! \brief returns a pointer to the CuActivity that generated the event
 *
 * @return a pointer to the CuActivity that generated the event
 */
const CuActivity *CuResultEvent::getActivity() const
{
    return d_p->activity;
}

bool CuResultEvent::isList() const
{
    return d_p->is_list;
}

const std::vector<CuData> &CuResultEvent::getDataList() const
{
    return d_p->data_list;
}

/*! \brief an exit event for the sender activity
 *
 * @param sender the CuActivity that has to be finished
 *
 * Called by CuThread::publishExitEvent
 *
 */
CuActivityExitEvent::CuActivityExitEvent(CuActivity *sender)
{
    m_activity = sender;
}

/*! \brief returns an exit event type
 *
 * @return CuEventI::CuActivityExitEvent
 */
CuEventI::CuEventType CuActivityExitEvent::getType() const
{
    return CuEventI::CuActivityExitEvent;
}

/*! \brief returns a pointer to the activity that sent the exit event
 *
 * @return a pointer to the CuActivity that sent the exit event
 */
CuActivity *CuActivityExitEvent::getActivity() const
{
    return m_activity;
}
