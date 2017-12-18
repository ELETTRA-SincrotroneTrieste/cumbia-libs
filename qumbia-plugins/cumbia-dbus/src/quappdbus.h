#ifndef QUAPPDBUS_H
#define QUAPPDBUS_H

#include <qudbusplugininterface.h>
#include <QString>

class QuApplication;

class QuAppDBus : public QObject, public QuAppDBusInterface
{
    Q_OBJECT
public:
    QuAppDBus(QObject* parent = NULL);

    // QuAppDBusInterface interface
public:
    void registerApp(QuApplication *app, const QString &key);
    void unregisterApp(QuApplication *app, const QString& key);
    QString getServiceName(QuApplication *app) const;

signals:
    void onAppRegistered(const QString &name, const QStringList &args, const QString& dbusServiceNam);

    void onAppUnregistered(const QString& name, const QStringList& args, const QString& dbusServiceNam);

private:

};

#endif // QUAPPDBUS_H
