#ifndef QTHREADSEVENTBRIDGE_H
#define QTHREADSEVENTBRIDGE_H

#include "cuthreadseventbridge_i.h"
#include <QObject>

class CuServiceProvider;

class QThreadsEventBridgePrivate;

class QThreadsEventBridge : public QObject, public CuThreadsEventBridge_I
{
    Q_OBJECT
public:
    QThreadsEventBridge(const CuServiceProvider *sp);

    virtual ~QThreadsEventBridge();

protected:
    bool event(QEvent *event);

    // ThreadsEventBridge_I interface
public:
    void postEvent(CuEventI *e);

    void setCuThreadsEventBridgeListener(CuThreadsEventBridgeListener *l);
private:
    QThreadsEventBridgePrivate *d;
};

#endif // QTHREADSEVENTBRIDGE_H
