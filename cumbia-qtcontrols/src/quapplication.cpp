#include "quapplication.h"
#include "qudbusplugininterface.h"
#include <QDir>
#include <QPluginLoader>

#include <cumacros.h>
#include <QtDebug>
#include <QWidget>
#include <QWindow>

class QuApplicationPrivate
{
public:
    QuAppDBusInterface *dbus_i;
    QString cumbia_dbus_plugin;
};

QuApplication::QuApplication(int & argc, char **argv) : QApplication(argc, argv)
{
    d = new QuApplicationPrivate;
    d->dbus_i = NULL;
    m_loadPlugin();
}

int QuApplication::exec()
{
    if(d->dbus_i)
    {
        d->dbus_i->registerApp(this, applicationName());
        emit dbusRegistered(exename(), cmdOpt(), d->dbus_i->getServiceName(this));
    }
    else
        perr("QuApplication.exec: plugin %s is not loaded", qstoc(d->cumbia_dbus_plugin));

    int ret = QApplication::exec();
    if(d->dbus_i)
    {
        d->dbus_i->unregisterApp(this, applicationName());
        emit dbusUnregistered(exename(), cmdOpt(), d->dbus_i->getServiceName(this));
        delete d->dbus_i;
    }
    return ret;
}

void QuApplication::raise()
{
    foreach(QWindow *w, topLevelWindows())
        w->raise();
}

void QuApplication::minimize()
{
    foreach(QWindow *w, topLevelWindows())
        w->showMinimized();
}

void QuApplication::quit()
{
    QApplication::quit();
}

QStringList QuApplication::arguments() const
{
    return QApplication::arguments();
}

QString QuApplication::exename() const
{
    QString n = QApplication::arguments().first();
    if(n.count('/') > 0)
        return n.section('/', -1);
    return n;
}

QStringList QuApplication::cmdOpt() const
{
    QStringList ar = QApplication::arguments();
    ar.removeFirst();
    return ar;
}

bool QuApplication::m_loadPlugin()
{
    QDir pluginsDir(CUMBIA_QTCONTROLS_PLUGIN_DIR);
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            QuDBusPluginInterface *dpi = qobject_cast<QuDBusPluginInterface *>(plugin);
            if(dpi)
                d->dbus_i = dpi->getAppIface();
            if (d->dbus_i)
                return true;
        }
        else
            perr("QuApplication.m_loadPlugin: error loading plugin: %s", qstoc(pluginLoader.errorString()));
    }

    return false;
}
