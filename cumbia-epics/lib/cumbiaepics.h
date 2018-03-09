#ifndef CUMBIAEPICS_H
#define CUMBIAEPICS_H

class CuEpicsActionFactoryI;
class CuDataListener;

#include <cumbia.h>
#include "cuepactioni.h"
#include<string>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;

/*! \mainpage Cumbia module for the Epics control system
 *
 * *cumbia-epics* is the cumbia module for the <a href="https://epics.anl.gov/">Experimental
 * Physics and Industrial Control System</a> (EPICS) control system.
 *
 * At the moment <strong>only a monitor (reader) has been implemented</strong>.
 *
 * \par Example
 * In the qumbia-epics-controls module, under the *examples* directory, you will find an
 * example of CumbiaEpics usage. It is completely equivalent to the *cumbia/tango*
 * counterpart.
 *
 * See <a href="../../qumbia-epics-controls/html/index.html">qumbia-epics-controls</a> documentation.
 */

/*! \brief Cumbia implementation over the EPICS control system
 *
 * \subsubsection Implementation
 * The \a CumbiaEpics class is an extension of the \a Cumbia base one.
 * Its main task is managing the so called  \a actions.
 * An \a action represents a task associated to an EPICS *pv* (called source).
 * Presently, reading from EPICS is the only action that can be accomplished by *cumbia-epics*.
 * More types of actions are foreseen, such as a writer implementation.
 * \a CuEpActionI defines the interface of an action. Operations include adding or removing data listeners,
 * starting and stopping an action, sending and getting data to and from the underlying thread (for example
 * retrieve or change the polling period of a source).
 * \a CuMonitor implements the interface and holds a reference to an activity designed to receive events
 * from \a EPICS.
 *
 *
 */
class CumbiaEpics : public Cumbia
{

public:
    enum Type { CumbiaEpicsType = Cumbia::CumbiaUserType + 1 };

    CumbiaEpics();

    CumbiaEpics(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    void setThreadFactoryImpl( CuThreadFactoryImplI *tfi);

    void setThreadEventsBridgeFactory( CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaEpics();

    void addAction(const std::string& source, CuDataListener *l, const CuEpicsActionFactoryI &f);

    void unlinkListener(const std::string& source, CuEpicsActionI::Type t, CuDataListener *l);

    CuEpicsActionI *findAction(const std::string& source, CuEpicsActionI::Type t) const;

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    virtual int getType() const;

private:

    void m_init();

    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
};

#endif // CUMBIAEPICS_H
