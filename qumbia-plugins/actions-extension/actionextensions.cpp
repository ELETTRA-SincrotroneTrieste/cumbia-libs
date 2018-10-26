#include "actionextensions.h"
#include <quaction-extension-plugininterface.h>
#include "actionextensionfactory.h"
#include <QMap>
#include <QString>

class ActionExtensionPrivate {
  public:
    QMap<QString, QuActionExtensionI *> extensions;
    QuActionExtensionFactoryI *ae_factory;
};

ActionExtensions::ActionExtensions(QObject *parent) :
    QuActionExtensionPluginInterface()
{
    d = new ActionExtensionPrivate;
    d->ae_factory = NULL;
}

ActionExtensions::~ActionExtensions()
{
    if(d->ae_factory)
        delete d->ae_factory;
    delete d;
}

void ActionExtensions::registerExtension(const QString &name, QuActionExtensionI *ae)
{
    d->extensions[name] = ae;
}

QStringList ActionExtensions::extensions() const
{
    return d->extensions.keys();
}

QuActionExtensionI *ActionExtensions::getExtension(const QString &name)
{
    return d->extensions[name];
}

QuActionExtensionFactoryI *ActionExtensions::getExtensionFactory()
{
    if(!d->ae_factory)
        d->ae_factory = new ActionExtensionFactory();
    return d->ae_factory;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(actions-extension, ActionExtension)
#endif // QT_VERSION < 0x050000
