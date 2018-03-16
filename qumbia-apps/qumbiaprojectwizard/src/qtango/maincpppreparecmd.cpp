#include "maincpppreparecmd.h"
#include <QRegExp>
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

    QRegExp qappre("\\s*[TQ]Application\\s+([A-Za-z0-9_]*)[\\s*\\([A-Za-z0-9_,\\s]*\\);");
    int pos = qappre.indexIn(input);
    m_err = pos < 0;
    if(pos > -1) {
        appvar = qappre.cap(1);
        QString match = qappre.cap(0);
        qDebug() << __FUNCTION__  << "appvar" <<  qappre.cap(0) << " cap 1 " << qappre.cap(1);
        match.replace(QRegExp("\\s+" + appvar + "\\b"), " qu_app");
        out.replace(qappre.cap(0), match);
        m_log.append(OpQuality("main.cpp pre check", appvar, match, filename(), "replaced", Quality::Ok, -1));
        // remove return a.exec();
        QRegExp appexecRe("(return\\s+" + appvar + ".exec\\(\\))");
        if(appexecRe.indexIn(out) > -1) {
            out.replace(appexecRe.cap(1), "// " + appexecRe.cap(1));
            m_log.append(OpQuality("main.cpp pre check", appexecRe.cap(1), "// " + appexecRe.cap(1), filename(), "removed old call to app.exec", Quality::Ok, -1));
        }
        out.replace(QRegExp("\\b" + appvar + "\\."), "qu_app.");
        m_log.append(OpQuality("main.cpp pre check", appvar + ".", "qu_app.", filename(), "replaced app var name occurrencies", Quality::Ok, -1));
    }
    else
        m_msg = "MainCppPrepareCmd::process: cannot find QApplication declaration in " + input + " regexp " + qappre.pattern();

    QRegExp mainwRe(QString("\\b%1\\s+([A-Za-z0-9_]*)\\s*;").arg(m_mainwinclass));
    pos = mainwRe.indexIn(out);
    if(pos > -1) {
        s = mainwRe.cap(0);
        s.replace(mainwRe.cap(1), "*w = new " + m_mainwinclass + "(0)"); // ";" remains from former line
        out.replace(mainwRe, s);
        // now replace win.function with win->function occurrences
        out.replace(mainwRe.cap(1) + ".",  "w->");
        m_log.append(OpQuality("main.cpp pre check", mainwRe.cap(0), "w->", filename(), "replaced main widget declaration", Quality::Ok, -1));
    }
    // check that the main window is there allocated in the heap
    QRegExp mainwHeapRe(QString("\\b%1\\s*\\*\\s*([A-Za-z0-9_]+)\\s*=\\s*new\\s+%s").arg(m_mainwinclass));
    m_err = pos < 0 && mainwHeapRe.indexIn(out) < 0;
    if(m_err)
        m_msg = "MainCppPrepareCmd::process: missing main widget heap allocation";

    return out;
}

QString MainCppPrepareCmd::name()
{
    return "MainCppPrepareCmd";
}
