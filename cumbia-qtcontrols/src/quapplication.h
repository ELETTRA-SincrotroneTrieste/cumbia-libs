#ifndef QUAPPLICATION_H
#define QUAPPLICATION_H

#include <QApplication>

#include <qudbusplugininterface.h>

class QuApplicationPrivate;


class QuApplication : public QApplication
{
    Q_OBJECT
public:
    QuApplication(int & argc, char **argv);

    int exec();

public slots:
    void raise();

    void minimize();

    void quit();

    QStringList arguments() const;

    QString exename() const;

    QStringList cmdOpt() const;

signals:
    void dbusRegistered(const QString& exename, const QStringList& args, const QString& dbus_servicenam);
    void dbusUnregistered(const QString& exename, const QStringList& args, const QString& dbus_servicenam);

private:
    bool m_loadPlugin();

    QuApplicationPrivate *d;
};

#endif // QUAPPLICATION_H
