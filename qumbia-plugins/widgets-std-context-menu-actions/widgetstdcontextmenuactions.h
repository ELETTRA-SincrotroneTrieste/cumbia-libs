#ifndef WIDGETSTDCONTEXTMENUACTIONS_H
#define WIDGETSTDCONTEXTMENUACTIONS_H

#include <cucontextmenuactionsplugin_i.h>
#include <QObject>

class QuActionExtensionPluginInterface;
class WidgetStdContextMenuActionsPrivate;
class CuData;
class CuContext;

class WidgetStdContextMenuActions : public QObject, public CuContextMenuActionsPlugin_I
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "widgets-std-context-menu-actions.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(CuContextMenuActionsPlugin_I)

public:
    WidgetStdContextMenuActions(QObject *parent = 0);

    virtual void setup(QWidget *widget, const CuContext* ctx);

    // CuContextMenuActionsPlugin_I interface
    QList<QAction *> getActions() const;

private slots:
    void onInfoActionTriggered();
    void onHelperAActionTriggered();
    void onDataReady(const CuData& da);

private:
    WidgetStdContextMenuActionsPrivate *d;
};

#endif // WIDGETSTDCONTEXTMENUACTIONS_H
