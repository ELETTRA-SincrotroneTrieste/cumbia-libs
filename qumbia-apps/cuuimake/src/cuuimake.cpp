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
            m_options->getopt("configure").toBool() ||
            m_options->getopt("list-options").toBool();

    m_debug = m_options->getopt("debug").toBool();
    if(m_options->getopt("show-config").toBool())
        m_options->printOptions(*this);
    else if(m_options->getopt("help").toBool())
        m_options->printHelp(*this);
    else if(m_options->getopt("list-options").toBool())
        m_options->printOptionsList(*this);
    else if(m_options->getopt("configure").toBool())
        m_options->configure(*this);
}

CuUiMake::~CuUiMake()
{
    delete m_options;
}

void CuUiMake::print(CuUiMake::Step step, bool err, bool plain_text, const char *fmt, ...) const
{
    va_list s;
    FILE* fd;
    va_start( s, fmt);
    if(!err)
        fd = stdout;
    else
        fd = stderr;

    char color[16];
    char cuuim_color[16];
    if(plain_text) {
        strcpy(color, "");
        strcpy(cuuim_color, "");
    }
    else if(err) {
        strcpy(color, "\e[1;31m");
        strcpy(cuuim_color, "\e[1;36m");
    }
    else {
        strcpy(color, "\e[1;32m");
        strcpy(cuuim_color, "\e[1;36m");
    }

    if(step == Uic)
        fprintf(fd, "[%scuuimake\e[0m:%s     uic\e[0m] ", cuuim_color,  color);
    else if(step == Analysis)
        fprintf(fd, "[%scuuimake\e[0m:%sanalysis\e[0m] ", cuuim_color, color);
    else if(step == Expand)
        fprintf(fd, "[%scuuimake\e[0m:%s  expand\e[0m] ", cuuim_color, color);
    else if(step == Conf)
        fprintf(fd, "[%scuuimake\e[0m:%s  config\e[0m] ", cuuim_color, color);
    else if(step == Help)
        fprintf(fd, "[%scuuimake\e[0m:%s    help\e[0m] ", cuuim_color, color);
    else if(step == Info)
        fprintf(fd, "[%scuuimake\e[0m:%s    info\e[0m] ", cuuim_color, color);
    else if(step == QMake)
        fprintf(fd, "[%scuuimake\e[0m:%s   qmake\e[0m] ", cuuim_color, color);
    else if(step == Make)
        fprintf(fd, "[%scuuimake\e[0m:%s    make\e[0m] ", cuuim_color, color);
    else if(step == Clean)
        fprintf(fd, "[%scuuimake\e[0m:%s   clean\e[0m] ", cuuim_color, color);
    else if(step == Doc)
        fprintf(fd, "[%scuuimake\e[0m:%s     doc\e[0m] ", cuuim_color, color);

    vfprintf(fd, fmt, s);

    va_end(s);
}

