#include "cuuimake.h"
#include "uicproc.h"
#include "defs.h"
#include "parser.h"
#include "processor.h"
#include "xmakeprocess.h"
#include "options.h"

#include <stdio.h>
#include <QFile>
#include <QIODevice>
#include <QProcess>
#include <QDir>
#include <QCoreApplication>

CuUiMake::CuUiMake()
{
    m_options = new Options(qApp->arguments());
    m_dummy = m_options->getopt("show-config").toBool() ||
            m_options->getopt("help").toBool() ||
            m_options->getopt("configure").toBool();

    m_debug = m_options->getopt("debug").toBool();
    if(m_options->getopt("show-config").toBool())
        m_options->printOptions(*this);
    else if(m_options->getopt("help").toBool())
        m_options->printHelp(*this);
    else if(m_options->getopt("configure").toBool())
        m_options->configure(*this);
}

CuUiMake::~CuUiMake()
{
    delete m_options;
}

void CuUiMake::print(CuUiMake::Step step, bool err, const char *fmt, ...) const
{
    va_list s;
    FILE* fd;
    va_start( s, fmt);
    if(!err)
        fd = stdout;
    else
        fd = stderr;

    char color[16];
    if(err)
        strcpy(color, "\e[1;31m");
    else
        strcpy(color, "\e[1;32m");

    if(step == Uic)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%s     uic\e[0m] ", color);
    else if(step == Analysis)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%sanalysis\e[0m] ", color);
    else if(step == Expand)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%s  expand\e[0m] ", color);
    else if(step == Conf)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%s  config\e[0m] ", color);
    else if(step == Help)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%s    help\e[0m] ", color);
    else if(step == Info)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%s    info\e[0m] ", color);
    else if(step == QMake)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%s   qmake\e[0m] ", color);
    else if(step == Make)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%s    make\e[0m] ", color);
    else if(step == Clean)
        fprintf(fd, "[\e[1;36mcuuimake\e[0m:%s   clean\e[0m] ", color);

    vfprintf(fd, fmt, s);

    va_end(s);
}

bool CuUiMake::make()
{
    // load configuration files
    Defs defs;
    defs.setDebug(m_debug);
    QString fname (SHAREDIR + QString("/cuuimake-cumbia-qtcontrols.xml"));

    QString localfname = m_findLocalConfFile();
    bool success = defs.loadConf(fname, localfname);
    if(localfname.isEmpty())
        print(Analysis, false, "\e[1;35minfo\e[0m: you can add a text file named cuuimake[.*].conf "
                               "with the list of the promoted widget names to expand\n");
    if(!success)
        print(Analysis, true, "error loading configuration file \"%s\": %s\n", qstoc(fname), qstoc(defs.lastError()));
    else
        print(Analysis, false, "default configuration loaded\n");

    if(success && m_options->getopt("clean").toBool())
    {
        print(Clean, false, "cleaning and removing ui_*.h files\n");
        XMakeProcess xmake;
        success = xmake.clean();
        if(!success)
            print(Clean, true, "failed to execute clean: %s\n", qstoc(xmake.lastError()));
        else {
            Processor p;
            int remcnt = p.remove_UI_H(defs.srcDirsInfo());
            print(Clean, false, "removed %d ui_.*.h files\n", remcnt);
        }
        return success;
    }

    if(success && m_options->getopt("qmake").toBool())
    {
        print(QMake, false, "running qmake\n");
        XMakeProcess xmake;
        success = xmake.qmake();
        if(!success)
            print(QMake, true, "failed to execute qmake: %s\n", qstoc(xmake.lastError()));
    }

    if(success)
    {

        SearchDirInfoSet searchDirInfoSet = defs.srcDirsInfo();
        Substitutions substitutions;
        print(Analysis, false, "parsing files to find substitutions...\n");
        Parser p;
        p.setDebug(m_debug);
        QString mode = p.detect(searchDirInfoSet, defs.getSearchList(), substitutions);
        success = !mode.isEmpty();
        if(!success)
        {
            print(Analysis, true, "error: no useful object definitions found:\n");
            foreach(Search s, defs.getSearchList())
                print(Analysis, true, "%s", qstoc(s.toString()));
            print(Analysis, true, "\n");
            print(Analysis, true, "in the .cpp files.\n\n");
            print(Analysis, true, "The following information may be useful:\n");

            fflush(stderr);
            fflush(stdout);
            print(Analysis, true, "\n");
            print(Analysis, true, "You can use the cumbia project wizard to create a project\n\n");
        }
        else
        {
            print(Analysis, false, "substitutions: %s\n", qstoc(substitutions.toString()));
            print(Expand, false, "processing ui_h file[s]...\n", qstoc(substitutions.toString()));
            Processor processor;
            UicProc uicp;
            QMap<QString, bool> uifmap = processor.findUI_H(searchDirInfoSet);
            foreach(QString uif, uifmap.keys())
            {
                if(uifmap[uif])
                {
                    QString cmd = uicp.getUicCmd(uif, searchDirInfoSet);
                    if(cmd.isEmpty())
                        print(Uic, true, "error executing uic on file  \"%s\": \"%s\"\n", qstoc(uif), qstoc(uicp.lastError()));
                    else
                    {
                        print(Uic, false, "executing \%s\"\n", qstoc(cmd));
                        success = uicp.run(cmd);
                        if(success)
                        {
                            // ui file more recent than ui_*.h file or ui_h file not existing
                            success = processor.expand(substitutions, defs.getObjectMap(), uif, searchDirInfoSet);
                            if(!success)
                                print(Expand, true, "error expanding file \"%s\": \"%s\"\n", qstoc(uif), qstoc(processor.lastError()));
                            else
                                print(Expand, false, "file \"%s\": successfully expanded\n", qstoc(uif), qstoc(processor.lastError()));
                        }
                        else
                            print(Uic, true, "error executing \"%s\": \"%s\"\n", qstoc(cmd), qstoc(uicp.lastError()));
                    }
                }
                else
                    print(Expand, false, "no need to expand \"%s\"...\n", qstoc(uif));
            }

        }
    }

    if(success && m_options->getopt("make").toBool())
    {
        print(Make, false, "building project\n");
        QString minus_j_opt = m_options->getopt("makej").toString();
        XMakeProcess xmake;
        success = xmake.make(minus_j_opt);
        if(!success)
            print(Make, true, "failed to execute \e[1;31mmake %s\e[0m: %s\n", qstoc(minus_j_opt), qstoc(xmake.lastError()));
    }

    return true;
}

QString CuUiMake::m_findLocalConfFile() const
{
    QDir wd;
    QStringList files = wd.entryList(QDir::Files);
    int idx = files.indexOf(QRegExp("cuuimake.*.conf"));
    if(idx >= 0)
        return files.at(idx);
    return "";
}
