#include "cuevent.h"

CuResultEventPrivate::CuResultEventPrivate() {
    is_list = false;
    step = total = 0;
    type = CuEventI::CuResultEv;
}

CuResultEventPrivate::~CuResultEventPrivate() {
}


/*!
 * \brief CuResultEvent::CuResultEvent the class constructor
 * \param sender the CuActivity that originates the event
 * \param data the data to be delivered
 * \param t the type of event
 *
 * CuResultEvent makes a copy of data before delivering it to the event loop thread.
 */
CuResultEvent::CuResultEvent(const CuActivity* sender, const CuData &da, CuEventI::CuEventType t) : data(da) {
    d_p = new CuResultEventPrivate();
    d_p->type = t;
    d_p->activity = sender;
}


/*!
 * \brief CuResultEvent::CuResultEvent the class constructor
 * \param sender the CuActivity that originates the event
 * \param data_list a vector of data to be delivered
 * \param t the type of event
 *
 * data_list contents will be *moved* into a local copy to be used by the receiving thread
 */
CuResultEvent::CuResultEvent(const CuActivity* sender, const std::vector<CuData> &dali, CuEventType t) : datalist(std::move(dali) ){
    d_p = new CuResultEventPrivate();
    d_p->type = t;
    d_p->activity = sender;
    d_p->is_list = true;
}

/*!
 * \brief CuResultEvent::CuResultEvent class constructor with progress information
 * \param sender the CuActivity that originated the event (sender)
 * \param step the current step of a multi-step data processing
 * \param total the total steps of a multi-step data processing
 * \param data the data to be delivered
 */
CuResultEvent::CuResultEvent(const CuActivity* sender, int step, int total, const CuData &da) : data(da) {
    d_p = new CuResultEventPrivate();
    d_p->step = step;
    d_p->total = total;
    d_p->type = CuProgressEv;
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

CuA_ExitEv::CuA_ExitEv(CuActivity *sender) : m_activity(sender) {}

CuEventI::CuEventType CuA_ExitEv::getType() const {
    return CuEventI::CuA_ExitEvent;
}

CuActivity *CuA_ExitEv::getActivity() const {
    return m_activity;
}

CuA_UnregisterEv::CuA_UnregisterEv(CuActivity *sender) : m_activity(sender) {}

CuEventI::CuEventType CuA_UnregisterEv::getType() const {
    return CuEventI::CuA_UnregisterEv;
}

CuActivity *CuA_UnregisterEv::getActivity() const {
    return m_activity;
}

CuEventI::CuEventType CuTimerEvent::getType() const {
    return CuEventI::CuTmrEv;
}

