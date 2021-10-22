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

/** \brief the class constructor
 *
 * @param ctx a pointer to CuContext. The default is NULL, the parameter is not used
 * by this class.
 */
CuApplicationLauncherExtension::CuApplicationLauncherExtension(const CuContext *ctx) {
    d = new CuApplicationLauncherPrivate;
    d->qudbusctrl_i = NULL;
    d->ctx = ctx;
    d->err = false;
}

CuApplicationLauncherExtension::~CuApplicationLauncherExtension() {
    // do not delete plugin instances
    delete d;
}

void CuApplicationLauncherExtension::start() {
    if(!d->qudbusctrl_i)
        m_loadCumbiaPlugin();

    if(d->qudbusctrl_i) {
        printf("CuApplicationLauncherExtension::start d->qudbusctrl_i is not null, OK\n");
        QStringList full_args = QStringList() << d->program;
        full_args += d->args;
        QList<QuAppInfo> ail = d->qudbusctrl_i->findApps(full_args);
        if(ail.size() > 0) {
            d->qudbusctrl_i->raise(ail.first());
            printf("CuApplicationLauncherExtension::start d->qudbusctrl_i->rais %s %s\n");
        }
        else {
            printf("CuApplicationLauncherExtension::start QProcess.startDetached %s %s\n", qstoc(d->program), qstoc(d->args.join(" ")));
            QProcess::startDetached(d->program, d->args);
        }
    }
}

void CuApplicationLauncherExtension::m_loadCumbiaPlugin() {
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
            perr("CuApplicationLauncherExtension.m_loadCumbiaPlugin: error loading plugin: %s", qstoc(pluginLoader.errorString()));
    }
    if(d->qudbusctrl_i)
        printf("\e[1;32m* \e[0mCuApplicationLauncherExtension: successfully loaded the \e[1;32;4mcumbia dbus\e[0m plugin\n");
}


QString CuApplicationLauncherExtension::getName() const {
    return "CuApplicationLauncherExtension";
}

CuData CuApplicationLauncherExtension::execute(const CuData &in, const CuContext *ctx) {
    d->ctx = ctx;
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


std::vector<CuData> CuApplicationLauncherExtension::execute(const std::vector<CuData>& , const CuContext *) {
    return std::vector<CuData>();
}

std::string CuApplicationLauncherExtension::message() const {
    return d->msg;
}

bool CuApplicationLauncherExtension::error() const {
    return d->err;
}
