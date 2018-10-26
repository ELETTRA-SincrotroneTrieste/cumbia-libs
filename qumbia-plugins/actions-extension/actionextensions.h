#ifndef ACTIONEXTENSION_H
#define ACTIONEXTENSION_H

#include <QGenericPlugin>
#include <quaction-extension-plugininterface.h>

class ActionExtensionPrivate;

class ActionExtensions : public QObject,  public QuActionExtensionPluginInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "actions-extension.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(QuActionExtensionPluginInterface)

public:
    ActionExtensions(QObject *parent = 0);

    virtual ~ActionExtensions();

    // QuActionExtensionPluginInterface interface
public:
    void registerExtension(const QString &name, QuActionExtensionI *ae);
    QStringList extensions() const;
    QuActionExtensionI *getExtension(const QString &name);
    QuActionExtensionFactoryI *getExtensionFactory();

private:
    ActionExtensionPrivate *d;

public:
};

#endif // ACTIONEXTENSION_H
