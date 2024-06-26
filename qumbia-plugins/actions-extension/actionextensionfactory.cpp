#include "actionextensionfactory.h"
#include "gettdbpropertyextension.h"
#include "infodialogextension.h"
#include "cuapplicationlauncherextension.h"
#include "engineswitchdialogextension.h"
#include <cucontexti.h>
#include <QString>

ActionExtensionFactory::ActionExtensionFactory()
{

}

/*! \brief Creates a specific implementation of QuActionExtensionI
 *
 * @param name one of the following names:
 *
 * \li GetTDbPropertyExtension to return a new GetTDbPropertyExtension (needs CuContextI)
 * \li InfoDialogExtension to return a new InfoDialogExtension (needs CuContextI)
 * \li CuApplicationLauncherExtension to return a new CuApplicationLauncherExtension (ctx is null)
 *
 * @return a new instance of the desired object or NULL if the specified name is not valid or the
 * context is NULL where it should be valid (GetTDbPropertyExtension, InfoDialogExtension)
 *
 */
QuActionExtensionI *ActionExtensionFactory::create(const QString &name, const CuContextI *ctxi)
{
    if(name == "GetTDbPropertyExtension")
        return new GetTDbPropertyExtension(ctxi);
    else if(name == "InfoDialogExtension")
        return new InfoDialogExtension(ctxi);
    else if(name == "CuApplicationLauncherExtension")
        return new CuApplicationLauncherExtension(ctxi);
    else if(name == "EngineSwitchDialogExtension")
        return new EngineSwitchDialogExtension(ctxi);
    return nullptr;
}
