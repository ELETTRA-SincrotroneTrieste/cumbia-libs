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

/*! the class constructor
 *
 * @param sp a pointer to a CuServiceProvider
 */
QThreadsEventBridge::QThreadsEventBridge()
{
    d = new QThreadsEventBridgePrivate;
    d->bridgeListener = NULL;
}

QThreadsEventBridge::~QThreadsEventBridge()
{
    pdelete("~QThreadsEventBridge %p", this);
    delete d;
}

/** \brief receives a QEvent from QApplication and delivers the contents to a
 *         registered CuThreadsEventBridgeListener.
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

void QThreadsEventBridge::setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener* l)
{
    d->bridgeListener = l;
}

void QThreadsEventBridge::postEvent(CuEventI *e)
{
    CuEvent_Qt *qe = new CuEvent_Qt(e);
    qApp->postEvent(this, qe);
}
