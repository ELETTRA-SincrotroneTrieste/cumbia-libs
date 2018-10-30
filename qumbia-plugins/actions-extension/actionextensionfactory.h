#ifndef ACTIONEXTENSIONFACTORY_H
#define ACTIONEXTENSIONFACTORY_H

#include <quaction-extension-plugininterface.h>

class CuContext;
class QString;

class ActionExtensionFactory : public QuActionExtensionFactoryI
{
public:
    ActionExtensionFactory();

    // QuActionExtensionFactoryI interface
public:
    QuActionExtensionI *create(const QString &name, const CuContext *ctx);
};

#endif // ACTIONEXTENSIONFACTORY_H
