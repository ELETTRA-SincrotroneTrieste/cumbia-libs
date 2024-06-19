#include "widgetstdcontextmenuactions.h"
#include "cucontext.h"
#include <quaction-extension-plugininterface.h>  // defined in cumbia-qtcontrols
#include <cucontrolsfactories_i.h>
#include <cucontrolsreader_abs.h>
#include <cucontrolswriter_abs.h>
#include <cucontexti.h>

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
    const CuContextI *m_ctxi;
    std::string m_msg;
    bool m_err;
};

WidgetStdContextMenuActions::WidgetStdContextMenuActions(QObject *parent) :
    QObject(parent)
{
    d = new WidgetStdContextMenuActionsPrivate;
    d->m_action_extensions = nullptr;
    d->m_ctxi = nullptr;
    d->m_widget = nullptr;
    d->m_err = false;
}

WidgetStdContextMenuActions::~WidgetStdContextMenuActions()
{
    printf("\e[1;31m~WidgetStdContextMenuActions\e[0m\n");
    delete d;
}

void WidgetStdContextMenuActions::setup(QWidget *widget, const CuContextI *ctx)
{
    qDebug () << __PRETTY_FUNCTION__ << widget << ctx;
    const char* extensions_plugin_name = "libactions-extension-plugin.so";
    d->m_ctxi = ctx;
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
                printf("\e[1;32m*\e[0m WidgetStdContextMenuActions: loaded plugin \e[1;32m\"%s\"\e[0m\n", qstoc(pluginFilePath));
                QuActionExtensionFactoryI *ae_fac = d->m_action_extensions->getExtensionFactory();
                QuActionExtensionI* tango_db_ex = ae_fac->create("GetTDbPropertyExtension", ctx);

                // 1. link stats
                QuActionExtensionI* infodlg_ex = ae_fac->create("InfoDialogExtension", ctx);
                if(infodlg_ex) {
                    QAction *info = findChild<QAction *>("linkStatsA");
                    if(!info) {
                        d->m_action_extensions->registerExtension("InfoDialogExtension", infodlg_ex);
                        info =    new QAction("Link stats", this);
                        info->setObjectName("linkStatsA");
                        connect(info, SIGNAL(triggered(bool)), this, SLOT(onInfoActionTriggered()));
                    }
                    d->m_actions << info;
                }
                // 2. helper application
                if(tango_db_ex) {
                    QAction *a = findChild<QAction *>("helperAppA");
                    if(!a) {
                        printf("\e[1;31mWidgetStdContextMenuActions::setup: action helperAppA not found, creating and connectin'\e[0m\n");
                        d->m_action_extensions->registerExtension("GetTDbPropertyExtension", tango_db_ex);
                        a = new QAction("Helper application", this);
                        a->setObjectName("helperAppA");
                        connect(tango_db_ex->get_qobject(), SIGNAL(onDataReady(const CuData&)), this, SLOT(onDataReady(const CuData&)));
                        connect(a, SIGNAL(triggered(bool)), this, SLOT(onHelperAActionTriggered()));
                    }
                    d->m_actions << a;
                }
                // 3. engine switch
                QuActionExtensionI* engineswe = ae_fac->create("EngineSwitchDialogExtension", ctx);
                if(engineswe) {
                    QAction *a = findChild<QAction *>("engineChangeA");
                    if(!a) {
                        printf("\e[1;31mWidgetStdContextMenuActions::setup: action engineChangeA not found, creating and connectin'\e[0m\n");
                        d->m_action_extensions->registerExtension("EngineSwitchDialogExtension", engineswe);
                        a = new QAction("Engine hot switch", this);
                        a->setObjectName("engineChangeA");
//                        connect(engineswe->get_qobject(), SIGNAL(onDataReady(const CuData&)), this, SLOT(onDataReady(const CuData&)));
                        connect(a, SIGNAL(triggered(bool)), this, SLOT(onEngineSwitchAction()));
                    }
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

int WidgetStdContextMenuActions::order() const
{
    return 0;
}

void WidgetStdContextMenuActions::onHelperAActionTriggered(const QString &source)
{
    QuActionExtensionI* tango_db_ex = d->m_action_extensions->getExtension("GetTDbPropertyExtension");
    if(tango_db_ex) {
        std::vector<CuData> in_datalist;
        TSource tsrc(source.toStdString());
        CuData din(TTT::Device, tsrc.getDeviceName());  // CuData din("device", tsrc.getDeviceName()
        din[TTT::Name] = std::string("helperApplication");  // din["name"]
        in_datalist.push_back(din);
        tango_db_ex->execute(in_datalist, d->m_ctxi);
    }
}

void WidgetStdContextMenuActions::onInfoActionTriggered()
{
    QuActionExtensionI* infodlg_ex = d->m_action_extensions->getExtension("InfoDialogExtension");
    qDebug() << __PRETTY_FUNCTION__ << "info dialog extension " << infodlg_ex << "widget" << d->m_widget;
    CuData in_par("sender", d->m_widget);
    infodlg_ex->execute(in_par, d->m_ctxi);
}

void WidgetStdContextMenuActions::onHelperAActionTriggered()
{
    if(d->m_action_extensions && d->m_ctxi->getContext()) {
        QString source;
        CuControlsReaderA *reader = d->m_ctxi->getContext()->getReader();
        CuControlsWriterA *writer = d->m_ctxi->getContext()->getWriter();
        if(reader)
            source = reader->source();
        else if(writer)
            source = writer->target();
        if(!source.isEmpty()) {
            onHelperAActionTriggered(source);
        }
    }
}

void WidgetStdContextMenuActions::onDataReady(const CuData &da)
{
    printf("Data received %s\n", da.toString().c_str());
    d->m_msg = da[TTT::Message].toString();  // da["msg"]
    d->m_err = da[TTT::Err].toBool();  // da["err"]
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
                                create(applauncher_ext, d->m_ctxi)));
                    }
                    if(app_launcher) {
                        std::string dev = prop.substr(0, prop.find(":helperApplication"));
                        if(dev.length()  > 0)
                            app += std::string(" ") + dev;
                        CuData in("command", app);
                        app_launcher->execute(in, d->m_ctxi);
                    }
                    else
                        perr("WidgetStdContextMenuActions::onDataReady: no CuApplicationLauncherExtension found in "
                             "extension factory");
                }
            }
        }
    }
}

void WidgetStdContextMenuActions::onEngineSwitchAction() {
    QuActionExtensionI* eng_switch_dlg_ex = d->m_action_extensions->getExtension("EngineSwitchDialogExtension");
    qDebug() << __PRETTY_FUNCTION__ << "engine switch dialog extension " << eng_switch_dlg_ex << "widget" << d->m_widget;
    CuData in_par("sender", d->m_widget);
    eng_switch_dlg_ex->execute(in_par, d->m_ctxi);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(widgets-std-context-menu-actions, WidgetStdContextMenuActions)
#endif // QT_VERSION < 0x050000
