#include "quappdbuscontroller.h"
#include "quappdbus.h"
#include "cumbiadbus_interface.h"
#include <QDBusReply>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QList>
#include <cumacros.h>

#define SERVICE_NAME "eu.elettra.quappdbus.controller"

class QuAppDBusControllerPrivate
{
public:
    QDBusConnectionInterface *dbus_if;
    QList<QuAppDBusControllerListener *> ctrl_listeners;
};

QuAppDBusController::QuAppDBusController(QObject *parent) : QObject(parent)
{
    d = new QuAppDBusControllerPrivate;
    d->dbus_if  = NULL;
    d->dbus_if = m_getDbusConnectionInterface();
}

QuAppDBusController::~QuAppDBusController()
{
    if(d->dbus_if)
       d->dbus_if->unregisterService(SERVICE_NAME);
    delete d;
}

QStringList QuAppDBusController::args_noPath(const QStringList &args) const
{
    QStringList ret;
    if(args.size() > 0)
    {
        QString exe = args.first();
        if(exe.count('/') > 0)
#if QT_VERSION >= QT_VERSION_CHECK(5.15.0)
            exe = exe.split('/', Qt::SkipEmptyParts).last();
#else
            exe = exe.split('/', QString::SkipEmptyParts).last();
#endif
        ret << exe;
    }
    for(int i = 1; i < args.size(); i++)
        ret << args[i];
    return ret;
}

/** Find information about a cumbia-qtcontrols QuApplication currently in execution with the given arguments
 *
 * @param args a the application arguments, including argv[0].
 *        If the first argument contains the full path, this is discarded, so that only the executable
 *        name is taken into account.
 *        The following arguments are argv[1], argv[2], ...
 *        If args is an empty list, then all applications registered as <em>quapplication</em> in the
 *        session bus are returned.
 *
 * @return a list of QuAppInfo with the list of applications matching args
 *
 */
QList<QuAppInfo> QuAppDBusController::findApps(const QStringList &search_app_args)
{
    QList<QuAppInfo>  il;
    if(!d->dbus_if)
        return il;

    QDBusReply<QStringList> services = d->dbus_if->registeredServiceNames();
    if(services.isValid())
    {
        /* populate the refreshed information list */
        foreach(QString service, services.value())
        {
            if(service.contains("quapplication"))
            {
                LocalQuApplicationInterface quappdbusi(service,  "/QuApplication", QDBusConnection::sessionBus(), this);
                if(quappdbusi.isValid())
                {
                    /* retrieve pid from service */
                    QDBusReply<QStringList> argsReply = quappdbusi.arguments();
                    QString display_host = quappdbusi.display_host();
                    int display_number = quappdbusi.display_number();
                    int screen_number = quappdbusi.screen_number();
                    bool is_plat_x11 = quappdbusi.isPlatformX11();
                    if(!argsReply.isValid())
                        perr("QuAppDBusController.findApps: reply error: %s", qstoc(argsReply.error().message()));
                    else
                    {
                        QStringList in_args(search_app_args);
                        if(in_args.size() > 0 && in_args.first().count("/") > 0)
                            in_args = args_noPath(in_args);
                        QStringList arguments = args_noPath(argsReply.value());
                        if(search_app_args.isEmpty() || arguments == args_noPath(search_app_args))
                            il.append(QuAppInfo(arguments, service, display_host, display_number, screen_number, is_plat_x11));
                    }
                }
            }
        }
    }
    return il;
}

void QuAppDBusController::close(const QuAppInfo &ai)
{
    LocalQuApplicationInterface quappdbusi(ai.dbusServiceName(),  "/QuApplication", QDBusConnection::sessionBus(), this);
    quappdbusi.quit();
}

void QuAppDBusController::raise(const QuAppInfo &ai)
{
    LocalQuApplicationInterface quappdbusi(ai.dbusServiceName(),  "/QuApplication", QDBusConnection::sessionBus(), this);
    quappdbusi.raise();
}

