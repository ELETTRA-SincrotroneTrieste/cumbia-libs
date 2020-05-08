#ifndef WIDGETSTDCONTEXTMENUACTIONS_H
#define WIDGETSTDCONTEXTMENUACTIONS_H

#include <cucontextmenuactionsplugin_i.h>
#include <QObject>

class QuActionExtensionPluginInterface;
class WidgetStdContextMenuActionsPrivate;
class CuData;
class CuContext;

/*! \mainpage
 *
 *  Standard context menu actions for cumbia-qtcontrols widgets plugin provides basic
 *  actions for cumbia-qtcontrols widgets *contextual menus*.
 *
 * \section Requirements
 * The plugin requires the cumbia-qtcontrols module and the qumbia-tango-controls module
 *
 * The *Standard context menu actions for cumbia-qtcontrols widgets plugin* provides a list of
 * QAction actions *ready to be added to and used by a QMenu*. This implies that all logic
 * (from the Qt QActions' signal/slot connections to the actual *slot* methods) is written in
 * the plugin.
 *
 * The plugin is loaded by cumbia-qtcontrols CuContextMenu, which is a QMenu right-click contextual
 * menu offered by the cumbia-qtcontrols widgets.
 *
 * Menus can be extended by custom plugins which name ends with <cite>"context-menu-actions.so"</cite>.
 * The user must place his own plugins under one of the directories listed in the
 * *CUMBIA_PLUGIN_PATH* environment variable.
 *
 * The actions provided by the WidgetStdContextMenuActions plugins are:
 * \li *Link stats*: opens a dialog with statistics on the cumbia-qtcontrols link;
 * \li *helper application*: an application launcher based on the *Tango helperApplication device property*.
 *
 */
class WidgetStdContextMenuActions : public QObject, public CuContextMenuActionsPlugin_I
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "widgets-std-context-menu-actions.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(CuContextMenuActionsPlugin_I)

public:
    WidgetStdContextMenuActions(QObject *parent = 0);

    virtual ~WidgetStdContextMenuActions();

    virtual void setup(QWidget *widget, const CuContext* ctx);

    // CuContextMenuActionsPlugin_I interface
    QList<QAction *> getActions() const;
    int order() const;

public slots:
    void onHelperAActionTriggered(const QString& source);

private slots:
    void onInfoActionTriggered();
    void onHelperAActionTriggered();
    void onDataReady(const CuData& da);

private:
    WidgetStdContextMenuActionsPrivate *d;
};



#endif // WIDGETSTDCONTEXTMENUACTIONS_H
