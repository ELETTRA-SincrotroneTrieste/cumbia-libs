#include "cuformulautils.h"
#include <cumacros.h>
#include <QRegularExpression>

#define DEVICE_REGEXP "(tango://){0,1}([A-Za-z_0-9\\-\\.]*[:]{1}[0-9]+[/]){0,1}[A-Za-z_0-9\\-\\.]+/[A-Za-z_0-9\\-\\.]+/[A-Za-z_0-9\\-\\.]+"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, dev_re, (DEVICE_REGEXP));
#else // deprecated in qt6
Q_GLOBAL_STATIC(QRegularExpression, dev_re, QRegularExpression::anchoredPattern(DEVICE_REGEXP));
#endif


QString CuFormulaUtils::replaceWildcards(const QString &s, const QStringList &args) const
{
    QString ret(s);
    QStringList devs;
    QRegularExpressionMatch ma;
    for (int i = 1; i < args.size(); i++) {
        ma = dev_re->match(args[i]);
        if(ma.hasMatch()) {
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
