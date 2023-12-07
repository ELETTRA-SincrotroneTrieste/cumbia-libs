#include "codeinjector.h"
#include <QFile>
#include <QTextStream>
#include <QtDebug>

CodeInjector::CodeInjector(const QString &filename, const QString &mainwidclass, const QString &mainwidvar, const QString &formclassnam) {
    m_filename = filename;
    m_err = false;
    m_mainwclass = mainwidclass;
    m_mainwvar = mainwidvar;
    m_formclassnam = formclassnam;
}


// Regexps
//
// example main widget class: "Danfisik9000"
//
// cpp constructor: find the end of the constructor  body
// (Danfisik9000::Danfisik9000[a-zA-Z0-9\s\(\)\*\:,]*\{(?:\{.*\}|[^\{])*\})
//
// cpp constructor: find the start of the constructor body
// (Danfisik9000::Danfisik9000[a-zA-Z0-9\s\(\)\*\:,]*\{)
//
// main.cpp: find the end of the main function
// (int\s+main[a-zA-Z0-9\s\(\)\*,]*\{(?:\{.*\}|[^\{])*\})
//
// h class definition: end of class definition
// (class\s+Danfisik9000[a-zA-Z0-9\s\(\)\*\:,]*\{(?:\{.*\}|[^\{])*\})
//
// first #include directive: (#include\s*["<])
QString CodeInjector::inject(const QString &input, const QList<Section> &sections)
{
    qDebug() << __FUNCTION__ << "working on file " << m_filename;
    QString output = input;
    int pos;
    QString block, orig;
    foreach(Section se, sections) {
        pos = -1;
        int lineno = -1;
        // replace sections wildcards
        se.text.replace("$MAINCLASS$", m_mainwclass);
        se.text.replace("$UIFORMCLASS$", m_formclassnam);
        if(se.where == Section::EndOfCppConstructor) {
            QRegularExpression endOfCppConstructorRe(QString("(%1::%1[a-zA-Z0-9\\s\\(\\)\\*\\:,]*\\{(?:\\{.*\\}|[^\\{])*\\})")
                                          .arg(m_mainwclass));
            QRegularExpressionMatch ma = endOfCppConstructorRe.match(output);
            qDebug() << __FUNCTION__ << "Section::EndOfCppConstructor";
            pos = ma.capturedStart();
            if(pos > -1) {
                orig = ma.captured(1);
                block = orig;
                block.insert(block.length() - 2, se.text);
                output.replace(endOfCppConstructorRe, block);
                lineno = input.section(orig, 0, 0).count("\n") + 1;
            }
        }
        // (int\s+main[a-zA-Z0-9\s\(\)\*,\[\]]*\{(?:\{.*\}|[^\{])*\})
        else if(se.where == Section::EndOfMain) {
            QRegularExpression endOfMainRe("(int\\s+main[a-zA-Z0-9\\s\\(\\)\\*,\\[\\]]*\\{(?:\\{.*\\}|[^\\{])*\\})");
            QRegularExpressionMatch ma = endOfMainRe.match(output);
            pos = ma.capturedStart();
            if(pos > -1) {
                orig = ma.captured(1);
                block = orig;
                block.insert(block.length() - 2, se.text);
                output.replace(endOfMainRe, block);
                lineno = input.section(orig, 0, 0).count("\n") + 1;
            }
        }
        else if(se.where == Section::StartOfCppConstructor) {
            QRegularExpression startOfCppConstructorRe(QString("(%1::%1[a-zA-Z0-9\\s\\(\\)\\*\\:,]*\\{)")
                                            .arg(m_mainwclass));
            QRegularExpressionMatch ma = startOfCppConstructorRe.match(output);
            pos = ma.capturedStart();
            if(pos > -1) {
                orig = ma.captured(1);
                block = orig + "\n" + se.text + "\n";
                output.replace(startOfCppConstructorRe, block);
                lineno = input.section(orig, 0, 0).count("\n") + 1;
            }
        }
        else if(se.where == Section::EndOfHConstructor) {
            QRegularExpression endOfHClassDefRe(QString("(class\\s+%1[a-zA-Z0-9\\s\\(\\)\\*\\:,]*\\{(?:\\{.*\\}|[^\\{])*\\})")
                                   .arg(m_mainwclass));
            QRegularExpressionMatch ma = endOfHClassDefRe.match(output);
            pos = ma.capturedStart();
            if(pos > -1) {
                orig = ma.captured(1);
                block = orig;
                block.insert(block.length() - 2, se.text);
                output.replace(endOfHClassDefRe, block);
                lineno = input.section(orig, 0, 0).count("\n") + 1;
            }
        }
        else if(se.where == Section::Includes) {
            // \n at the beginning avoids matching commented include directives
            // (\n#include\s*[\"<>A-Za-z0-9_/\.]+\s*)
            qDebug() << __FUNCTION__ << "file is " << m_filename;

            QRegularExpression firstIncludeRe("(\\n#include\\s*[\\\"<>A-Za-z0-9_/\\.]+\\s*)");
            QRegularExpressionMatch ma = firstIncludeRe.match(output);
            pos = ma.capturedStart();
            if(pos > -1) {
                orig = ma.captured(1);
                block = orig;
                block.insert(0, se.text);
                output.replace(ma.captured(1), block);
                lineno = input.section(firstIncludeRe, 0, 0).count("\n") + 1;
            }
        }
        else if(se.where == Section::MainCppBeforeNewWidget) {
            // find where the new widget is declared
            // (Danfisik9000\s*\*[A-Za-z0-9_]+\s*=\s*new\s+Danfisik9000)
            QRegularExpression widDeclRe(QString("(\\s*%1\\s*\\*[A-Za-z0-9_]+\\s*=\\s*new\\s+%1)").arg(m_mainwclass));
            QRegularExpressionMatch ma = widDeclRe.match(output);
            pos = ma.capturedStart();
            if(pos > -1) {
                block = ma.captured(1);
                block.insert(0, "\n" + se.text + "\n");
                output.replace(widDeclRe, block);
                lineno = input.section(ma.captured(1), 0, 0).count("\n") + 1;
            }
        }
        else if(se.where == Section::ElseWhere) {

        }
        if(pos > -1)
            m_log.append(OpQuality(se.whereStr(), orig, block, m_filename, "replacement done", Quality::Ok, lineno));

    }
    return output;
}

bool CodeInjector::write_out(const QString &outf, const QString& s)
{
    QFile f(outf);
    m_err = !f.open(QIODevice::WriteOnly|QIODevice::Text);
    if(!m_err){
        qDebug() << __FUNCTION__ << "writing output on file " + f.fileName() << s;
        QTextStream out(&f);
        out << s;
        f.close();
    }
    if(m_err)
        m_errMsg = "CodeInjector::write_out: error opening " + f.fileName() + " for writing: " + f.errorString();
    return !m_err;
}
