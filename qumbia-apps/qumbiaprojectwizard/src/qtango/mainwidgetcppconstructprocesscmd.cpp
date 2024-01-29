#include "mainwidgetcppconstructprocesscmd.h"
#include <QRegularExpression>
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
    QRegularExpression constrRe(QString("%1::%1\\((QWidget\\s*\\*\\s*[a-zA-Z0-9_]+)\\)").arg(m_mainclass));
    QRegularExpressionMatch ma = constrRe.match(out);
    if(ma.hasMatch()) {
        const QString& constr = ma.captured(0);
        constructorArgs = ma.captured(1);
        if(!constructorArgs.isEmpty()) {
            QRegularExpression cupoolRe ("cu_pool\\s*=\\s*([A-Za-z0-9_]+);");
            ma = cupoolRe.match(out);
            if(ma.hasMatch()) {
                QString cu_pool_varnam = ma.captured(1);
                if(!cu_pool_varnam.isEmpty()) {
                    QString newconstr = QString("%1::%1(CumbiaPool *%2, %3)").
                            arg(m_mainclass).arg(cu_pool_varnam).arg(constructorArgs);
                    out.replace(constrRe, newconstr);
                    m_log.append(OpQuality("expand main widget constructor", constr, newconstr, filename(),
                                           "expanded main widget constructor", Quality::Ok, -1));
                }
            }
        }
    }

    m_err = !ma.hasMatch();
    if(!m_err) {
        // 2 if ui.setupUi(this) or ui->setupUi(this), remove, because
        //  ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
        // should be injected automatically
        // (\s*[a-zA-Z0-9_]+(?:\->|\.)setupUi\(this\);.*\n*)
        //
        QRegularExpression setupUiRe("([a-zA-Z0-9_]+(?:\\->|\\.)setupUi\\(this\\);\\n*)");
        ma = setupUiRe.match(out);
        if(ma.hasMatch()) {
            out.replace(ma.captured(1), "//" + ma.captured(1));
        }

        // 3 replace all occurrences of ui.something with ui->something
        // (ui)\.([A-Za-z0-9_]+)
        // two capturing parenthesis: cap(1) contains ui var name
        // cap(2) the method(arg1, arg2, ...);
        //
        QRegularExpression uire("(ui)\\.([A-Za-z0-9_]+)");
        int pos = 0;
        int lineno;
        QString ns;
        while(pos >= 0) {
            ma = uire.match(out, pos);
            pos = ma.capturedStart();
            qDebug() << __FUNCTION__ << "match pos " << pos << ma.captured(0) << ma.captured(1) << ma.captured(2);
            if(pos >= 0) {
                if(ma.captured(2) != "setupUi") {
                    lineno = input.section(uire, 0, 0).count("\n") + 1;
                    ns = ma.captured(1) + "->" + ma.captured(2); // new string
                    out.replace(ma.captured(0), ns);
                    m_log.append(OpQuality("main widget ui var check", ma.captured(0), ns, filename(), "ui.someMethod() replaced with ui->someMethod()",
                                           Quality::Ok, lineno));
                }
                // Returns the offset inside the subject string immediately after
                // the ending position of the substring captured by the nth
                // capturing group. If the nth capturing group did not capture
                // a string or doesn't exist, returns -1.
                pos = ma.capturedEnd(2);
            }
        }

        // 3. remove ui instantiation if in this form:
        //
        // ComplexQTangoDemo::ComplexQTangoDemo(CumbiaPool *cut, QWidget *parent) :
        // QWidget(parent),
        // ui(new Ui::ComplexQTangoDemo) <----
        // unescaped pattern: (,\n*\s*ui\(new Ui::ClassName\))

        // capture Form class name from the ui = new Ui::ClassName; line
        // unescaped: ui\s*=\s*new\s+Ui::([A-Za-z0-9_]+)
        QString uiform_class;
        QRegularExpression formClassRe("ui\\s*=\\s*new\\s+Ui::([A-Za-z0-9_]+)");
        QRegularExpressionMatch ma = formClassRe.match(out);
        if(ma.hasMatch()) {
            uiform_class = ma.captured(1);
            QRegularExpression badNewUi(QString("(,\\n*\\s*ui\\(new Ui::%1\\))").arg(uiform_class));
            ma = badNewUi.match(out);
            if(ma.hasMatch()) {
                QString capture = ma.captured(1);
                QString commented =  "\t/* " + capture.remove("\n") + "\t  // ## qumbiaprojectwizard: instantiated in constructor body */";
                out.replace(ma.captured(1), commented);
                lineno = input.section(badNewUi, 0, 0).count("\n") + 1;
                m_log.append(OpQuality("main widget ui var check", capture.remove("\n"), commented.remove("\n"), filename(),
                                       "commented ui form instantiation in constructor parameter initialization",
                                       Quality::Ok, lineno));
            }
        }
    }


    if(m_err) {
        m_msg = "MainWidgetCppConstructProcessCmd::process: could not match constructor arguments";
        qDebug() << __PRETTY_FUNCTION__ << "could not match pattern " << constrRe.pattern() << "in";
    }
    return out;
}

QString MainWidgetCppConstructProcessCmd::name()
{
    return "MainWidgetCppConstructProcessCmd";
}