bool CuUiMake::make()
{
    bool plain_text = m_options->getopt("plain-text-output").toBool();
    char color[16], white[16];
    plain_text ? strcpy(color, "") : strcpy(color, "\e[1;35m");
    plain_text ? strcpy(white, "") : strcpy(white, "\e[0m");

    // load configuration files
    int removed_ui_cnt = -1;
    Defs defs;
    defs.setDebug(m_debug);
    QString fname (CONFDIR + QString("/cuuimake-cumbia-qtcontrols.xml"));

    QString localfname = m_findLocalConfFile();
    bool success = defs.loadConf(fname, localfname);
    print(Doc, false, plain_text, "file://%s/%s\n", CUUIMAKE_DOCDIR, "html/cuuimake.html");
    if(localfname.isEmpty()) {
        print(Analysis, false, plain_text, "%sinfo%s: if any promoted widget is not automatically detected you can add a text file named \n", color, white);
        print(Analysis, false, plain_text, "%sinfo%s: cuuimake[.*].conf with the list of the \e[0;4mpromoted widget\e[0m names to expand\n", color, white);
    }
    if(!success)
        print(Analysis, true, "error loading configuration file \"%s\": %s\n", qstoc(fname), qstoc(defs.lastError()));
    else {
        foreach(QString msg, defs.messages)
            print(Analysis, false, "%s\n", qstoc(msg));
        print(Analysis, false, plain_text, "default configuration loaded\n");
    }

    bool pre_clean = m_options->getopt("pre-clean").toBool();
    bool clean = m_options->getopt("clean").toBool();
    bool refresh = m_options->getopt("refresh").toBool();
    if(success && (clean || pre_clean || refresh) ) {
        Processor p;
        removed_ui_cnt = p.remove_UI_H(defs.srcDirsInfo());
        print(Clean, false, plain_text, "removed %d ui_.*.h files\n", removed_ui_cnt);
    }
    if(success && (clean || pre_clean) )
    {
        print(Clean, false, plain_text, "cleaning and removing ui_*.h files\n");
        XMakeProcess xmake;
        success = xmake.clean();
        if(!success)
            print(Clean, true, plain_text, "failed to execute clean: %s\n", qstoc(xmake.lastError()));
        if(!m_options->getopt("pre-clean").toBool())
            return success;
        // otherwise go on
    }

    if(success && m_options->getopt("qmake").toBool())
    {
        print(QMake, false, plain_text, "running qmake\n");
        XMakeProcess xmake;
        success = xmake.qmake();
        if(!success)
            print(QMake, true, plain_text, "failed to execute qmake: %s\n", qstoc(xmake.lastError()));
    }

    if(success)
    {

        SearchDirInfoSet searchDirInfoSet = defs.srcDirsInfo();
        Substitutions substitutions;
        print(Analysis, false, plain_text, "parsing files to find substitutions...\n");
        Parser p;
        p.setDebug(m_debug);
        QString mode = p.detect(searchDirInfoSet, defs.getSearchList(), substitutions);
        success = !mode.isEmpty();
        if(!success)
        {
            print(Analysis, true, plain_text, "error: no useful object definitions found:\n");
            foreach(Search s, defs.getSearchList())
                print(Analysis, true, plain_text, "%s", qstoc(s.toString()));
            print(Analysis, true, plain_text, "\n");
            print(Analysis, true, plain_text, "in the .cpp files.\n\n");
            print(Analysis, true, plain_text, "The following information may be useful:\n");

            fflush(stderr);
            fflush(stdout);
            print(Analysis, true, plain_text, "\n");
            print(Analysis, true, plain_text, "You can use the cumbia project wizard to create a project\n\n");
        }
        else
        {
            print(Analysis, false, plain_text, "substitutions: %s\n", qstoc(substitutions.toString()));
            print(Expand, false, plain_text, "processing ui_h file[s]...\n", qstoc(substitutions.toString()));
            Processor processor;
            UicProc uicp;
            QMap<QString, bool> uifmap = processor.findUI_H(searchDirInfoSet);
            foreach(QString uif, uifmap.keys())
            {
                if(uifmap[uif])
                {
                    QString cmd = uicp.getUicCmd(uif, searchDirInfoSet);
                    if(cmd.isEmpty())
                        print(Uic, true, plain_text, "error executing uic on file  \"%s\": \"%s\"\n", qstoc(uif), qstoc(uicp.lastError()));
                    else
                    {
                        print(Uic, false, plain_text, "executing \%s\"\n", qstoc(cmd));
                        success = uicp.run(cmd);
                        if(success)
                        {
                            // ui file more recent than ui_*.h file or ui_h file not existing
                            success = processor.expand(substitutions, defs.getObjectMap(), uif, searchDirInfoSet);
                            if(!success)
                                print(Expand, true, plain_text, "error expanding file \"%s\": \"%s\"\n", qstoc(uif), qstoc(processor.lastError()));
                            else
                                print(Expand, false, plain_text, "file \"%s\": successfully expanded\n", qstoc(uif), qstoc(processor.lastError()));
                        }
                        else
                            print(Uic, true, plain_text, "error executing \"%s\": \"%s\"\n", qstoc(cmd), qstoc(uicp.lastError()));
                    }
                }
                else
                    print(Expand, false, plain_text, "no need to expand \"%s\"...\n", qstoc(uif));
            }

        }
    }

    if(success && m_options->getopt("make").toBool())
    {
        print(Make, false, plain_text, "building project\n");
        QString minus_j_opt = m_options->getopt("makej").toString();
        XMakeProcess xmake;
        success = xmake.make(minus_j_opt);
        if(!success)
            print(Make, true, plain_text, "failed to execute \e[1;31mmake %s\e[0m: %s\n", qstoc(minus_j_opt), qstoc(xmake.lastError()));
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
