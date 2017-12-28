#ifndef CUMBIADBUSPLUGIN_H
#define CUMBIADBUSPLUGIN_H

#include <QObject>
#include <QtPlugin>

#include "quapplication.h"
#include "qudbusplugininterface.h"

class CumbiaDBusPlugin : public QObject, QuDBusPluginInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.qudbus.QuDBusPluginInterface" FILE "cumbiadbusplugin.json")
#endif
    Q_INTERFACES(QuDBusPluginInterface)
public:
    explicit CumbiaDBusPlugin(QObject *parent = NULL);


    // QuDBusPluginInterface interface
public:
    QuAppDBusInterface *getAppIface() const;
    QuAppDBusControllerInterface *getAppCtrlIface() const;
};

#endif // CUMBIADBUSPLUGIN_H
