#include "maincppexpandcmd.h"

MainCppExpandCmd::MainCppExpandCmd(const QString &fnam, const QString &mainwnam)
    : FileCmd(fnam)
{
    m_mainwclass = mainwnam;
}

//
// expand main widget's constructor to pass CumbiaPool
// find CumbiaPool variable name:   CumbiaPool\s*\*\s*([A-Za-z0-9_]*)\s*=\s*new\s+CumbiaPool
// find main widget constructor Danfisik9000\s*\*w\s*=\s*new\s*Danfisik9000(\(([0|NULL|nullptr])\));
// expand main widget constructor to pass cumbia tango variable
QString MainCppExpandCmd::process(const QString &input)
{
    QString out = input;
    QRegExp cupoolRe ("CumbiaPool\\s*\\*\\s*([A-Za-z0-9_]*)\\s*=\\s*new\\s+CumbiaPool");
    m_err = cupoolRe.indexIn(out) < 0;
    if(!m_err) {
        QString cuta = cupoolRe.cap(1);
        QRegExp mainwRe(QString("%1\\s*\\*w\\s*=\\s*new\\s*%1(\\(([0|NULL|nullptr])\\));").arg(m_mainwclass));
        int pos = mainwRe.indexIn(out);
        if(pos > -1) {
            QString params = mainwRe.cap(1);
            QString param = mainwRe.cap(2);
            QString mainw = mainwRe.cap(0);
            mainw.replace(params, "(" + cuta + ", " + param + ")");
            out.replace(mainwRe.cap(0), mainw);
            m_log.append(OpQuality("main.cpp expand", mainwRe.cap(0), mainw, filename(),
                                   "expanded main widget constructor", Quality::Ok, -1));
        }
    }
    return out;
}

QString MainCppExpandCmd::name()
{
    return "MainCppExpandCmd";
}
