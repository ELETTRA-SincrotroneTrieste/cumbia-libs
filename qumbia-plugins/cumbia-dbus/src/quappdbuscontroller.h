#ifndef QAPPDBUSCONTROLLER_H
#define QAPPDBUSCONTROLLER_H

#include <QObject>
#include <QList>
#include <qudbusplugininterface.h>

class QDBusConnectionInterface;
class QuAppDBusControllerPrivate;

/** \brief This class implements the QuAppDBusControllerInterface and can be used
 *         as a proxy to communicate to a cumbia application (QuApplication) using
 *         DBus
 *
 * This class provides a very basic functionality to close an application, to raise its
 * window (window manager dependent behaviour), monitor an application subscription to
 * the DBus server.
 *
 * A QuAppDBusControllerListener implementation can be registered to QuAppDBusController
 * in order to receive notifications when an application is registered and unregistered from DBus.
 * start_monitor and stop_monitor have to be called in order to receive notifications
 * by QuAppDBusControllerListener.
 *
 * @see QuAppDBus
 *
 */
class QuAppDBusController : public QObject, public QuAppDBusControllerInterface
{
    Q_OBJECT
public:
    explicit QuAppDBusController(QObject *parent = nullptr);

    virtual ~QuAppDBusController();

    void close(const QuAppInfo &ai);

    void raise(const QuAppInfo &ai);

    QStringList args_noPath(const QStringList& args) const;

    // QuAppDBusControllerInterface interface
    void start_monitor(const QString& serviceName);

    void stop_monitor(const QString& serviceName);

    void addCtrlListener(QuAppDBusControllerListener *l);

    void removeCtrlListener(QuAppDBusControllerListener *l);

public slots:
    void onAppRegistered(const QString& exenam, const QStringList &args, const QString& dbus_servicenam,
                         const QString &display_h, int disp_num, int screen_num, bool is_plat_x11);

    void onAppUnregistered(const QString& exenam, const QStringList &args, const QString& dbus_servicenam,
                           const QString& display_h, int disp_num, int screen_num, bool is_plat_x11);

private:
    QList<QuAppInfo> findApps(const QStringList &args = QStringList());


    QDBusConnectionInterface *m_getDbusConnectionInterface();

    QuAppDBusControllerPrivate *d;

};

#endif // QAPPDBUSCONTROLLER_H
