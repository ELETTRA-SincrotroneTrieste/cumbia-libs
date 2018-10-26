#include "actionextensionfactory.h"
#include <gettdbpropertyextension.h>
#include <cucontexti.h>
#include <QString>

ActionExtensionFactory::ActionExtensionFactory()
{

}

QuActionExtensionI *ActionExtensionFactory::create(const QString &name, CuContextI *ctx)
{
    if(name == "GetTDbPropertyExtension") {
        return new GetTDbPropertyExtension(ctx->getContext());
    }
    return NULL;
}
