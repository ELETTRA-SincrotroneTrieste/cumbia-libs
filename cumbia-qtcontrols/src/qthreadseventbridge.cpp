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
    const CuServiceProvider *service_provider;
    CuThreadsEventBridgeListener *bridgeListener;
};

QThreadsEventBridge::QThreadsEventBridge(const CuServiceProvider *sp)
{
    d = new QThreadsEventBridgePrivate;
    d->service_provider = sp;
    d->bridgeListener = NULL;
}

QThreadsEventBridge::~QThreadsEventBridge()
{
    pdelete("~QThreadsEventBridge %p", this);
    delete d;
}

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
