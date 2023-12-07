#include "maincpppreparecmd.h"
#include <QRegularExpression>
#include <QtDebug>

MainCppPrepareCmd::MainCppPrepareCmd(const QString& filenam, const QString &mainwclass) : FileCmd(filenam)
{
    m_mainwinclass = mainwclass;
}

// 1.
// find the QApplication variable name
// [spaces]QApplication[space]variable_name (argc, argv);
// NO QApplication a = new QApplication...
// \s*[TQ]Application\s+([A-Za-z0-9_]*)[\s*\([A-Za-z0-9_,\s]*\);
//
// 2.
// find the main widget declaration and, if not in the heap,
// replace the declaration appropriately
// \bDanfisik9000\s+([A-Za-z0-9_]*)\s*;
//
QString MainCppPrepareCmd::process(const QString &input)
{
    QString out = input;
    QString appvar, s;
    QRegularExpression qappre("\\s*[TQ]Application\\s+([A-Za-z0-9_]*)[\\s*\\([A-Za-z0-9_,\\s]*\\);");
    QRegularExpressionMatch ma = qappre.match(input);
    int pos = ma.capturedStart();
    m_err = pos < 0;
    if(pos > -1) {
        appvar = ma.captured(1);
        QString match = ma.captured(0);
        qDebug() << __FUNCTION__  << "appvar" <<  ma.captured(0) << " cap 1 " << ma.captured(1);
        match.replace(QRegularExpression("\\s+" + appvar + "\\b"), " qu_app");
        out.replace(ma.captured(0), match);
        m_log.append(OpQuality("main.cpp pre check", appvar, match, filename(), "replaced", Quality::Ok, -1));
        // remove return a.exec();
        QRegularExpression appexecRe("(return\\s+" + appvar + ".exec\\(\\))");
        ma = appexecRe.match(out);
        if(ma.hasMatch()) {
            out.replace(ma.captured(1), "// " + ma.captured(1));
            m_log.append(OpQuality("main.cpp pre check", ma.captured(1), "// " + ma.captured(1), filename(), "removed old call to app.exec", Quality::Ok, -1));
        }
        out.replace(QRegularExpression("\\b" + appvar + "\\."), "qu_app.");
        m_log.append(OpQuality("main.cpp pre check", appvar + ".", "qu_app.", filename(), "replaced app var name occurrencies", Quality::Ok, -1));
    }
    else
        m_msg = "MainCppPrepareCmd::process: cannot find QApplication declaration in " + input + " regexp " + qappre.pattern();

    QRegularExpression mainwRe(QString("\\b%1\\s+([A-Za-z0-9_]*)\\s*;").arg(m_mainwinclass));
    ma = mainwRe.match(out);
    pos = ma.capturedStart();
    if(pos > -1) {
        s = ma.captured(0);
        s.replace(ma.captured(1), "*w = new " + m_mainwinclass + "(0)"); // ";" remains from former line
        out.replace(mainwRe, s);
        // now replace win.function with win->function occurrences
        out.replace(ma.captured(1) + ".",  "w->");
        m_log.append(OpQuality("main.cpp pre check", ma.captured(0), "w->", filename(), "replaced main widget declaration", Quality::Ok, -1));
    }
    // check that the main window is there allocated in the heap
    QRegularExpression mainwHeapRe(QString("\\b%1\\s*\\*\\s*([A-Za-z0-9_]+)\\s*=\\s*new\\s+%s").arg(m_mainwinclass));
    ma = mainwHeapRe.match(out);
    m_err = pos < 0 && !ma.hasMatch();
    if(m_err)
        m_msg = "MainCppPrepareCmd::process: missing main widget heap allocation";
    return out;
}

QString MainCppPrepareCmd::name()
{
    return "MainCppPrepareCmd";
}
