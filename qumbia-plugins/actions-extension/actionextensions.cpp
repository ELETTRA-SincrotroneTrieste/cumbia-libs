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
    QObject(parent),
    QuActionExtensionPluginInterface()
{
    d = new ActionExtensionPrivate;
    d->ae_factory = NULL;
}

ActionExtensions::~ActionExtensions()
{
    if(d->ae_factory)
        delete d->ae_factory;
    foreach(QuActionExtensionI *ex, d->extensions.values()) {
        printf("~ActionExtensions: deleting %s\n", qstoc(ex->getName()));
        delete ex;
    }
    delete d;
}

/*! \brief register a new extension into the ActionExtensions manger. Ownership is handed over to
 *         ActionExtensions.
 *
 * @param name the name associated to the QuActionExtensionI to register
 * @param ae an instance of QuActionExtensionI
 *
 * \note if another extension is registered with the same name, the older extension is deleted
 * \note All registered extensions are deleted upon ActionExtensions destruction
 *
 */
void ActionExtensions::registerExtension(const QString &name, QuActionExtensionI *ae)
{
    if(d->extensions.contains(name))
        delete d->extensions[name];
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

void ActionExtensions::clear()
{
    d->extensions.clear();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(actions-extension, ActionExtensions)
#endif // QT_VERSION < 0x050000

