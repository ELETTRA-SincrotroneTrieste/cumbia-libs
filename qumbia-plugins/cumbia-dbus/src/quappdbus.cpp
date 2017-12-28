#include "quappdbus.h"
#include <quapplication.h>
#include <cumacros.h>
#include <QtDebug>
#include <QDBusConnection>
#include <QApplication>
#include <QDBusError>
#include <errno.h>
#include <QUrl>
#include <QStringList>
#include <unistd.h>
#include "cumbiadbus_adaptor.h"

QuAppDBus::QuAppDBus(QObject *parent) : QObject(parent)
{
    qDebug() << __FUNCTION__ << "constructor: parent" << parent;
}

/** \brief Register a QuApplication on the DBus service.
 *
 * @param app a reference to the QuApplication to be registered.
 *
 * The application is registered with a service name formed by:
 * "eu.elettra." + hostname + ".display" + $DISPLAY  + ".quapplication.pid" + getpid() + "."  + appname
 *
 * The getServiceName method provides the service name string.
 *
 * @see unregisterApp
 *
 */
void QuAppDBus::registerApp(QuApplication *app)
{
    QStringList args = app->arguments();
    args.removeFirst();
    new QuAppDBusInterfaceAdaptor(app);
    QDBusConnection connection = QDBusConnection::sessionBus();
    QString serviceName = getServiceName(app);
    bool ret = connection.registerObject("/QuApplication", app);
    if(ret)
        ret = connection.registerService(serviceName);
    if(!ret)
        perr("QuAppDBus.registerApp: failed to register app \%s\" with DBus (\"%s\"): %s [%s]",
             qstoc(app->applicationName()), qstoc(getServiceName(app)),
             qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));
    if(ret)
        emit onAppRegistered(app->arguments().first(), args, serviceName);
}

/** \brief unregister the QuApplication from the DBus service
 *
 * Unregisters the application with the service name returned by getServiceName, which is the
 * same method used by registerApp in the registration phase.
 *
 * @see registerApp
 */
void QuAppDBus::unregisterApp(QuApplication *app)
{
    QStringList args = app->arguments();
    args.removeFirst();
    emit onAppUnregistered(app->arguments().first(), args, getServiceName(app));
}

QString QuAppDBus::getServiceName(QuApplication* app) const
{
    QString dbus_servicenam;
    char hostname[HOST_NAME_MAX] = "unknown_host";
    char *display;
    QString qsdisplay;
    QString appname;
    QStringList params(app->arguments());
    params.removeFirst();
    appname = app->arguments().at(0);
    if(appname.contains("/"))
        appname = appname.split("/", QString::SkipEmptyParts).last();

    if(gethostname(hostname, HOST_NAME_MAX))
        perr("QuAppDBus.m_makeServiceName: hostname unavailable");

    display = getenv("DISPLAY");
    if(!display)
        printf("QuAppDBus.m_makeServiceName: DISPLAY env variable unavailable");
    else
        qsdisplay = QString(display).remove(QRegExp("\\.\\d*")).remove(":");
    dbus_servicenam = "eu.elettra." + QString(hostname) + ".display" + qsdisplay  +
             ".quapplication.pid" + QString::number(getpid()) + "."  + appname ;

    printf("QuAppDBus.m_makeServiceName: service name \"%s\"\n", qstoc(dbus_servicenam));
    return dbus_servicenam;
}

