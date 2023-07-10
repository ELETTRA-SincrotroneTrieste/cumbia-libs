#include "cudatachecker.h"
#include <stdio.h>
#include <QTextStream>
#include <QFile>
#include <QDirIterator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtDebug>
#include <QDirIterator>
/*
 * class CuDType {
public:
    enum Key {
        Time_us,  ///< timestamp microsecs ("timestamp_us")
        Time_ms,   ///< timestamp millis ("timestamp_ms")
        Value, ///< the value, (was "value")
        Src,  ///< source name (was "src")
 */

// \s*([A-Za-z0-9_]+),\s*///<.*"(.*)".*
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, comment_re, ("\s*//\s+"));
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, keys_re, ("\\s*([A-Za-z0-9_]+),\\s*///<.*\"(.*)\".*"));

#else
Q_GLOBAL_STATIC(QRegularExpression, comment_re, "\s*//\s+");
Q_GLOBAL_STATIC(QRegularExpression, keys_re, "\\s*[A-Za-z0-9_]+,\\s*///<.*\"(A-Za-z0-9)\".*");
#endif

CuDataChecker::CuDataChecker() {
    QFile f(QString(CUMBIA_INCLUDES + QString("/cudatatypes.h")));
    if(!f.open(QIODevice::Text|QIODevice::ReadOnly))
        msg = f.errorString();
    else {
        QRegularExpressionMatch ma;
        QTextStream in(&f);
        while(!in.atEnd()) {
            QString l = in.readLine();
            if(!l.contains(*comment_re)) {
                ma = keys_re->match(l);
                if(ma.capturedTexts().size() == 3) {
                    QList<QRegularExpression> rexps;
                    // 1. da["src"] = "a/b/c/d"  // \[\"src\"\]
                    rexps << QRegularExpression(QString("\\[\\\"%1\\\"\\]").arg(ma.capturedTexts().at(2)));
                    // 2. ("src", "a/b/c/d") --> set\(\"src\", .*\).*
                    // like da.set("value", 10);
                    // or CuData da("df", 1);
                    rexps << QRegularExpression(QString("\\(\\\"(%1)\\\", .*\\).*").arg(ma.capturedTexts().at(2)));
                    subs[ma.captured(1)] = rexps;
                }
            }
        }
    }
}

bool CuDataChecker::check() {
    if(msg.length() == 0) { // file open ok
        QDirIterator it(QDir::currentPath(), QStringList() << "*.cpp" << "*.h" ,  QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString fnam = it.next();
            QFile f(fnam);
            if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
                msg = f.errorString();
            }
            else {
                QRegularExpressionMatch ma;
                QTextStream in(&f);
                while(!in.atEnd()) {
                    QString l = f.readLine();
                    l.remove("\n");
                    foreach(const QList<QRegularExpression> &reli, subs.values()) {
                        foreach(const QRegularExpression & re, reli) {
                            ma = re.match(l);
                            if(ma.capturedTexts().size() == 2) {
                                printf("CuDataChecker.check: file %s line %s contains \e[1;32m%s\e[0m\n", fnam.toLatin1().data(), l.toLatin1().data(), ma.capturedTexts()[1].toLatin1().data());
                        }
                    }
                    }
                }
            }
        }
        return msg.isEmpty();
    }
    return false;
}

bool CuDataChecker::update()
{
    printf("CuDataChecker.update \n");
    return true;
}
