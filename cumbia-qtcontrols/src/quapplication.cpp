#include "quapplication.h"
#include <QDir>
#include <QPluginLoader>

#include <cumacros.h>
#include <QtDebug>

class QuApplicationPrivate
{
public:
    QuAppDBusInterface *dbus_i;
    QString cumbia_dbus_plugin;
};

QuApplication::QuApplication(int & argc, char **argv) : QApplication(argc, argv)
{
    d = new QuApplicationPrivate;
    m_loadPlugin();
}

void QuApplication::init()
{
    if(d->dbus_i)
        d->dbus_i->registerApp(applicationName());
    else
        perr("QuApplication.init: plugin %s is not loaded", qstoc(d->cumbia_dbus_plugin));
}

bool QuApplication::m_loadPlugin()
{
    QDir pluginsDir(CUMBIA_QTCONTROLS_PLUGIN_DIR);
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            d->dbus_i = qobject_cast<QuAppDBusInterface *>(plugin);
            if (d->dbus_i)
                return true;
        }
        else
            perr("QuApplication.m_loadPlugin: error loading plugin: %s", qstoc(pluginLoader.errorString()));
    }

    return false;
}
