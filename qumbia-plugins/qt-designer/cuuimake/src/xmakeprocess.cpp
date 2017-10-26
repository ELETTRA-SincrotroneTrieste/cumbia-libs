#include "xmakeprocess.h"
#include "defs.h" // for qstoc
#include <QProcess>

XMakeProcess::XMakeProcess()
{
    m_error = false;
}

bool XMakeProcess::qmake()
{
    QProcess p;
    QString program = "qmake";
    p.start(program);
    bool finished = p.waitForFinished(5000);
    QProcess::ExitStatus es = p.exitStatus();
    if(finished && es == QProcess::NormalExit && p.exitCode() == 0)
        m_error = false;
    else if(finished && es == QProcess::NormalExit)
    {
        m_error = true;
        m_lastError = "XMakeProcess.qmake: process \"" + program + "\" returned code " + QString::number(p.exitCode()) +
                p.readAllStandardOutput();
    }
    else
    {
        m_error = true;
        if(es == QProcess::CrashExit)
            m_lastError = "XMakeProcess.qmake: process \"" + program + "\" crashed";
    }
    return !m_error;
}

bool XMakeProcess::make(const QString& minus_jN)
{
    QProcess p;
    QString program = "make";
    QStringList params;
    if(!minus_jN.isEmpty())
        params << minus_jN;
    connect(&p, SIGNAL(readyReadStandardError()), this, SLOT(onStdErrReady()));
    connect(&p, SIGNAL(readyReadStandardOutput()), this, SLOT(onStdOutReady()));
    p.start(program, params);
    bool finished = p.waitForFinished();
    QProcess::ExitStatus es = p.exitStatus();
    if(finished && es == QProcess::NormalExit && p.exitCode() == 0)
        m_error = false;
    else if(finished && es == QProcess::NormalExit)
    {
        m_error = true;
        m_lastError = "XMakeProcess.make: process \"" + program + "\" returned code " + QString::number(p.exitCode());
    }
    else
    {
        m_error = true;
        if(es == QProcess::CrashExit)
            m_lastError = "XMakeProcess.make: process \"" + program + "\" crashed";
    }
    return !m_error;
}

void XMakeProcess::onStdErrReady()
{
    QProcess *p = qobject_cast<QProcess *>(sender());
    QString e = p->readAllStandardError();
    printf("\e[1;31m%s\e[0m\n", qstoc(e));
}

void XMakeProcess::onStdOutReady()
{
    QProcess *p = qobject_cast<QProcess *>(sender());
    QString e = p->readAllStandardOutput();
    printf("%s\n", qstoc(e));
}
