#include "cuapplicationlauncher.h"
#include <QDir>
#include <QPluginLoader>
#include <QProcess>
#include <qudbusplugininterface.h>
#include <cumacros.h>

class CuApplicationLauncherPrivate {
public:
    QString program;
    QStringList args;
    QuAppDBusControllerInterface *qudbusctrl_i;
};

CuApplicationLauncher::CuApplicationLauncher(const QStringList &args)
{
    d = new CuApplicationLauncherPrivate;
    QStringList ar(args);
    if(args.size() > 0) {
        d->program  = ar.takeFirst();
        d->args = ar;
    }
    d->qudbusctrl_i = NULL;
}

void CuApplicationLauncher::start()
{
    m_loadCumbiaPlugin();
    if(d->qudbusctrl_i)
    {
        QStringList full_args = QStringList() << d->program;
        full_args += d->args;
        printf("calling findApps!\n");
        QList<QuAppInfo> ail = d->qudbusctrl_i->findApps(full_args);
        printf("found apps %d\n", ail.size());
        if(ail.size() > 0) {
            printf("found %d applications matching %s\n", ail.size(), qstoc(full_args.join(" ")));
            d->qudbusctrl_i->raise(ail.first());
        }
        else {
            QProcess p;
            p.setProgram(d->program);
            p.setArguments(d->args);
            p.startDetached();
        }
    }
}

void CuApplicationLauncher::m_loadCumbiaPlugin() {

    QDir pluginsDir(CUMBIA_QTCONTROLS_PLUGIN_DIR);
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            QuDBusPluginInterface *dpi = qobject_cast<QuDBusPluginInterface *>(plugin);
            if(dpi)
                d->qudbusctrl_i = dpi->getAppCtrlIface();
        }
        else
            perr("BrowserTreeDBusExtension.m_loadCumbiaPlugin: error loading plugin: %s", qstoc(pluginLoader.errorString()));
    }
    if(d->qudbusctrl_i)
        printf("\e[1;32m* \e[0msuccessfully loaded the \e[1;32;4mcumbia dbus\e[0m plugin\n");
}
