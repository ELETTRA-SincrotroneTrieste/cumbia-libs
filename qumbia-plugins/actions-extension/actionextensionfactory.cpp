#include "actionextensionfactory.h"
#include "gettdbpropertyextension.h"
#include "infodialogextension.h"
#include "cuapplicationlauncherextension.h"
#include <cucontexti.h>
#include <QString>

ActionExtensionFactory::ActionExtensionFactory()
{

}

QuActionExtensionI *ActionExtensionFactory::create(const QString &name, const CuContext *ctx)
{
    if(name == "GetTDbPropertyExtension")
        return new GetTDbPropertyExtension(ctx);
    else if(name == "InfoDialogExtension")
        return new InfoDialogExtension(ctx);
    else if(name == "CuApplicationLauncherExtension")
        return new CuApplicationLauncherExtension(ctx);
    return NULL;
}
