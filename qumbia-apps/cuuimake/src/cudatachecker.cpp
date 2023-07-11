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
                   <<  QRegularExpression("[a-zA-Z0-9_\\-\\.]+\\s*\\[\\\"([a-zA-Z0-9_\\-\\.]+)\\\"\\]")
                   // 2. ("src", "a/b/c/d") --> set\(\"src\", .*\).*
                   // like da.set("value", 10);
                     << QRegularExpression("CuData\\s+[A-Za-z0-9_]+\\(\\\"([A-Za-z0-9_]+)\\\", .*\\)")
                   // or CuData da("df", 1);
                   << QRegularExpression("(?:set|has|add)\\(\\\"([a-zA-Z0-9_\\-\\.]+)\\\",.*\\)")
                   // std::string s = da.s("value") --> match da.s("value") --> [A-Za-z0-9_]+\.[A-Za-z_0-9]+\(\"(.*)\"\)
                   << QRegularExpression("[A-Za-z0-9_]+\\.[A-Za-z_0-9]+\\(\\\"([a-zA-Z0-9_\\-\\.]+)\\\"\\)")
                    // CuData da1("value", 1), da2("dt", 1), da3("df", 2);  <-- match the da2 and da3
                    // ,\s*[A-Za-z0-9_]+\(\"([A-Za-z0-9\-\._]+)\",.*\)
                     << QRegularExpression(",\\s*[A-Za-z0-9_]+\\(\\\"([A-Za-z0-9\\-\\._]+)\\\",.*\\)")),

    m_debug(debug)
{
    m_key_patterns[0].setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    m_key_patterns[1].setPatternOptions(QRegularExpression::InvertedGreedinessOption);
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
    result = m_process(false);
    return result;
}

int CuDataChecker::update() {
    result =   m_process(true);
    return result;
}

// returns
// >= 0 number of string keys found / replaced
// < 0 *.h, *.cpp file open error
int CuDataChecker::m_process(bool rw) {
    int found = 0;
    if(msg.length() == 0) { // file open ok
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
                QString newf;
                QRegularExpressionMatch ma;
                QTextStream in(&f);
                while(!in.atEnd()) {
                    QString l = f.readLine();
                    QString lnonl(l);
                    bool hasnl = l.endsWith("\n");
                    if(hasnl)
                        lnonl.remove("\n");
                    QString updated_line(lnonl);
                    lcnt++;
                    QStringList matches_in_line, full_matches_in_line;
                    ma = m_comment_re.match(l);
                    if(!ma.hasMatch()) {
                        foreach(const QRegularExpression &re, m_key_patterns) {
                            QRegularExpressionMatchIterator i = re.globalMatch(l);
                            while (i.hasNext()) {
                                ma = i.next();
                                if (ma.hasMatch()) {
                                    const QStringList &caps = ma.capturedTexts();

                                    if(caps.size() == 2 && subs.contains(caps[1])) {
                                        matches_in_line << caps[1];
                                        full_matches_in_line << caps[0];
                                        str_keys_invalid << caps[1];
                                        found++;
                                        if(rw)
                                            updated_line.replace(QString("\"%1\"").arg(caps[1]), QString("CuDType::%1").arg(subs[caps[1]]));
                                    }
                                    else if(caps.size() == 2) {
                                        str_keys_unmapped << caps[1];
                                    }
                                }
                            }
                        } // after all reg exps applied to a line
                    }
                    if(rw) {
                        if(updated_line != lnonl) {
                            updated_line += "  // " + full_matches_in_line.join(", ");
                        }
                        newf += updated_line + (hasnl ?  "\n" : "");

                    }
                    // read next line
                } // in.atEnd()
                f.close();
                if(!newf.isEmpty() && rw && !f.open(QIODevice::WriteOnly|QIODevice::Text))
                    msg = "error opening file in write mode: "  + f.errorString();
                else if(!newf.isEmpty() && rw) {
                    QTextStream out(&f);
                    out << newf;
                    f.close();
                }
            }
        } // iterator next file
    }

    return msg.isEmpty() ? found : -1;
}
