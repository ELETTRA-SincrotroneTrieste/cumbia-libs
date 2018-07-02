#ifndef CUMBIAQMLCONTROLSPLUGIN_H
#define CUMBIAQMLCONTROLSPLUGIN_H

#include <QQmlExtensionPlugin>

class CumbiaQmlControlsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri);
};

#endif // CUMBIAQMLCONTROLSPLUGIN_H
