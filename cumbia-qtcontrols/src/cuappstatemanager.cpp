#include "cuappstatemanager.h"
#include <QGuiApplication>

CuAppStateManager::CuAppStateManager(QGuiApplication *app, QObject *root) : QObject(app)
{
    m_root = root;
    connect(app, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));
}

void CuAppStateManager::addSuspendable(QObject *suspendable)
{
    m_suspendables.append(suspendable);
}

void CuAppStateManager::removeSuspendable(QObject *suspendable)
{
    m_suspendables.removeAll(suspendable);
}

void CuAppStateManager::onApplicationStateChanged(Qt::ApplicationState st)
{
    switch(st) {
    case Qt::ApplicationHidden:
    case Qt::ApplicationInactive:
    case Qt::ApplicationSuspended:
        foreach(QObject * o, m_root->findChildren<QObject *>()) {
            if(o->metaObject()->indexOfMethod(metaObject()->normalizedSignature("suspend()")) > -1)
                QMetaObject::invokeMethod(o, "suspend");
        }
        foreach(QObject* suspendable, m_suspendables)
            QMetaObject::invokeMethod(suspendable, "suspend");
        break;
    case Qt::ApplicationActive:
        foreach(QObject * o, m_root->findChildren<QObject *>()) {
            if(o->metaObject()->indexOfMethod(metaObject()->normalizedSignature("start()")) > -1)
                QMetaObject::invokeMethod(o, "start");
        }
        foreach(QObject* suspendable, m_suspendables)
            QMetaObject::invokeMethod(suspendable, "start");
        break;
    default:
        break;
    }
}
