#include "cuhttptangoreplacewildcards.h"
#include <QRegularExpression>
#include <QtDebug>

// (http[s]{0,1}://){0,1}(tango://){0,1}([A-Za-z_0-9\-\.]*[:]{1}[0-9]+[/]){0,1}[A-Za-z_0-9\-\.]+/[A-Za-z_0-9\-\.]+/[A-Za-z_0-9\-\.]+
#define DEVICE_REGEXP "(http[s]{0,1}://){0,1}(tango://){0,1}([A-Za-z_0-9\\-\\.]*[:]{1}[0-9]+[/]){0,1}[A-Za-z_0-9\\-\\.]+/[A-Za-z_0-9\\-\\.]+/[A-Za-z_0-9\\-\\.]+"

// anchoredPattern: exact match
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, device_re, (QRegularExpression::anchoredPattern(DEVICE_REGEXP)))
#else
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, device_re, (QRegularExpression(DEVICE_REGEXP)))
#endif
class CuHttpTangoReplaceWildcardsPrivate {
public:
    QStringList args;
};

/*!
 * \brief class constructor
 * \param args the command line arguments
 */
CuHttpTangoReplaceWildcards::CuHttpTangoReplaceWildcards(const QStringList& args) {
    d = new CuHttpTangoReplaceWildcardsPrivate;
    d->args = args;
}

CuHttpTangoReplaceWildcards::~CuHttpTangoReplaceWildcards() {
    delete d;
}

/** \brief Replace wildcard arguments ($1, $2, ... $N) in tango sources with the command line
 *         arguments.
 *
 * @param the list of the application arguments (qApp->arguments). The first one (the application name) will be ignored.
 * @param s the source which wildcards will be replaced.
 *
 * \par examples
 * \li source: $1/double_scalar  args: ./bin/my_app 2 http://tango://test/device/1 --> ./bin/my_app and 2 will be ignored. http://tango://test/device/1 will replace $1
 * \li source: $2/double_scalar  args: ./bin/app 1 --verbose http://test/dev/1 test/dev/c --> will be test/dev/c/double_scalar
 *
 */
QString CuHttpTangoReplaceWildcards::replaceWildcards(const QString &s, const QStringList &_args) const
{
    QString ret(s);
    QStringList devs;
    QStringList args;
    QRegularExpressionMatch ma;
    int i0;
    d->args.isEmpty() ? args = _args : args = d->args;
    d->args.isEmpty() ? i0 = 1 : i0 = 0;
    for (int i = i0; i < args.size(); i++) {
        ma = device_re->match(args[i]);
        if(ma.hasMatch())
            devs << (args[i]);
    }
    if (!s.isEmpty() && (s.contains("$")))
        for (int i = devs.size() - 1; i >= 0; i--)
            ret.replace(QString("$%1").arg(i+1), devs[i]);
    return ret;
}