void QuAppDBusController::start_monitor(const QString &serviceName)
{
    LocalQuApplicationInterface *quappdbusi =
            new LocalQuApplicationInterface(serviceName,  "/QuApplication", QDBusConnection::sessionBus(), this);
    connect(quappdbusi, SIGNAL(dbusRegistered(QString,QStringList,QString,QString,int,int,bool)), this,
            SLOT(onAppRegistered(QString,QStringList,QString,QString,int,int,bool)));
    connect(quappdbusi, SIGNAL(dbusUnregistered(QString,QStringList,QString,QString,int,int,bool)), this,
            SLOT(onAppUnregistered(QString,QStringList,QString,QString,int,int,bool)));
    quappdbusi->setObjectName(serviceName);
    qDebug() << __FUNCTION__ << "mapped signals to monitor " << serviceName;
    if(!d->dbus_if)
        return;

    QDBusReply<QStringList> services = d->dbus_if->registeredServiceNames();
    int idx = services.value().indexOf(serviceName);
    if(idx > -1)
    {
        LocalQuApplicationInterface quappdbusi(serviceName,  "/QuApplication", QDBusConnection::sessionBus(), this);
        if(quappdbusi.isValid())
        {
            /* retrieve pid from service */
            QDBusReply<QStringList> argsReply = quappdbusi.arguments();
            if(!argsReply.isValid())
                perr("QuAppDBusController.findApps: reply error: %s", qstoc(argsReply.error().message()));
            else
            {
                QStringList arguments = args_noPath(argsReply.value());
                QString disp_host = quappdbusi.display_host();
                int disp_n = quappdbusi.display_number();
                int screen_n = quappdbusi.screen_number();
                bool is_plat_x11 = quappdbusi.isPlatformX11();
                QuAppInfo ai(arguments, serviceName, disp_host, disp_n, screen_n, is_plat_x11);
                onAppRegistered(ai.exename(), ai.args(), serviceName, disp_host, disp_n, screen_n, is_plat_x11);
            }
        }
    }
}

void QuAppDBusController::stop_monitor(const QString &serviceName)
{
    LocalQuApplicationInterface *quappdbusi = findChild<LocalQuApplicationInterface *>(serviceName);
    if(quappdbusi)
    {
        qDebug() << __FUNCTION__ << "deleting monitored interface " << quappdbusi->objectName();
        delete quappdbusi;
    }
}

void QuAppDBusController::addCtrlListener(QuAppDBusControllerListener *l)
{
    d->ctrl_listeners.append(l);
}

void QuAppDBusController::removeCtrlListener(QuAppDBusControllerListener *l)
{
    d->ctrl_listeners.removeAll(l);
}

void QuAppDBusController::onAppRegistered(const QString &exenam, const QStringList &args, const QString &dbus_servicenam,
                                          const QString& display_h, int disp_num, int screen_num, bool is_plat_x11)
{
    qDebug() << "QuAppDBusController::" << __FUNCTION__ << exenam << args << dbus_servicenam;
    QuAppInfo ai(exenam, args, dbus_servicenam, display_h, disp_num, screen_num, is_plat_x11);
    foreach(QuAppDBusControllerListener *l, d->ctrl_listeners)
        l->onAppRegistered(ai);
}

void QuAppDBusController::onAppUnregistered(const QString &exenam, const QStringList &args, const QString &dbus_servicenam,
                                            const QString& display_h, int disp_num, int screen_num, bool is_plat_x11)
{
    qDebug() << __FUNCTION__ << exenam << args << dbus_servicenam;
    QuAppInfo ai(exenam, args, dbus_servicenam, display_h, disp_num, screen_num, is_plat_x11);
    foreach(QuAppDBusControllerListener *l, d->ctrl_listeners)
        l->onAppUnregistered(ai);
}

QDBusConnectionInterface *QuAppDBusController::m_getDbusConnectionInterface()
{
    if(d->dbus_if)
        return d->dbus_if;

    QDBusConnection connection = QDBusConnection::sessionBus();

    if(connection.isConnected()) {
    }
    else if(!connection.registerService(SERVICE_NAME)) {
        perr("QuAppDBusController: failed to register service \"%s\": %s: %s", SERVICE_NAME, qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));
    }
    else if(!connection.registerObject("/QuAppDBusController", this))
        perr("QuAppDBusController: failed to register object with path \"%s\" %s: %s:", SERVICE_NAME, qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));

    if(connection.isConnected()) {
        d->dbus_if = connection.interface();
    }

    return d->dbus_if;
}
