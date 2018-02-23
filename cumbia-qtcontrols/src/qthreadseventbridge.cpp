#include "qthreadseventbridge.h"
#include "cuevent_qt.h"
#include "cuserviceprovider.h"
#include "cuactivitymanager.h"
#include "cuevent_qt.h"
#include "cumacros.h"
#include "cuthreadlistener.h"

#include <QCoreApplication>

class QThreadsEventBridgePrivate
{
public:
    CuThreadsEventBridgeListener *bridgeListener;
};

/*! \brief the class constructor
 *
 * Constructs a new instance of the object
 */
QThreadsEventBridge::QThreadsEventBridge()
{
    d = new QThreadsEventBridgePrivate;
    d->bridgeListener = NULL;
}

/*! \brief the class destructor
 *
 * class destructor
 */
QThreadsEventBridge::~QThreadsEventBridge()
{
    pdelete("~QThreadsEventBridge %p", this);
    delete d;
}

/** \brief receives a QEvent from QApplication and delivers the contents to a
 *         registered CuThreadsEventBridgeListener on the *main thread*.
 *
 * @param event a QEvent that wraps a CuEventI.
 *
 * If the QEvent::type is QEvent::User + 101, then QEvent is safely casted to a
 * CuEvent_Qt object. CuEvent_Qt contains a CuEventI instance, that is delivered
 * to a registered CuThreadsEventBridgeListener.
 *
 * @return true if the event is a CuEvent_Qt
 * @return QObject::event(event) otherwise
 *
 */
bool QThreadsEventBridge::event(QEvent *event)
{
    pr_thread();
    if(event->type() == QEvent::User + 101) /* publish result event */
    {
        CuEvent_Qt *re = static_cast<CuEvent_Qt *>(event);
        CuEventI *ei = re->getEvent();
        if(d->bridgeListener)
            d->bridgeListener->onEventPosted(ei);
        else
            perr("QThreadsEventBridge.event: no event bridge listener installed!");

        return true;
    }
    else
        return QObject::event(event);
}

/*! \brief set the event bridge listener on the bridge
 *
 * @param l a CuThreadsEventBridgeListener, such as CuThread
 *
 * When an event is delivered through the QApplication event loop, (QThreadsEventBridge::event)
 * CuThreadsEventBridgeListener::onEventPosted is called on the main
 * thread.
 */
void QThreadsEventBridge::setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener* l)
{
    d->bridgeListener = l;
}

/*! \brief posts an event through QApplication::postEvent, normally *from
 *         the background thread to the foreground*
 *
 * @param e a CuEventI
 *
 * \par examples of functions calling this method from the *background
 *      thread*:
 * \li CuThread::publishResult
 * \li CuThread::publishExitEvent
 * \li CuThread::publishProgress
 *
 */
void QThreadsEventBridge::postEvent(CuEventI *e)
{
    CuEvent_Qt *qe = new CuEvent_Qt(e);
    qApp->postEvent(this, qe);
}
