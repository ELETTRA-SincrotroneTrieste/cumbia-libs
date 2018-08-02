#ifndef QMLAPPSTATEMANAGER_H
#define QMLAPPSTATEMANAGER_H

#include <QObject>

class QmlAppStateManager : public QObject
{
    Q_OBJECT
public:
    explicit QmlAppStateManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // QMLAPPSTATEMANAGER_H