#include "widgetstdcontextmenuactions.h"
#include <quaction-extension-plugininterface.h>  // defined in cumbia-qtcontrols
#include <cucontrolsfactories_i.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>
#include <cucontext.h>

#include <cudata.h>
#include <cumacros.h>

#include <cupluginloader.h>
#include <QDir>
#include <QPluginLoader>
#include <QtDebug>
#include <QProcessEnvironment>

// tango
#include <tsource.h>

class WidgetStdContextMenuActionsPrivate {
public:
    QuActionExtensionPluginInterface *m_action_extensions;
    QList<QAction *>m_actions;
    QWidget *m_widget;
    const CuContext *m_ctx;
    std::string m_msg;
    bool m_err;
};

WidgetStdContextMenuActions::WidgetStdContextMenuActions(QObject *parent) :
    QObject(parent)
{
    d = new WidgetStdContextMenuActionsPrivate;
    d->m_action_extensions = NULL;
    d->m_ctx = NULL;
    d->m_widget = NULL;
    d->m_err = false;
}

WidgetStdContextMenuActions::~WidgetStdContextMenuActions()
{
    printf("\e[1;31m~WidgetStdContextMenuActions\e[0m\n");
    delete d;
}

void WidgetStdContextMenuActions::setup(QWidget *widget, const CuContext *ctx)
{
    const char* extensions_plugin_name = "libactions-extension-plugin.so";
    d->m_ctx = ctx;
    d->m_widget = widget;
    d->m_actions.clear();

    CuPluginLoader pl;
    QString pluginFilePath = pl.getPluginAbsoluteFilePath(CUMBIA_QTCONTROLS_PLUGIN_DIR, extensions_plugin_name);
    if(!pluginFilePath.isEmpty()) {
        QPluginLoader pluginLoader(pluginFilePath);
        QObject *plugin = pluginLoader.instance();
        if(plugin) {
            d->m_action_extensions = qobject_cast<QuActionExtensionPluginInterface *>(plugin);
            if(d->m_action_extensions) {
                printf("WidgetStdContextMenuActions::WidgetStdContextMenuActions: loaded plugin \e[1;32m\"%s\"\e[0m\n", extensions_plugin_name);
                QuActionExtensionFactoryI *ae_fac = d->m_action_extensions->getExtensionFactory();
                QuActionExtensionI* tango_db_ex = ae_fac->create("GetTDbPropertyExtension", ctx);

                // 1. link stats
                QuActionExtensionI* infodlg_ex = ae_fac->create("InfoDialogExtension", ctx);
                if(infodlg_ex) {
                    d->m_action_extensions->registerExtension("InfoDialogExtension", infodlg_ex);
                    QAction *info = new QAction("Link stats", this);
                    connect(info, SIGNAL(triggered(bool)), this, SLOT(onInfoActionTriggered()));
                    d->m_actions << info;
                }
                // 2. helper application
                if(tango_db_ex) {
                    d->m_action_extensions->registerExtension("GetTDbPropertyExtension", tango_db_ex);
                    QAction *a = new QAction("Helper application", this);
                    connect(a, SIGNAL(triggered(bool)), this, SLOT(onHelperAActionTriggered()));
                    d->m_actions << a;
                }
            }
        }
    }
}

QList<QAction *> WidgetStdContextMenuActions::getActions() const
{
    return d->m_actions;
}

void WidgetStdContextMenuActions::onInfoActionTriggered()
{
    QuActionExtensionI* infodlg_ex = d->m_action_extensions->getExtension("InfoDialogExtension");
    CuData in_par("sender", d->m_widget);
    infodlg_ex->execute(in_par);
}

void WidgetStdContextMenuActions::onHelperAActionTriggered()
{
    if(d->m_action_extensions) {
        QString source;
        QuActionExtensionI* tango_db_ex = d->m_action_extensions->getExtension("GetTDbPropertyExtension");
        CuControlsReaderA *reader = d->m_ctx->getReader();
        CuControlsWriterA *writer = d->m_ctx->getWriter();
        if(reader)
            source = reader->source();
        else if(writer)
            source = writer->target();
        if(!source.isEmpty()) {
            std::vector<CuData> in_datalist;
            TSource tsrc(source.toStdString());
            CuData din("device", tsrc.getDeviceName());
            din["name"] = std::string("helperApplication");
            in_datalist.push_back(din);
            connect(tango_db_ex->get_qobject(), SIGNAL(onDataReady(const CuData&)), this, SLOT(onDataReady(const CuData&)));
            tango_db_ex->execute(in_datalist);
        }
        //        QStringList args = cmd.split(QRegExp("\\s+"));
        //        CuApplicationLauncher launcher(args);
        //        launcher.start();
    }
}

void WidgetStdContextMenuActions::onDataReady(const CuData &da)
{
    printf("Data received %s\n", da.toString().c_str());
    d->m_msg = da["msg"].toString();
    d->m_err = da["err"].toBool();
    if(d->m_err) {

    }
    else {
        std::vector<std::string> props = da["list"].toStringVector();
        if(props.size() > 0) {
            const std::string& prop = props[0];
            if(prop.rfind(":helperApplication") != std::string::npos) {
                // string ends with helperApplication
                std::string app = da[prop].toString();
                if(app.length() > 0 && d->m_action_extensions) {
                    // create launcher extension only if necessary
                    const char* applauncher_ext = "CuApplicationLauncherExtension";
                    QuActionExtensionI *app_launcher = d->m_action_extensions->getExtension(applauncher_ext);
                    if(!app_launcher) {
                        d->m_action_extensions->
                                registerExtension(applauncher_ext,
                                                  (app_launcher = d->m_action_extensions->getExtensionFactory()->
                                create(applauncher_ext, d->m_ctx)));
                        if(app_launcher) {
                            std::string dev = prop.substr(0, prop.find(":helperApplication"));
                            if(dev.length()  > 0)
                                app += std::string(" ") + dev;
                            CuData in("command", app);
                            app_launcher->execute(in);
                        }
                        else
                            perr("WidgetStdContextMenuActions::onDataReady: no CuApplicationLauncherExtension found in "
                                 "extension factory");
                    }

                }
            }
        }
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(widgets-std-context-menu-actions, WidgetStdContextMenuActions)
#endif // QT_VERSION < 0x050000
