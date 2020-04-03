#include "cuwstangoreplacewildcards.h"
#include <QtDebug>

// (ws[s]{0,1}://){0,1}(tango://){0,1}([A-Za-z_0-9\-\.]*[:]{1}[0-9]+[/]){0,1}[A-Za-z_0-9\-\.]+/[A-Za-z_0-9\-\.]+/[A-Za-z_0-9\-\.]+
#define DEVICE_REGEXP "(ws[s]{0,1}://){0,1}(tango://){0,1}([A-Za-z_0-9\\-\\.]*[:]{1}[0-9]+[/]){0,1}[A-Za-z_0-9\\-\\.]+/[A-Za-z_0-9\\-\\.]+/[A-Za-z_0-9\\-\\.]+"

class CuWsTangoReplaceWildcardsPrivate {
public:
    QStringList args;
};

/*!
 * \brief class constructor
 * \param args the command line arguments
 */
CuWsTangoReplaceWildcards::CuWsTangoReplaceWildcards(const QStringList& args) {
    d = new CuWsTangoReplaceWildcardsPrivate;
    d->args = args;
}

CuWsTangoReplaceWildcards::~CuWsTangoReplaceWildcards() {
    delete d;
}

/** \brief Replace wildcard arguments ($1, $2, ... $N) in tango sources with the command line
 *         arguments.
 *
 * @param the list of the application arguments (qApp->arguments). The first one (the application name) will be ignored.
 * @param s the source which wildcards will be replaced.
 *
 * \par examples
 * \li source: $1/double_scalar  args: ./bin/my_app 2 ws://tango://test/device/1 --> ./bin/my_app and 2 will be ignored. ws://tango://test/device/1 will replace $1
 * \li source: $2/double_scalar  args: ./bin/app 1 --verbose ws://test/dev/1 test/dev/c --> will be test/dev/c/double_scalar
 *
 */
QString CuWsTangoReplaceWildcards::replaceWildcards(const QString &s, const QStringList &_args) const
{
    QString ret(s);
    QStringList devs;
    QStringList args;
    int i0;
    d->args.isEmpty() ? args = _args : args = d->args;
    d->args.isEmpty() ? i0 = 1 : i0 = 0;
    qDebug() << __PRETTY_FUNCTION__ << "using args " << args << "d_args" << d->args << "_args" << args;
    for (int i = i0; i < args.size(); i++)
    {
       if(QRegExp(DEVICE_REGEXP).exactMatch(args[i]))
       {
            devs << (args[i]);
       }
    }
    if (!s.isEmpty() && (s.contains("$")))
        for (int i = devs.size() - 1; i >= 0; i--)
            ret.replace(QString("$%1").arg(i+1), devs[i]);
    return ret;
}
