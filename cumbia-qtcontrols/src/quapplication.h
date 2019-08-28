#ifndef QUAPPLICATION_H
#define QUAPPLICATION_H

#include <QApplication>

#include <qudbusplugininterface.h>

class QuApplicationPrivate;

/** \brief A QApplication with additional functions, such as to register with dbus, minimize and raise the main widget.
 *
 * \ingroup utils
 *
 * QuApplication derives from QApplication. If the cumbia-dbus plugin is available, it is used to register the
 * application with the DBus session bus. This allows the application to receive and send messages, so that it
 * can be used with an application browser that raises the window of a running application rather than launching
 * a new instance.
 *
 * See QuDBusPluginInterface documentation and the cumbia-dbus plugin
 *
 */
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

    QString display_host() const;

    int display_number() const;

    int screen_number() const;

    bool isPlatformX11() const;

signals:
    void dbusRegistered(const QString& exename, const QStringList& args, const QString& dbus_servicenam);
    void dbusUnregistered(const QString& exename, const QStringList& args, const QString& dbus_servicenam);

private:
    bool m_loadPlugin();

    QuApplicationPrivate *d;
};

#endif // QUAPPLICATION_H
