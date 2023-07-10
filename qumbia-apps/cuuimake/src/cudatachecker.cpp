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



CuDataChecker::CuDataChecker(bool debug)
    : m_keys_re("\\s*([A-Za-z0-9_]+),\\s*///<.*\"(.*)\".*"),
    m_comment_re ("\\s*//\\s+"),
    m_key_patterns (QList<QRegularExpression>()
                   // 1. da["src"] = "a/b/c/d"  // \[\"src\"\]
                   <<  QRegularExpression("\\[\\\"(.*)\\\"\\]")
                   // 2. ("src", "a/b/c/d") --> set\(\"src\", .*\).*
                   // like da.set("value", 10);
                   // or CuData da("df", 1);
                     << QRegularExpression("\\(\\\"(.*)\\\", .*\\).*")),
    m_debug(debug)
{
    QFile f(QString(CUMBIA_INCLUDES + QString("/cudatatypes.h")));
    if(!f.open(QIODevice::Text|QIODevice::ReadOnly))
        msg = f.errorString();
    else {
        QRegularExpressionMatch ma;
        QTextStream in(&f);
        while(!in.atEnd()) {
            QString l = in.readLine();
            if(!l.contains(m_comment_re)) {
                ma = m_keys_re.match(l);
                const QStringList& ct = ma.capturedTexts();
                if(ct.size() == 3) {
                    subs[ct.at(2)] = ct.at(1);
                }
            }
        }
    }
}

int CuDataChecker::check() {
    return m_process(false);
}

int CuDataChecker::update() {
    return  m_process(true);
}

// returns
// >= 0 number of string keys found / replaced
// < 0 *.h, *.cpp file open error
int CuDataChecker::m_process(bool rw) {
    int found = 0;
    if(msg.length() == 0) { // file open ok
        QString newf;
        const QString& cwd = QDir::currentPath();
        QDirIterator it(cwd, QStringList() << "*.cpp" << "*.h" ,  QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            int lcnt = 0;
            QString fnam = it.next();
            QString relfnam(fnam);
            relfnam.remove(cwd + "/"); // remove current w dir from abs path
            QFile f(fnam);
            if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
                msg = f.errorString();
            }
            else {
                QRegularExpressionMatch ma;
                QTextStream in(&f);
                while(!in.atEnd()) {
                    QString l = f.readLine();
                    QString lnonl(l);
                    QString updated_line(l);
                    lcnt++;
                    QStringList matches_in_line;
                    lnonl.remove("\n");
                    foreach(const QRegularExpression &re, m_key_patterns) {
                        QRegularExpressionMatchIterator i = re.globalMatch(l);
                        while (i.hasNext()) {
                            ma = i.next();
                            if (ma.hasMatch()) {
                                const QStringList &caps = ma.capturedTexts();
                                if(caps.size() == 2 && subs.contains(caps[1])) {
                                    matches_in_line << caps[1];
                                    found++;
//                                    if(m_debug)
//                                        printf("CuDataChecker.check: file %s line %s contains \e[1;32m%s --> \e[1;36mCuDType::%s\e[0m\n",
//                                               relfnam.toLatin1().data(), lcp.remove("\n").toLatin1().data(),
//                                               caps[1].toLatin1().data(), subs[caps[1]].toStdString().c_str());
                                    if(rw)
                                        updated_line.replace(caps[1], subs[caps[1]]);
//                                    if(re.pattern().contains("set"))
//                                        printf("[WARNING]: manually check line %d \"%s\", file \"%s\" using the \"set\" method on CuData\n",
//                                               lcnt, lcp.remove("\n").toLatin1().data(), relfnam.toLatin1().data());
                                }
                                else if(caps.size() == 2 && m_debug) {
                                    printf("CuDataChecker.check: file %s line %s contains string key \"\e[1;33m%s\e[0m\" which is not mapped [\e[1;32mOK\e[0m]\n",
                                           relfnam.toLatin1().data(), lnonl.remove("\n").toLatin1().data(),
                                           caps[1].toLatin1().data());
                                }
                            }
                        }
                    } // after all reg exps applied to a line
                    if(m_debug)
                    {
                        if(matches_in_line.size() > 0) {
                            printf("file %s line %d: \e[0;34m%s\e[0m: (", relfnam.toStdString().c_str(), lcnt, lnonl.toStdString().c_str());
                            foreach(const QString& s, matches_in_line) {
                                printf("[\e[0;32m%s\e[0m -> \e[1;32m%s\e[0m] ", s.toStdString().c_str(), subs[s].toStdString().c_str());
                            }
                            printf(")\n");
                        }
                    }
                    if(updated_line != l && m_debug)
                        printf("<<\e[0;35m%s\e[0m>>\n--> <<\e[1;32m%s\e[0m<<\n", l.toStdString().c_str(), updated_line.toStdString().c_str());
                    if(rw)
                        newf += updated_line;
                }
            }
            f.close();
            if(rw && !f.open(QIODevice::WriteOnly|QIODevice::Text))
                msg = "error opening file in write mode: "  + f.errorString();
            else if(rw) {
                QTextStream out(&f);
                out << newf;
                f.close();
            }
        }
    }

    return msg.isEmpty() ? found : -1;
}
