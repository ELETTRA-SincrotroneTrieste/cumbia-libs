#include "cucontextmenu.h"
#include <cuapplicationlauncher.h>
#include <cumacros.h>
#include "quaction-extension-plugininterface.h"
#include "cucontextmenuactionsplugin_i.h"
#include "cupluginloader.h"
#include <QAction>
#include <QtDebug>
#include <QDir>
#include <QPluginLoader>
#include <QMessageBox>

/** \brief Creates a QMenu with a set of actions loaded from qumbia-plugins (*libwidgets-std-context-menu-actions.so*)
 *         and additional user actions defined in custom plugins.
 *
 * Plese refer to the WidgetStdContextMenuActions class defined in qumbia-plugins/widgets-std-context-menu-actions plugin
 * for further details.
 *
 * The mentioned plugin loads some predefined actions, amongst which:
 *
 * \li A Link statistics action, to request information about the underlying link.
 * \li A helper application action, if the parent stores a valid *helperApplication* property
 *
 * @param parent: the parent widget of this menu.
 * @param ctx: a *const* pointer to a CuContext (the widget's CuContext)
 *
 * \par Custom actions
 * Custom actions can be provided by one or more additional plugins provided that:
 * \li they reside in a folder listed in the *CUMBIA_PLUGIN_PATH* environment variable (colon separated)
 * \li the name of the <cite>.so</cite> plugin library ends with <cite>context-menu-actions.so</cite>.
 * The list of actions is populated starting with those provided by custom plugins within the *CUMBIA_PLUGIN_PATH*
 * and finally with the ones provided by *libwidgets-std-context-menu-actions.so*.
 *
 */
CuContextMenu::CuContextMenu(QWidget *parent, const CuContext *ctx) :
    QMenu(parent), m_ctx(ctx)
{
    unsigned loaded_p_cnt = 0;
    QList<QAction *> actions;
    CuPluginLoader cupl;
    QStringList pluginPaths = cupl.getPluginAbsoluteFilePaths(CUMBIA_QTCONTROLS_PLUGIN_DIR, QRegExp(".*context-menu-actions\\.so"));
    for(int i = 0; i < pluginPaths.size(); i++) {
        CuContextMenuActionsPlugin_I *w_std_menu_actions_plugin = NULL;
        QPluginLoader pluginLoader(pluginPaths[i]);
        QObject *plugin = pluginLoader.instance();
        if (plugin){
            loaded_p_cnt++;
            w_std_menu_actions_plugin = qobject_cast<CuContextMenuActionsPlugin_I *> (plugin);
            if(w_std_menu_actions_plugin) {
                w_std_menu_actions_plugin->setup(parent, ctx);
                actions.append(w_std_menu_actions_plugin->getActions());
            }
        }
        if(!plugin || !w_std_menu_actions_plugin){
            perr("CuContextMenu::CuContextMenu: failed to load plugin \"%s\" under \"%s\"",
                 qstoc(pluginPaths[i]), CUMBIA_QTCONTROLS_PLUGIN_DIR);
        }
    } // for pluginPaths
    foreach(QAction *a, actions) {
        addAction(a);
    }

    if(loaded_p_cnt == 0) {
        addAction("No menu actions plugins found", this, SLOT(popup_noplugin_msg()));
    }
}

CuContextMenu::~CuContextMenu()
{

}

void CuContextMenu::popup_noplugin_msg()
{
    QMessageBox::information(this, "No plugins found for the context menu",
                             QString("No plugins providing context menu actions have been found under"
                                     "\"%1\"\n" ).arg(CUMBIA_QTCONTROLS_PLUGIN_DIR));
}

