#include "mainwidgethprocesscmd.h"
#include <QRegularExpression>
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
    QRegularExpression uidefRe(QString("\\n(\\s*Ui::%1\\s+([A-Za-z0-9_]+);)").arg(m_formclassnam));
    QRegularExpressionMatch ma = uidefRe.match(out);
    if(ma.hasMatch() && ma.capturedTexts().size() > 2) {
        QString def = ma.captured(1);
        QString uinam = ma.captured(2);
        QString heapdef = def.replace(uinam, "* " + uinam);
        out.replace(ma.captured(1), "// " + ma.captured(1) + "\n" + heapdef + "\n");
    }

    // 2 expand constructor
    //
    QString constructorArgs;
    // capture the constructor declaration and expand;
    // \n*(\s*Danfisik9000\(([QWidget|QMainWindow][\s*\*\s*=A-Za-z0-9_]+)\);)
    QRegularExpression constrRe(QString("\\n*(\\s*%1\\(([QWidget|QMainWindow][\\s*\\*\\s*=A-Za-z0-9_]+)\\);)").arg(m_mainwnam));
    ma = constrRe.match(out);
    if(ma.hasMatch() && ma.capturedTexts().size() > 2) {
        QString constructorDecl = ma.captured(1);
        constructorArgs = ma.captured(2);
        if(!constructorDecl.isEmpty() && !constructorArgs.isEmpty()) {
            constructorDecl.replace(constructorArgs, "CumbiaPool *cu_p, " + constructorArgs);
            out.replace(ma.captured(1), constructorDecl);
        }
    }
    return out;
}

QString MainWidgetHProcessCmd::name()
{
    return "MainWidgetHProcessCmd";
}
