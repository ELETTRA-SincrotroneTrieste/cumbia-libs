#include "cuepcontrols-utils.h"
#include <cumacros.h>

// (tango://){0,1}([A-Za-z_0-9\.]*[:]{1}[0-9]+[/]){0,1}[A-Za-z_0-9\.]+/[A-Za-z_0-9\.]+/[A-Za-z_0-9\\.]+
#define DEVICE_REGEXP "(epics://){0,1}([A-Za-z_0-9\\.]*[:]{1}[0-9]+[/]){0,1}[A-Za-z_0-9\\.]+/[A-Za-z_0-9\\.]+/[A-Za-z_0-9\\\\.]+"

CuEpControlsUtils::CuEpControlsUtils()
{

}

QString CuEpControlsUtils::replaceWildcards(const QString &s, const QStringList &args)
{
    QString ret(s);
    QStringList devs;
    for (int i = 0; i < args.size(); i++)
    {
        cuprintf("See ifArgument %s matches regexp\n", qstoc(args[i]));
       if(QRegExp(DEVICE_REGEXP).exactMatch(args[i]))
       {
            devs << (args[i]);
       }
    }
    if (!s.isEmpty() && (s.contains("$")))
        for (int i = devs.size() - 1; i >= 0; i--)
            ret.replace(QString("$%1").arg(i+1), devs[i]);
    cuprintf("CuTControlsUtils::replaceWildcards: %s --> %s\n", qstoc(s), qstoc(ret));
    return ret;
}
