#include "cuformulautils.h"
#include <cumacros.h>

#define DEVICE_REGEXP "(tango://){0,1}([A-Za-z_0-9\\-\\.]*[:]{1}[0-9]+[/]){0,1}[A-Za-z_0-9\\-\\.]+/[A-Za-z_0-9\\-\\.]+/[A-Za-z_0-9\\-\\.]+"

QString CuFormulaUtils::replaceWildcards(const QString &s, const QStringList &args) const
{
    QString ret(s);
    QStringList devs;
    for (int i = 1; i < args.size(); i++) {
        if(QRegExp(DEVICE_REGEXP).exactMatch(args[i])) {
            devs << (args[i]);
        }
    }
    if (!s.isEmpty() && (s.contains("$"))) {
        for (int i = devs.size() - 1; i >= 0; i--) {
            ret.replace(QString("$%1").arg(i+1), devs[i]);
        }
    }
    return ret;
}
