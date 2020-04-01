#include "cutangoreplacewildcards.h"
#include "cutcontrols-utils.h"

QString CuTangoReplaceWildcards::replaceWildcards(const QString &s, const QStringList &args) const {
    return CuTControlsUtils().replaceWildcards(s, args);
}
