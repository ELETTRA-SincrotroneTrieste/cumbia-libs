#ifndef QUAPPLICATION_H
#define QUAPPLICATION_H

#include <QApplication>
#include <qudbusplugininterface.h>

class QuApplicationPrivate;
class CumbiaPool;
class CuControlsFactoryPool;

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
    /// NOTE
    ///
    /// Changes to this file require running
    /// $ qdbuscpp2xml src/quapplication.h -o ../qumbia-plugins/cumbia-dbus/src/cumbiadbus.xml
    /// Then rebuild cumbia-dbus
    ///

    Q_OBJECT
public:
    QuApplication(int & argc, char **argv);
    QuApplication(int &argc, char** argv, CumbiaPool *cu_p, CuControlsFactoryPool *fp);

    int exec();

    CumbiaPool *cumbiaPool() const;
    CuControlsFactoryPool *fpool() const;

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
    void dbusRegistered(const QString& exename, const QStringList& args, const QString& dbus_servicenam,
                        const QString& display_host, int display_number, int screen_number, bool is_plat_x11);
    void dbusUnregistered(const QString& exename, const QStringList& args, const QString& dbus_servicenam,
                          const QString& display_host, int display_number, int screen_number, bool is_plat_x11);

private:
    bool m_loadPlugin();

    void m_get_display_info(QString* host, int* d_num, int  *screen_num) const;

    QuApplicationPrivate *d;
};

#endif // QUAPPLICATION_H
