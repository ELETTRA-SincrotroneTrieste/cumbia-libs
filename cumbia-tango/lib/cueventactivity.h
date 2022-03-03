#ifndef CUEVENTACTIVITY_H
#define CUEVENTACTIVITY_H

#include <cuactivity.h>
#include <cuactivityevent.h>
#include <tango.h>

class CuEventActivityPrivate;
class CuDeviceFactoryService;
class TSource;

/*! @private
 */
class CuTAStopEvent : public CuActivityEvent
{
public:


    // CuActivityEvent interface
public:
    Type getType() const;
};

/*! \brief an activity to subscribe and receive events from Tango. Implements CuActivity and Tango::Callback
 *         interfaces
 *
 * Implementing CuActivity, the work is done in the background by the three methods
 *
 * \li init
 * \li execute
 * \li onExit
 *
 * They are called from a separate thread. In cumbia-tango, threads are grouped by device.
 * As an implementor of Tango::Callback, events are received through the push_event callback
 *
 * Apart from the three execution hooks listed above, some other methods have been written
 * to comply with the CuActivityInterface:
 *
 * \li getType, returning CuEventActivityType
 * \li event, to receive CuActivityEvent events
 * \li matches, that returns true if the input token represents an activity like this
 * \li repeat, that in this case always returns false
 *
 * \par note
 * To solve a known Tango issue that prevents events from working if the client application subscribes
 * to sources across different devices, an instance of omni_thread::ensure_self is kept alive as long as
 * the CuEventActivity lives.
 *
 */
class CuEventActivity : public CuActivity, public Tango::CallBack
{
public:
    enum Type { CuEventActivityType = CuActivity::User + 2 };

    CuEventActivity(const TSource& tso, CuDeviceFactoryService *df, const std::string &refreshmo, const CuData& tag);
    ~CuEventActivity();

    // CuActivity interface
public:
    int getType() const;
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;
    int repeat() const;

protected:
    void init();
    void execute();
    void onExit();

    void push_event(Tango::EventData *);

private:
    CuEventActivityPrivate *d;

    Tango::EventType m_tevent_type_from_string(const std::string& set) const;
};

#endif // CUEVENTACTIVITY_H
