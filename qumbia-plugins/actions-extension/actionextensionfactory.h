#ifndef ACTIONEXTENSIONFACTORY_H
#define ACTIONEXTENSIONFACTORY_H

#include <quaction-extension-plugininterface.h>

class CuContextI;
class QString;

class ActionExtensionFactory : public QuActionExtensionFactoryI
{
public:
    ActionExtensionFactory();

    // QuActionExtensionFactoryI interface
public:
    QuActionExtensionI *create(const QString &name, const CuContextI *ctxi);
};

#endif // ACTIONEXTENSIONFACTORY_H
