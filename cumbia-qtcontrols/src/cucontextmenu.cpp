#include "cucontextmenu.h"
#include <cumacros.h>
#include <cucontext.h>
#include <cucontexti.h>
#include "plugin_ifaces/quaction-extension-plugininterface.h"
#include "plugin_ifaces/cucontextmenuactionsplugin_i.h"
#include "cupluginloader.h"
#include <QAction>
#include <QMultiMap>
#include <QtDebug>
#include <QDir>
#include <QPluginLoader>
#include <QMessageBox>

class CuContextMenuPrivate {
public:
    QObject *plugin;
};

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
CuContextMenu::CuContextMenu(QWidget *parent) :
    QMenu(parent)
{
    d = new CuContextMenuPrivate;
    d->plugin = nullptr;
}

CuContextMenu::~CuContextMenu() {
    delete d;
}

void CuContextMenu::prepare(const CuContextI *ctxi) {
    if(!d->plugin) {
        unsigned loaded_p_cnt = 0;
        QMultiMap<int, QList<QAction *> > actions_map;
        CuPluginLoader cupl;
        QStringList pluginPaths = cupl.getPluginAbsoluteFilePaths(CUMBIA_QTCONTROLS_PLUGIN_DIR, QRegularExpression(".*context-menu-actions\\.so"));
        for(int i = 0; i < pluginPaths.size(); i++) {
            CuContextMenuActionsPlugin_I *w_std_menu_actions_plugin = NULL;
            QPluginLoader pluginLoader(pluginPaths[i]);
            d->plugin = pluginLoader.instance();
            if (d->plugin && ctxi){
                loaded_p_cnt++;
                w_std_menu_actions_plugin = qobject_cast<CuContextMenuActionsPlugin_I *> (d->plugin);
                if(w_std_menu_actions_plugin) {
                    w_std_menu_actions_plugin->setup(parentWidget(), ctxi);
                    QList<QAction *> pl_actions = w_std_menu_actions_plugin->getActions();
                    actions_map.insert(w_std_menu_actions_plugin->order(), pl_actions);
                }
            }
            if(!d->plugin || !w_std_menu_actions_plugin){
                perr("CuContextMenu::CuContextMenu: failed to load plugin \"%s\" under \"%s\"",
                     qstoc(pluginPaths[i]), CUMBIA_QTCONTROLS_PLUGIN_DIR);
            }
        } // for pluginPaths

        QMultiMap<int, QList<QAction *> >::iterator i = actions_map.begin();
        while(i != actions_map.end()) {
            insertActions(nullptr, i.value());
            ++i;
        }

        if(loaded_p_cnt == 0) {
            addAction("No menu actions plugins found", this, SLOT(popup_noplugin_msg()));
        }
    }
    else if(d->plugin && qobject_cast<CuContextMenuActionsPlugin_I *> (d->plugin)) {
        qobject_cast<CuContextMenuActionsPlugin_I *> (d->plugin)->setup(parentWidget(), ctxi);
    }
}

void CuContextMenu::popup(const QPoint &pos, const CuContextI *ctxi) {
    prepare(ctxi);
    QMenu::popup(pos);
}

void CuContextMenu::popup_noplugin_msg()
{
    QMessageBox::information(this, "No plugins found for the context menu",
                             QString("No plugins providing context menu actions have been found under"
                                     "\"%1\"\n" ).arg(CUMBIA_QTCONTROLS_PLUGIN_DIR));
}

