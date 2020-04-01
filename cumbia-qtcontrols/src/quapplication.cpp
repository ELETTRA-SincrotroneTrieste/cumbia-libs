#include "quapplication.h"
#include "plugin_ifaces/qudbusplugininterface.h"
#include <QDir>
#include <QPluginLoader>
#include <unistd.h> // gethostname

#include <cupluginloader.h>
#include <cumacros.h>
#include <QtDebug>
#include <QWidget>
#include <QWindow>

#ifdef QT_X11EXTRAS_LIB
#include <QtX11Extras/QX11Info>
#endif

class QuApplicationPrivate
{
public:
    QuAppDBusInterface *dbus_i;
    QString cumbia_dbus_plugin;
};

/** \brief The class constructor.
 *
 * QuApplication is build the very same way as QApplication.
 * Just replace QApplication with QuApplication in your main.cpp
 *
 */
QuApplication::QuApplication(int & argc, char **argv) : QApplication(argc, argv)
{
    d = new QuApplicationPrivate;
    d->dbus_i = NULL;
    d->cumbia_dbus_plugin = "QuDBusPluginInterface";
    m_loadPlugin();
}

/** \brief calls exec on QApplication after registering with the DBus session bus
 *         if the qumbia-dbus plugin is available.
 *
 * @return the result returned by QApplication::exec
 *
 * If the QPluginLoader can load the QuDBusPluginInterface plugin, then the
 * application is registered with DBus.
 * The dbusRegistered signal is emitted, then QApplication::exec is called.
 * After QApplication::exec returns, dbusUnregistered signal is emitted,
 * the plugin is released and the result of QApplication::exec is returned.
 *
 * See QApplication::exec
 */
int QuApplication::exec()
{
    QString disp_h;
    int disp_n, screen_n;
    m_get_display_info(&disp_h, &disp_n, &screen_n);
    if(d->dbus_i) {
        d->dbus_i->registerApp(this);
        emit dbusRegistered(exename(), cmdOpt(), d->dbus_i->getServiceName(this), disp_h, disp_n, screen_n, isPlatformX11());
    }
    else
        perr("QuApplication.exec: plugin \"%s\" is not loaded", qstoc(d->cumbia_dbus_plugin));

    int ret = QApplication::exec();
    if(d->dbus_i) {
        d->dbus_i->unregisterApp(this);
        emit dbusUnregistered(exename(), cmdOpt(), d->dbus_i->getServiceName(this), disp_h, disp_n, screen_n, isPlatformX11());
        delete d->dbus_i;
    }
    return ret;
}

/** \brief raises the top level windows
 *
 * Foreach QWindow w, calls  w->raise
 */
void QuApplication::raise()
{
    foreach(QWindow *w, topLevelWindows())
        w->raise();
}

/** \brief minimizes the top level windows
 *
 * Foreach QWindow w, calls  w->showMinimized
 */
void QuApplication::minimize()
{
    foreach(QWindow *w, topLevelWindows())
        w->showMinimized();
}

/*! \brief simply calls QApplication::quit
 *
 */
void QuApplication::quit()
{
    QApplication::quit();
}

/*! \brief returns QApplication::arguments()
 *
 * @return QApplication::arguments()
 */
QStringList QuApplication::arguments() const
{
    return QApplication::arguments();
}

/*! \brief returns the executable name
 *
 * @return the executable name obtained by QApplication::arguments().first()
 *         from where the path is removed.
 *
 * \par example
 * If the application is /usr/local/bin/qumbiaprojectwizard, exename returns
 * *qumbiaprojectwizard*.
 */
QString QuApplication::exename() const
{
    QString n = QApplication::arguments().first();
    if(n.count('/') > 0)
        return n.section('/', -1);
    return n;
}

/*! returns the list of the command line options
 *
 * @return a QStringList taken from QApplication::arguments without the first
 *         element.
 */
QStringList QuApplication::cmdOpt() const
{
    QStringList ar = QApplication::arguments();
    ar.removeFirst();
    return ar;
}

QString QuApplication::display_host() const
{
    QString disp = QString(getenv("DISPLAY"));
    QRegExp re("([A-Za-z_0-9\\.\\-]*):(\\d*)[\\.]{0,1}(\\d*)");
    if(re.indexIn(disp) > -1 && re.capturedTexts().size() > 1) {
        return re.capturedTexts().at(1);
    }
    return "";
}

int QuApplication::display_number() const
{
    QString disp = QString(getenv("DISPLAY"));
    QRegExp re("([A-Za-z_0-9\\.\\-]*):(\\d*)[\\.]{0,1}(\\d*)");
    if(re.indexIn(disp) > -1 && re.capturedTexts().size() > 2) {
        return re.capturedTexts().at(2).toInt();
    }
    return 0;
}

int QuApplication::screen_number() const
{
    QString disp = QString(getenv("DISPLAY"));
    QRegExp re("([A-Za-z_0-9\\.\\-]*):(\\d*)[\\.]{0,1}(\\d*)");
    if(re.indexIn(disp) > -1 && re.capturedTexts().size() > 3) {
        return re.capturedTexts().at(3).toInt();
    }
    return 0;
}

bool QuApplication::isPlatformX11() const
{
#ifdef QT_X11EXTRAS_LIB
    return QX11Info::isPlatformX11();
#else
    return false;
#endif
}

/*! @private
 *
 * load the QuDBusPluginInterface using the QPluginLoader
 */
bool QuApplication::m_loadPlugin()
{
    QuDBusPluginInterface *dpi = NULL;
    const char* plugin_name = "libcumbia-dbus-plugin.so";
    QString filePath = CuPluginLoader().getPluginAbsoluteFilePath(CUMBIA_QTCONTROLS_PLUGIN_DIR, plugin_name);
    QPluginLoader pluginLoader(filePath);
    QObject *plugin = pluginLoader.instance();
    if (plugin) {
        dpi = qobject_cast<QuDBusPluginInterface *>(plugin);
        if(dpi)
            d->dbus_i = dpi->getAppIface();
        if (d->dbus_i)
            return true;
    }
    else
        perr("QuApplication.m_loadPlugin: error loading plugin \"%s\": %s",
             plugin_name, qstoc(pluginLoader.errorString()));

    if(!dpi)
        perr("QuApplication.m_loadPlugin: error loading plugin interface \"QuDBusPluginInterface\"");
    if(!d->dbus_i)
        perr("QuApplication.m_loadPlugin: error getting QuAppDBusInterface");
    if(!plugin || !dpi || !d->dbus_i)
        perr("QuApplication.m_loadPlugin: file \"%s\" is searched under %s and the \"CUMBIA_PLUGIN_PATH\" environment variable",
             plugin_name, CUMBIA_QTCONTROLS_PLUGIN_DIR);

    return false;
}

void QuApplication::m_get_display_info(QString *host, int *d_num, int *screen_num) const
{
    *host = ""; *d_num = *screen_num = 0;
    QString disp = QString(getenv("DISPLAY"));
    QRegExp re("([A-Za-z_0-9\\.\\-]*):(\\d*)[\\.]{0,1}(\\d*)");
    const int pos = re.indexIn(disp);
    const QStringList& caps = re.capturedTexts();
    if(pos > -1 && caps.size() > 1)
        *host = caps[1];
    if(pos > -1 && caps.size() > 2)
        *d_num = caps[2].toInt();
    if(pos > -1 && caps.size() > 3)
        *screen_num = caps[3].toInt();
}
