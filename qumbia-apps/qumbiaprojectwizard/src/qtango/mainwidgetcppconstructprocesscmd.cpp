#include "mainwidgetcppconstructprocesscmd.h"
#include <QtDebug>

MainWidgetCppConstructProcessCmd::MainWidgetCppConstructProcessCmd(const QString& fnam, const QString &mainwclass)
    : FileCmd(fnam)
{
    m_mainclass = mainwclass;
}


QString MainWidgetCppConstructProcessCmd::process(const QString &input)
{
    m_err = false;

    QString out = input;
    // 1 expand constructor
    QString constructorArgs;
    // capture the constructor arguments Danfisik9000::Danfisik9000\((QWidget\s*\*\s*[a-zA-Z0-9_]+)\)
    QRegExp constrRe(QString("%1::%1\\((QWidget\\s*\\*\\s*[a-zA-Z0-9_]+)\\)").arg(m_mainclass));
    int pos = constrRe.indexIn(out);
    if(pos > -1 ) {
        constructorArgs = constrRe.cap(1);
        if(!constructorArgs.isEmpty()) {
            QRegExp cutRe ("cu_t\\s*=\\s*([A-Za-z0-9_]+);");
            pos = cutRe.indexIn(out);
            if(pos > -1) {
                QString cumbia_tango_varnam = cutRe.cap(1);
                if(!cumbia_tango_varnam.isEmpty()) {
                    QString newconstr = QString("%1::%1(CumbiaTango *%2, %3)").
                            arg(m_mainclass).arg(cumbia_tango_varnam).arg(constructorArgs);
                    out.replace(constrRe, newconstr);
                    m_log.append(OpQuality("expand main widget constructor", constrRe.cap(0), newconstr, filename(),
                                           "expanded main widget constructor", Quality::Ok, -1));
                }
            }
        }
    }

    m_err = pos < 0;
    if(!m_err) {
        // 2 if ui.setupUi(this) or ui->setupUi(this), remove, because
        //  ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
        // should be injected automatically
        // (\s*[a-zA-Z0-9_]+(?:\->|\.)setupUi\(this\);.*\n*)
        //
        QRegExp setupUiRe("([a-zA-Z0-9_]+(?:\\->|\\.)setupUi\\(this\\);\\n*)");
        pos = setupUiRe.indexIn(out);
        if(pos > -1) {
            out.replace(setupUiRe.cap(1), "//" + setupUiRe.cap(1));
        }

        // 3 replace all occurrences of ui.something with ui->something
        // (ui)\.([A-Za-z0-9_]+)
        // two capturing parenthesis: cap(1) contains ui var name
        // cap(2) the method(arg1, arg2, ...);
        //
        QRegExp uire("(ui)\\.([A-Za-z0-9_]+)");
        pos = 0;
        int lineno;
        QString ns;
        while(pos >= 0) {
            pos = uire.indexIn(out, pos);
            qDebug() << __FUNCTION__ << "match pos " << pos << uire.cap(0) << uire.cap(1) << uire.cap(2);
            if(pos >= 0) {
                if(uire.cap(2) != "setupUi") {
                    lineno = input.section(uire, 0, 0).count("\n") + 1;
                    ns = uire.cap(1) + "->" + uire.cap(2); // new string
                    out.replace(uire.cap(0), ns);
                    m_log.append(OpQuality("main widget ui var check", uire.cap(0), ns, filename(), "ui.someMethod() replaced with ui->someMethod()",
                                           Quality::Ok, lineno));
                }
                pos += uire.matchedLength();
            }
        }

        // 3. remove ui instantiation if in this form:
        //
        // ComplexQTangoDemo::ComplexQTangoDemo(CumbiaTango *cut, QWidget *parent) :
        // QWidget(parent),
        // ui(new Ui::ComplexQTangoDemo) <----
        // unescaped pattern: (,\n*\s*ui\(new Ui::ClassName\))

        // capture Form class name from the ui = new Ui::ClassName; line
        // unescaped: ui\s*=\s*new\s+Ui::([A-Za-z0-9_]+)
        QString uiform_class;
        QRegExp formClassRe("ui\\s*=\\s*new\\s+Ui::([A-Za-z0-9_]+)");
        pos = formClassRe.indexIn(out);
        if(pos >= -1) {
            uiform_class = formClassRe.cap(1);
            QRegExp badNewUi(QString("(,\\n*\\s*ui\\(new Ui::%1\\))").arg(uiform_class));
            pos = badNewUi.indexIn(out);
            if(pos > -1) {
                QString capture = badNewUi.cap(1);
                QString commented =  "\t/* " + capture.remove("\n") + "\t  // ## qumbiaprojectwizard: instantiated in constructor body */";
                out.replace(badNewUi.cap(1), commented);
                lineno = input.section(badNewUi, 0, 0).count("\n") + 1;
                m_log.append(OpQuality("main widget ui var check", capture.remove("\n"), commented.remove("\n"), filename(),
                                       "commented ui form instantiation in constructor parameter initialization",
                                       Quality::Ok, lineno));
            }
        }
    }


    if(m_err)
        m_msg = "MainWidgetCppConstructProcessCmd::process: could not match constructor arguments";
    return out;
}

QString MainWidgetCppConstructProcessCmd::name()
{
    return "MainWidgetCppConstructProcessCmd";
}
