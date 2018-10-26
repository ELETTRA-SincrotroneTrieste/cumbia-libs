#include "cucontextmenu.h"
#include <cuapplicationlauncher.h>
#include <cumacros.h>
#include "quaction-extension-plugininterface.h"
#include "cucontrolsfactories_i.h"
#include <QAction>
#include <QtDebug>
#include <QProcess>
#include <QDir>
#include <QPluginLoader>

/** \brief Creates a QMenu with a minimal set of actions.
 *
 * The following actions are created by the CuContextMenuConstructor:
 *
 * \li A Link statistics action, to request information about the underlying link.
 * \li A helper application action, if the parent stores a valid *helperApplication* property
 *
 * @param parent: the parent widget of this menu.
 * @param parent_as_cwi: the same object as parent must be passed. It must implement CuContextI
 *        interface.
 *
 * \par Note.
 * Since it is not possible to cast CuContextI to QWidget and vice versa, it is necessary
 * to store the parent reference both as QWidget and CuContextI.
 * The linkStatsTriggered signal will contain the CuContextI pointer that will be used
 * by the receiver to get the CuContext reference to access the cumbia-qtcontrols widget
 * context.
 */
CuContextMenu::CuContextMenu(QWidget *parent, CuContextI *parent_as_cwi) : QMenu(parent)
{
    QAction *info = new QAction("Link stats...", this);
    connect(info, SIGNAL(triggered(bool)), this, SLOT(onInfoActionTriggered()));
    addAction(info);
    m_parent_as_cwi = parent_as_cwi;

    printf("loading plugin...\n");
    QDir pluginsDir(CUMBIA_QTCONTROLS_PLUGIN_DIR);
    pluginsDir.cd("plugins");
    QString fileName = "libactions-extension-plugin.so";
    QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
    QObject *plugin = pluginLoader.instance();
    if (plugin) {
        m_action_extensions = qobject_cast<QuActionExtensionPluginInterface *>(plugin);
        if(m_action_extensions) {
            printf("loaded plugin \e[1;32m\"%s\"\e[0m\n", qstoc(fileName));
            QuActionExtensionFactoryI *ae_fac = m_action_extensions->getExtensionFactory();
            QuActionExtensionI* tango_db_ex = ae_fac->create("GetTDbPropertyExtension", parent_as_cwi);
            m_action_extensions->registerExtension("GetTDbPropertyExtension", tango_db_ex);
            QAction *a = new QAction("Helper Application", this);
            connect(a, SIGNAL(triggered(bool)), this, SLOT(onHelperAActionTriggered()));
            addAction(a);
        }
    }
    else
        perr("CuContextMenu::CuContextMenu: failed to load plugin \"%s\" under \"%s\"",
             qstoc(fileName), CUMBIA_QTCONTROLS_PLUGIN_DIR);
}

void CuContextMenu::onInfoActionTriggered()
{
    emit linkStatsTriggered(parentWidget(), m_parent_as_cwi);
}

void CuContextMenu::onHelperAActionTriggered()
{
    if(m_action_extensions) {
        QuActionExtensionI* tango_db_ex = m_action_extensions->getExtension("GetTDbPropertyExtension");
        CuData in("type", "get_property");
        connect(tango_db_ex->get_qobject(), SIGNAL(onDataReady(const CuData&)), this, SLOT(onDataReady(const CuData&)));
        tango_db_ex->execute(in);
//        QStringList args = cmd.split(QRegExp("\\s+"));
//        CuApplicationLauncher launcher(args);
//        launcher.start();
    }
}

void CuContextMenu::onDataReady(const CuData &da)
{
    printf("CuContextMenu::onDataReady(): \e[1;32mout is \"%s\"\e[0m\n", da.toString().c_str());
}
