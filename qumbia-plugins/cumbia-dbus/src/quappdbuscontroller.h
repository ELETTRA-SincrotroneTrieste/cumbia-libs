#ifndef QAPPDBUSCONTROLLER_H
#define QAPPDBUSCONTROLLER_H

#include <QObject>
#include <QList>
#include <qudbusplugininterface.h>

class QDBusConnectionInterface;
class QuAppDBusControllerPrivate;

class QuAppDBusController : public QObject, public QuAppDBusControllerInterface
{
    Q_OBJECT
public:
    explicit QuAppDBusController(QObject *parent = nullptr);

    virtual ~QuAppDBusController();

    int closeAll(const QList<QuAppInfo> &ail);

    QStringList args_noPath(const QStringList& args) const;

    // QuAppDBusControllerInterface interface
    void start_monitor(const QString& serviceName);

    void stop_monitor(const QString& serviceName);

    void addCtrlListener(QuAppDBusControllerListener *l);

    void removeCtrlListener(QuAppDBusControllerListener *l);

public slots:
    void onAppRegistered(const QString& exenam, const QStringList &args, const QString& dbus_servicenam);

    void onAppUnregistered(const QString& exenam, const QStringList &args, const QString& dbus_servicenam);

private:
    QList<QuAppInfo> findApps(const QStringList &args = QStringList());


    QDBusConnectionInterface *m_getDbusConnectionInterface();

    QuAppDBusControllerPrivate *d;

};

#endif // QAPPDBUSCONTROLLER_H
