#include "aliasproc.h"
#include <QRegularExpression>
#include <cumacros.h>

AliasProc::AliasProc()
{

}

QString AliasProc::findAndReplace(const QString &in, const QList<AliasEntry> &aliases)
{
    QString out(in);
    foreach(const AliasEntry& e, aliases) {
        // \bdouble\b
        QRegularExpression re(QString("\\b%1\\b").arg(e.name));
        QRegularExpressionMatch match = re.match(out);
        if(match.hasMatch()) {
            QString capture = match.captured(0);
            capture.replace(e.name, e.replaces);
            out.replace(match.captured(0), capture);
        }
    }
    return out;
}
