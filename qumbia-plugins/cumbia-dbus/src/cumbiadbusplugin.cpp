#include "cumbiadbusplugin.h"
#include "quappdbus.h"
#include "quappdbuscontroller.h"
#include <cumacros.h>
#include <QtDebug>
#include <QDBusConnection>
#include <QDBusError>
#include <errno.h>
#include <QUrl>
#include <QStringList>
#include <unistd.h>
#include "cumbiadbus_adaptor.h"

CumbiaDBusPlugin::CumbiaDBusPlugin(QObject *parent) : QObject(parent)
{
}

QuAppDBusInterface *CumbiaDBusPlugin::getAppIface() const
{
    return new QuAppDBus();
}

QuAppDBusControllerInterface *CumbiaDBusPlugin::getAppCtrlIface() const
{
    return new QuAppDBusController();
}

