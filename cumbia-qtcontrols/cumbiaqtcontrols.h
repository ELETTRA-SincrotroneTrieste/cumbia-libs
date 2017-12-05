#ifndef CUMBIAQTCONTROLS_H
#define CUMBIAQTCONTROLS_H

#include "cumbia-qtcontrols_global.h"
#include <QMetaType>
#include <cudata.h>

/*! \mainpage Cumbia Qt controls module
 *
 * \section Introduction Introduction
 * This module combines cumbia to the \a Qt cross platform software framework, offering graphical control system components.
 * Labels, gauges and advanced graphs are supplied, as well as buttons and boxes to set values.
 * As mentioned earlier, elementary data representation is provided, due to the component unawareness of the \a cumbia
 * engine lying beneath. In order to display real data on the controls, you have to combine different building
 * blocks at the moment of setting up each reader or writer in your application, as described later.
 * When data is ready, it is delivered to the main thread through the onUpdate method that there must be in the control
 * component (such as a label), for it must implement the \a CuDataListener interface.
 * For an event loop must be executing, messages are posted to the main thread relying on an implementation of the
 * \a CuThreadsEventBridge_I interface. In \a Qt, we use \a QCoreApplication’s event loop in conjunction with \a cumbia-qtcontrols’
 * \a QThreadsEventBridge, which exploits \a QCoreApplication’s \a postEvent, a familiar scheme for \a Qt developers.
 * From within \a onUpdate, data is extracted and presented to the user by way of the control widget.
 *
 * Not strictly related to widgets themselves, but targeted to associate with them are a couple of abstract classes
 * that define an interface to readers and writers. They compel readers and writers to provide methods to set and
 * remove sources and targets of execution, as well as means to send and receive messages to and from actions.
 * \a CuControlsReaderA and \a CuControlsWriterA, that’s their names, keep also references to the currently active
 * \a Cumbia and data listener instances. Object composition to accomplish the set up of a \a Tango (\a Epics) reader (writer)
 * will be discussed in the \a cumbia-tango-controls documentation.
 *
 *
 * \section DeveloperObs Observations for developers
 *
 * \subsection Unlinking Unlinking a source from cumbia
 * Example: reader implemented in the cumbia-tango and qumbia-tango-controls modules.
 * Every Cumbia Qt controls widget holds a reference to a CuControlsReaderA object. The unsetSource method,
 * called from the main thread, is used to unlink a reader from cumbia.
 * Let's take QuLabel as an example. QuLabel is a graphical Qt element representing a text label to display
 * values. QuLabel implements the CuDataListener interface to receive updates from the underline reader.
 * QuLabel::unsetSource calls
 *
 * \subsubsection mainthread1 In the main thread
 * CuControlsReaderA::unsetSource on the specific CuControlsReaderA implementation.
 * \li CuTControlsReader is an implementation of CuControlsReaderA from the qumbia-tango-controls module.
 *     CuTControlsReader::unsetSource is called.
 * \li CuEpControlsReader is an implementation of CuControlsReaderA from the qumbia-epics-controls module.
 *     CuEpControlsReader::unsetSource is called.
 * These implementations call CumbiaTango::unlinkListener for  CuTangoActionI::AttConfig and
 * CuTangoActionI::Reader action types in a row.
 * CumbiaTango::unlinkListener performs a search among all the registered CuTangoActionI actions by name and type.
 * It then removes a listener from the action found, by calling removeDataListener on the CuTangoActionI.
 * In our example, CuTReader::removeDataListener is called.
 * The removeDataListener method of CuTReader removes the data listener from the list of listeners it holds.
 * This immediately prevents the listener to receive further data. In our example, the listener is QuLabel.
 *
 * \par notes
 * \li QuLabel will not be updated right after QuLabel::unsetSource.
 * \li CuTReader, holding a list of registered listeners, may decide to call CuTReader::stop if the list
 *     of listeners is empty. Actually, one CuTReader can update several listeners connected to the same
 *     source.
 *
 * CuTReader::stop calls Cumbia::unregisterActivity, passing the current activity as argument. In the case
 * of the Tango CuTReader, the activity can be either CuPollingActivity or CuEventActivity.
 * Cumbia::unregisterActivity fetches the thread where the activity belongs through the CuActivityManager
 * service and calls CuThread::unregisterActivity on it.
 * CuThread::unregisterActivity grabs the lock on its event queue and enqueues an event of type
 * UnRegisterActivityEvent, to be processed in CuThread's thread execution context.
 *
 * \subsubsection cuthread In the CuThread's thread
 * In CuThread's execution thread the UnRegisterActivityEvent holds a reference to the CuActivity that
 * has to be stopped. CuActivity::doOnExit is called in the CuThread's thread.
 * CuActivity::doOnExit is a <cite>template method</cite>: sets the activity flags (an exit flag is activated)
 * and calls the pure virtual method CuActivity::onExit. In our example, CuTReader can be executing a
 * CuPollingActivity or a CuEventActivity. In the second case, CuEventActivity::onExit is called.
 * As in every CuActivity::init, CuActivity::execute and CuActivity::onExit, the execution takes place in the
 * CuThread's thread. So, CuEventActivity::onExit is the place where <em>Tango</em> event unsubscription
 * takes place. As usual within CuActivity::init, CuActivity::execute and CuActivity::onExit, a CuData
 * object is prepared in order to hand results to a CuDataListener (our QuLabel) in the main thread.
 * As you can see from CuEventActivity::onExit, CuActivity::publishResult is finally invoked.
 *
 * CuActivity::publishResult receives the CuData that must be transmitted to the CuDataListener in the main
 * thread. Here, through the CuActivityManager::getThread, the CuThread where the activity belongs is
 * retrieved.  CuActivityManager is a service (CuServiceI), only one instance is available and registered
 * in Cumbia.
 *
 * CuActivityManager::getThread grabs a lock on its thread list and returns the thread to that
 * the activity was assigned. CuActivityManager's thread list is actually a multi map: more CuActivity
 * objects can live in the same thread. In the Tango example, the cumbia-tango module chose to assign
 * all activities referring to the same Tango device to the same CuThread. In other words distinct Tango devices,
 * different threads.
 *
 * When CuActivity::publishResult gets its thread, calls CuThread::publishResult on it.
 *
 * CuThread::publishResult relies on the installed CuThreadsEventBridge_I implementation to post an event
 * from the CuThread's secondary thread to the main thread. In a Qt application, the Qt event loop is normally
 * used and the QThreadsEventBridge will invoke QCoreApplication::postEvent on the QApplication so as to
 * deliver the CuData to the main thread (the "ui" thread).
 *
 * We are ready to go back to the main execution thread.
 *
 * \subsubsection mainthread2 (Back) In the main thread
 *
 * \note If instead of calling QuLabel::unsetSource the QuLabel had been directly deleted,
 * QuLabel::unsetSource would have destroyed CuTControlsReader, which in turn would have called
 * CuTControlsReader::unsetSource. <br/>
 * <em>At this time, QuLabel would not exist anymore, and not even the CuTControlsReader</em>. Anyway, the underline
 * activity is still there, receiving events from CuThread. They will simply not be delivered
 * to the deleted CuDataListener. <em>Also CuTReader is still alive</em>. CuTReader is deleted only after it
 * receives a CuData with the "exit" flag set to true from the CuThread, in CuTReader::onResult.
 *
 * At the end of the section <em>In the CuThread's thread</em> we left the CuThread's thread execution
 * with an event bridge (with QApplication event system) posting an event on the main thread. That's where
 * we are now.
 *
 * QThreadsEventBridge is a QObject, so it receives events received with QCoreApplication::postEvent
 * within the QThreadsEventBridge::event method. QThreadsEventBridge implements CuThreadsEventBridge_I
 * and holds a reference to CuThreadsEventBridgeListener, which interface in turn is implemented by
 * CuThread. QThreadsEventBridge::event invokes CuThread::onEventPosted
 *
 * The event carried by CuThread::onEventPosted contains the destination activity and the data for it.
 * Through the CuActivityManager::getThreadListeners method, the list of CuThreadListner is fetched.
 * We remember that CuTReader implements CuTangoActionI that implements CuThreadListener. So, our
 * CuTReader will receive the data coming from the CuThread through CuTReader::onProgress or
 * CuTReader::onResult. Let's consider the CuTReader::onResult case.
 *
 * CuTReader::onResult, and this is the end of the long story, delivers the CuData to all the registered
 * CuDataListener objects, like QuLabel in our discussion, through QuLabel::onUpdate.
 *
 * Inside QuLabel::onUpdate it is important to avoid making calls that could potentially modify in place
 * the list of registered CuDataListener. This means calling unsetSource (or setSource with the same
 * source name) is therein discouraged. Delay setSource or unsetSource somehow outside QuLabel::onUpdate.
 *
 *
 */
class CUMBIAQTCONTROLSSHARED_EXPORT CumbiaQtControls
{

public:
    CumbiaQtControls();
};

#endif // CUMBIAQTCONTROLS_H
