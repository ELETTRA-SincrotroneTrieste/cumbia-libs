#include "cuapplicationlauncherextension.h"
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
    const CuContext*  ctx;
    std::string msg;
    bool err;
};

CuApplicationLauncherExtension::CuApplicationLauncherExtension(const CuContext *ctx)
{
    d = new CuApplicationLauncherPrivate;
    d->qudbusctrl_i = NULL;
    d->ctx = ctx;
    d->err = false;
}

CuApplicationLauncherExtension::~CuApplicationLauncherExtension()
{
    if(d->qudbusctrl_i) {
        printf("~CuApplicationLauncherExtension: deleting QuDBusPluginInterface %p\n", d->qudbusctrl_i);
        delete d->qudbusctrl_i;
    }
    delete d;
}

void CuApplicationLauncherExtension::start()
{
    if(!d->qudbusctrl_i)
        m_loadCumbiaPlugin();

    if(d->qudbusctrl_i)
    {
        QStringList full_args = QStringList() << d->program;
        full_args += d->args;
        QList<QuAppInfo> ail = d->qudbusctrl_i->findApps(full_args);
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

void CuApplicationLauncherExtension::m_loadCumbiaPlugin() {

    QDir pluginsDir(QUMBIA_PLUGIN_DIR);
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


QString CuApplicationLauncherExtension::getName() const {
    return "CuApplicationLauncherExtension";
}

CuData CuApplicationLauncherExtension::execute(const CuData &in)
{
    QString cmd = QString::fromStdString(in["command"].toString());
    QStringList ar = cmd.split(QRegExp("\\s+"));
    if(ar.size() > 0) {
        d->program  = ar.takeFirst();
        d->args = ar;
    }
    start();
    return CuData();
}

QObject *CuApplicationLauncherExtension::get_qobject() {
    return NULL;
}

const CuContext *CuApplicationLauncherExtension::getContext() const {
    return d->ctx;
}


std::vector<CuData> CuApplicationLauncherExtension::execute(const std::vector<CuData>& )
{
    return std::vector<CuData>();
}


std::string CuApplicationLauncherExtension::message() const {
    return d->msg;
}

bool CuApplicationLauncherExtension::error() const {
    return d->err;
}
