#include "mainwidgethprocesscmd.h"
#include <QtDebug>

MainWidgetHProcessCmd::MainWidgetHProcessCmd(const QString& fnam, const QString& mainwnam,
                                             const QString& formClassName)
    : FileCmd(fnam)
{
    m_mainwnam = mainwnam;
    m_formclassnam = formClassName;
}


QString MainWidgetHProcessCmd::process(const QString &input)
{
    m_err = false;
    // 1 find if there's a definition like
    //     Ui::Danfisik9000 ui;
    // and replace with
    //     Ui::Danfisik9000 *ui;
    //
    QString out = input;
    QRegExp uidefRe(QString("\\n(\\s*Ui::%1\\s+([A-Za-z0-9_]+);)").arg(m_formclassnam));
    if(uidefRe.indexIn(out) > -1) {
        QString def = uidefRe.cap(1);
        QString uinam = uidefRe.cap(2);
        QString heapdef = def.replace(uinam, "* " + uinam);
        out.replace(uidefRe.cap(1), "// " + uidefRe.cap(1) + "\n" + heapdef + "\n");
    }

    // 2 expand constructor
    //
    QString constructorArgs;
    // capture the constructor declaration and expand;
    // \n*(\s*Danfisik9000\(([QWidget|QMainWindow][\s*\*\s*=A-Za-z0-9_]+)\);)
    QRegExp constrRe(QString("\\n*(\\s*%1\\(([QWidget|QMainWindow][\\s*\\*\\s*=A-Za-z0-9_]+)\\);)").arg(m_mainwnam));
    int pos = constrRe.indexIn(out);
    if(pos > -1 ) {
        QString constructorDecl = constrRe.cap(1);
        constructorArgs = constrRe.cap(2);
        if(!constructorDecl.isEmpty() && !constructorArgs.isEmpty()) {
            constructorDecl.replace(constructorArgs, "CumbiaTango *cut, " + constructorArgs);
            out.replace(constrRe.cap(1), constructorDecl);
        }
    }
    return out;
}

QString MainWidgetHProcessCmd::name()
{
    return "MainWidgetHProcessCmd";
}
