#include "uicproc.h"
#include "defs.h"
#include <QList>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QCoreApplication>
#include <QRegExp>

UicProc::UicProc()
{

}

QString UicProc::getUicCmd(const QString &ui_h_fname, const SearchDirInfoSet &dirInfoSet)
{
    QString cmd;
    QList<SearchDirInfo> di_ui_h_list = dirInfoSet.getDirInfoList(SearchDirInfoSet::Ui_H);
    m_error = di_ui_h_list.size() != 1;
    if(m_error)
    {
        m_lastError = "UicProc.getUicCmd: there must be only one configured ui output dir for ui_*.h files";
        return "";
    }
    QString uidir = di_ui_h_list.first().name();
    QDir ui(uidir);
    if(!ui.exists())
    {
        QDir wd;
        m_error = !wd.mkdir(uidir);
    }
    if(m_error)
    {
        m_lastError = "UicProc.getUicCmd: error creating ui directory \"" + uidir + "\"";
        return cmd; // empty
    }

    // get qt uic path from makefile
    QFile mkf("Makefile");
    m_error = !mkf.open(QIODevice::ReadOnly|QIODevice::Text);
    if(m_error)
    {
        m_lastError = "UicProc.getUicCmd: error opening Makefile in read mode: be sure to run qmake before " +
                qApp->applicationName() + " and to execute "  + qApp->applicationName() + " from the "
                "directory where .pro and Makefile files are. You can pass the \"--qmake\" option on the command line "
                "or configure cuuimake to automatically execute \"qmake\" before everything else "
                "(execute cuuimake --configure to set up the application). "
                "The error is: \""+ mkf.errorString() + "\"";
        return cmd; // empty
    }
    QString binpath;
    QTextStream in(&mkf);
    QRegExp re("QMAKE\\s*=\\s*([A-Za-z_0-9\\.\\-/]*)/qmake");
    while(!in.atEnd())
    {
        QStringList list;
        QString line = in.readLine();
        int pos = re.indexIn(line);
        if(pos > -1)
        {
            list = re.capturedTexts();
            if(list.size() == 2)
                binpath = list.last();
            break;
        }
    }

    /* find ui input file */
    QList<SearchDirInfo> di_ui_list = dirInfoSet.getDirInfoList(SearchDirInfoSet::Ui);
    QString ui_h_filenam(ui_h_fname); // ui_h_fname is const
    QString ui_in = ui_h_filenam.remove("ui_").remove(".h") + ".ui";
    foreach(SearchDirInfo sdi, di_ui_list) {
        QString dnam = sdi.name();
        QFile f(dnam + "/" + ui_in);
        if(f.exists())
        {
            ui_in = dnam + "/" + ui_in;
            break;
        }
    }
    m_error = ui_in.isEmpty();
    if(m_error)
        m_lastError = "UicProc.getUicCmd: input UI file \"" + ui_in + "\" does not exist";
    else
        cmd = binpath + "/uic " + ui_in + " -o " + uidir + "/" + ui_h_fname;

    return cmd;
}

bool UicProc::run(const QString &cmd)
{
    QProcess p;
    QString program;
    QStringList params = cmd.split(QRegExp("\\s+"));
    program = params.first();
    params.removeAt(0);
    p.start(program, params);
    bool finished = p.waitForFinished(5000);
    QProcess::ExitStatus es = p.exitStatus();
    if(finished && es == QProcess::NormalExit && p.exitCode() == 0)
        m_error = false;
    else if(finished && es == QProcess::NormalExit)
    {
        m_error = true;
        m_lastError = "UicProc.run: process \"" + cmd + "\" returned code " + QString::number(p.exitCode()) +
                p.readAllStandardOutput();
    }
    else
    {
        m_error = true;
        if(es == QProcess::CrashExit)
            m_lastError = "UicProc.run: process \"" + cmd + "\" crashed";
    }
    return !m_error;
}
