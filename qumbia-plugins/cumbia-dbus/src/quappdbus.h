#ifndef QUAPPDBUS_H
#define QUAPPDBUS_H

#include <qudbusplugininterface.h>
#include <QString>

class QuApplication;

/** \brief An implementation of QuAppDBusInterface to register and unregister an application.
 *
 */
class QuAppDBus : public QObject, public QuAppDBusInterface
{
    Q_OBJECT
public:
    QuAppDBus(QObject* parent = NULL);

    // QuAppDBusInterface interface
public:
    void registerApp(QuApplication *app);
    void unregisterApp(QuApplication *app);
    QString getServiceName(QuApplication *app) const;

signals:
    void onAppRegistered(const QString &name, const QStringList &args, const QString& dbusServiceNam);

    void onAppUnregistered(const QString& name, const QStringList& args, const QString& dbusServiceNam);

private:

};

#endif // QUAPPDBUS_H
