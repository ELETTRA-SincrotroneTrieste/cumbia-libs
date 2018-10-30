#include "cucontextmenu.h"
#include <cuapplicationlauncher.h>
#include <cumacros.h>
#include "quaction-extension-plugininterface.h"
#include "cucontextmenuactionsplugin_i.h"
#include <QAction>
#include <QtDebug>
#include <QDir>
#include <QPluginLoader>
#include <QMessageBox>

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
CuContextMenu::CuContextMenu(QWidget *parent, const CuContext *ctx) :
    QMenu(parent), m_ctx(ctx)
{
    unsigned loaded_p_cnt = 0;
    QList<QAction *> actions;
    QDir pluginsDir(CUMBIA_QTCONTROLS_PLUGIN_DIR);
    QStringList entryList = pluginsDir.entryList(QDir::Files);
    for(int i = 0; i < entryList.size(); i++) {
        QString fileName = entryList[i];
        CuContextMenuActionsPlugin_I *w_std_menu_actions_plugin = NULL;
        if(fileName.contains("context-menu-actions")) {

            QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
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
                     qstoc(fileName), CUMBIA_QTCONTROLS_PLUGIN_DIR);
            }
        } // if ends with context-menu-actions
    } // for entryList
    foreach(QAction *a, actions) {
        addAction(a);
    }

    if(loaded_p_cnt == 0) {
        addAction("No menu actions plugins found", this, SLOT(popup_noplugin_msg()));
    }
}

void CuContextMenu::popup_noplugin_msg()
{
    QMessageBox::information(this, "No plugins found for the context menu",
                             QString("No plugins providing context menu actions have been found under"
                                     "\"%1\"\n" ).arg(CUMBIA_QTCONTROLS_PLUGIN_DIR));
}

