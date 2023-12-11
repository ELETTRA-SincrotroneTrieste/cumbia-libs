#ifndef CUTHREADSEVENTBRIDGE_H
#define CUTHREADSEVENTBRIDGE_H

#include <cuthreadseventbridgefactory_i.h>
#include <cuthreadseventbridge_i.h>
#include <cumacros.h>
#include <cueventloop.h>

class CuThreadsEventBridgeListener;
class CuServiceProvider;
class CuEventI;

/*! \brief cumbia basic thread event bridge
 *
 * A *thread event bridge* provides
 * an interface to deliver events from a background thread to an event loop running
 * in another thread.
 *
 * This class is usually instantiated by a CuThreadsEventBridgeFactory_I *threads
 * bridge factory implementation*. A CuThreadsEventBridgeFactory_I implementation
 * is passed as argument in Cumbia::registerActivity.
 *
 * If you are developing graphical applications using qt, you should consider
 * adopting the Qt implementation in the cumbia-qtcontrols module:
 *
 * \li QThreadsEventBridge: Qt bridge that relies on QApplication and QApplication::postEvent.
 *     This is the *first choice for Qt applications.* The factory that is used
 *     in Cumbia::registerActivity is QThreadsEventBridgeFactory.
 *
 * \par Example
 * \code
    m_cumbia = new Cumbia();
    CuData thread_token(CuDType::Thread, "athread");  // CuData thread_token("thread", "athread")
    Activity *a = new Activity(CuData("name", "my_activity"));
    m_cumbia->registerActivity(a, this, thread_token,
                               CuThreadFactoryImpl(),
                               QThreadsEventBridgeFactory());
 * \endcode
 *
 * @implements CuThreadsEventBridge_I
 * @implements CuEventLoopListener
 */
class CuThreadsEventBridge : public CuThreadsEventBridge_I, public CuEventLoopListener
{
public:
    CuThreadsEventBridge(const CuServiceProvider *sp);

    virtual ~CuThreadsEventBridge();

    // CuThreadsEventBridge_I interface
    void postEvent(CuEventI *e);

    void setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener *l);

    // CuEventLoopListener interface
    void onEvent(CuEventI *e);

private:
    const CuServiceProvider *m_service_provider;
    CuThreadsEventBridgeListener *m_bridgeListener;
    CuEventLoopService *m_event_loop;
};

/*! \brief an implementation of CuThreadsEventBridgeFactory_I for cumbia
 *
 * This factory is used to instantiate a new CuThreadsEventBridge_I implementation,
 * i.e. a new CuThreadsEventBridge
 *
 * If developing Qt applications, consider using Qt's own event loop
 * provided by QApplication. See the QThreadsEventBridgeFactory class
 * in cumbia-qtcontrols module.
 *
 */
class CuThreadsEventBridgeFactory : public CuThreadsEventBridgeFactory_I
{
    // CuThreadsEventBridgeFactory_I interface
public:
    CuThreadsEventBridge_I *createEventBridge(const CuServiceProvider *sp) const;
};

#endif // CUTTHREADSEVENTBRIDGE_H
