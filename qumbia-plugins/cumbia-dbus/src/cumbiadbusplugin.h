#ifndef CUMBIADBUSPLUGIN_H
#define CUMBIADBUSPLUGIN_H

#include <QObject>
#include <QtPlugin>

#include "quapplication.h"

class CumbiaDBusPlugin : public QObject, QuAppDBusInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "eu.elettra.cumbia-qtcontrols.QuAppDBusInterface" FILE "cumbiadbusplugin.json")
    Q_INTERFACES(QuAppDBusInterface)
public:
    explicit CumbiaDBusPlugin(QObject *parent = nullptr);

signals:

public slots:

    // QuAppDBusInterface interface
public:
    void registerApp(const QString &key);
    void unregisterApp(const QString &key);
};

#endif // CUMBIADBUSPLUGIN_H
