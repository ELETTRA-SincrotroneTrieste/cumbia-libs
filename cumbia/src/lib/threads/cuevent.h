#ifndef CUEVENT_H
#define CUEVENT_H

#include <vector>
#include "cudata.h"

class CuActivity;
class CuUserData;

/*! \brief interface for a generic *cumbia event* designed to be
 *         generated in a secondary thread (CuActivity's thread - CuThread)
 *         and delivered into the main thread through an event loop.
 *
 * This class is used internally.
 *
 * The CuEventType enum defines some default event types used internally
 * by the library and lets the user extend the types.
 *
 * The pure virtual method getType must be reimplemented in subclasses to
 * provide *rtti* for event type conversion at runtime.
 *
 */
class CuEventI
{
public:
    virtual ~CuEventI() {}

    enum CuEventType { CuProgressEv = 0, CuResultEv, CuA_UnregisterEv, CuA_ExitEvent,
                       CuExitLoopEv, CuThAutoDestroyEv, CuTmrEv, User = 100 };

    /*! \brief returns the event type
     *
     * @return a value from the enum CuEventType or a user-defined value
     *
     * This method is used for event type conversion at runtime (rtti).
     */
    virtual CuEventType getType() const = 0;
};

/** \brief This event is used by CuEventLoop service to exit the loop
 *
 * This class is used internally.
 */
class CuExitLoopEvent : public CuEventI
{
    // CuEventI interface
public:

    /*!
     * \brief getType returns the exit loop event type
     * \return CuEventI::ExitLoop
     */
    CuEventType getType() const { return CuEventI::CuExitLoopEv; }
};

/*! \brief at the end of the thread event loop (CuThread::run), this
 *         event is posted. When received, the thread is auto destroyed
 *
 */
class CuThreadAutoDestroyEvent : public CuEventI {
    /*!
     * \brief getType returns the thread auto destroy event type
     * \return CuEventI::ThreadAutoDestroy
     */
    CuEventType getType() const { return CuEventI::CuThAutoDestroyEv; }
};

/*! @private */
class CuResultEventPrivate
{
public:
    CuResultEventPrivate();
    ~CuResultEventPrivate();

    CuEventI::CuEventType type;
    const CuActivity *activity;
    int step, total;
    bool is_list;
};

/*!
 * \brief The CuResultEvent class stores a result that a CuActivity (sender)
 *        wants to deliver from the background to the main thread
 *
 * This is used internally by the library
 *
 * The CuResultEvent is usually extracted in the CuThread::onEventPosted
 * method. It is used to get the exchanged data and the pointer to the
 * activity that sent the event
 *
 * @implements CuEventI
 */
class CuResultEvent : public CuEventI
{
public:
    CuResultEvent(const CuActivity* sender, const CuData &data, CuEventType t = CuResultEv);
    CuResultEvent(const CuActivity* sender, const std::vector<CuData> &dali, CuEventType t = CuResultEv);
    CuResultEvent(const CuActivity* sender, int step, int total, const CuData &data);
    CuResultEvent(const CuActivity *sender, const CuUserData* data, CuEventType t = CuResultEv);

    virtual ~CuResultEvent();

    CuData data;
    std::vector<CuData> datalist;
    const CuUserData *u_data;

    // CuResultEventI interface
public:
    void setType(CuEventType t);
    CuEventType getType() const;
    int getStep() const;
    int getTotal() const;
    const CuActivity *getActivity() const;
    bool isList() const;
    bool has_user_data() const;

protected:
    CuResultEventPrivate *d_p;
};

/*! \brief exit event for the activity
 *
 * This is used internally by the library
 *
 * used from
 * \li CuThread::onEventPosted
 * \li CuThread::publishExitEvent
 */
class CuA_ExitEv : public CuEventI {
public:
    CuA_ExitEv(CuActivity *sender);
    CuEventType getType() const;
    CuActivity *getActivity() const;

private:
    CuActivity *m_activity;
};

class CuA_UnregisterEv : public CuEventI {
public:
    CuA_UnregisterEv(CuActivity *sender);
    CuEventType getType() const;
    CuActivity *getActivity() const;
private:
    CuActivity *m_activity;
};

class CuTimerEvent : public CuEventI {
    // CuEventI interface
public:
    CuEventType getType() const;
};

#endif // CUELOOPEVENT_H
