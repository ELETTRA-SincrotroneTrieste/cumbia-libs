#ifndef CUMBIATANGO_H
#define CUMBIATANGO_H

class CuTangoActionFactoryI;
class CuDataListener;

#include <cumbia.h>
#include <cutangoactioni.h>
#include<string>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;

/** \mainpage
 * \a cumbia-tango integrates cumbia with the Tango control system framework, providing specialised Activities to read,
 * write attributes and impart commands.
 *
 * Implementation
 * The \a CumbiaTango class is an extension of the \a Cumbia base one. Its main task is managing the so called  \a actions.
 * An \a action represents a task associated to either a Tango device attribute or a command (called source).
 * Read, write, configure are the main sort of jobs an action can accomplish.
 * More types of actions are foreseen, such as multiple readings or writings in sequence.
 * \a CuTangoActionI defines the interface of an action. Operations include adding or removing data listeners,
 * starting and stopping an action, sending and getting data to and from the underlying thread (for example
 * retrieve or change the polling period of a source).
 *  \a CuTReader implements the interface and holds a reference to either an activity designed to receive events
 * from \a Tango or another intended to poll a source.
 *
 * \a Activities is where the \a Tango connection is setup, database is accessed for configuration, events are subscribed,
 * a poller is started or a write operation is performed. This is done inside the thread safe \a init, \a execute
 * and \a onExit methods, invoked from another thread.
 * Progress and results are forwarded by the \a publishProgress and \a publishResult methods in the activity
 * and received in the \a onProgress and \a onResult ones implemented by the action.
 * Therein, CuDataListener’s \a onUpdate method is invoked with the new data. Reception safely occurs in
 * the main thread.
 *
 * As previously stated, activities identified by the same token (a CuData object) belong to the same thread.
 * <em>cumbia-tango groups threads by Tango device name</em>.
 *
 * \par Tutorials
 * \li \ref md_lib_tutorial_activity
 *
 */

class CumbiaTango : public Cumbia
{

public:
    enum Type { CumbiaTangoType = Cumbia::CumbiaUserType + 1 };

    CumbiaTango();

    CumbiaTango(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    void setThreadFactoryImpl( CuThreadFactoryImplI *tfi);

    void setThreadEventsBridgeFactory( CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaTango();

    void addAction(const std::string& source, CuDataListener *l, const CuTangoActionFactoryI &f);

    void unlinkListener(const std::string& source, CuTangoActionI::Type t, CuDataListener *l);

    CuTangoActionI *findAction(const std::string& source, CuTangoActionI::Type t) const;

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    virtual int getType() const;

private:

    void m_init();

    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
};

#endif // CUMBIATANGO_H
