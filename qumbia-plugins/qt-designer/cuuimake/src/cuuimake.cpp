#include "cuuimake.h"
#include "defs.h"
#include "parser.h"
#include "processor.h"

#include <stdio.h>
#include <QFile>
#include <QIODevice>
#include <QProcess>
#include <QDir>
#include <QCoreApplication>

CuUiMake::CuUiMake()
{
    m_debug = qApp->arguments().contains("--debug");
}

void CuUiMake::print(CuUiMake::Step step, bool err, const char *fmt, ...)
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

    if(success)
    {
        SearchDirInfoSet searchDirInfoSet = defs.srcDirsInfo();
        Substitutions subst;
        print(Analysis, false, "parsing files to find substitutions...\n");
        Parser p;
        p.setDebug(m_debug);
        QString mode = p.detect(searchDirInfoSet, defs.getSearchList(), subst);
        success = !mode.isEmpty();
        if(!success)
        {
            print(Analysis, true, "error: no useful object definitions found:\n");
            foreach(Search s, defs.getSearchList())
                print(Analysis, true, "%s", qstoc(s.toString()));
            print(Analysis, true, "\n");
            print(Analysis, true, "in the .cpp files.\n\n");
            print(Analysis, true, "The following information may be useful:\n");
            QList<SearchDirInfo> dilist = searchDirInfoSet.getDirInfoList(SearchDirInfoSet::Source);
            foreach(SearchDirInfo di, dilist)
                printf("%s\n", qstoc(di.toString()));

            fflush(stderr);
            fflush(stdout);
            print(Analysis, true, "\n");
            print(Analysis, true, "You can use the cumbia project wizard to create a project\n\n");
        }
        else
        {
            print(Analysis, false, "substitutions: %s\n", qstoc(subst.toString()));
            print(Expand, false, "processing ui_h file[s]...\n", qstoc(subst.toString()));
            Processor processor;
            QMap<QString, bool> uifmap = processor.findUI_H(searchDirInfoSet);
            foreach(QString uif, uifmap.keys())
            {
                if(uifmap[uif])
                {
                    // ui file more recent than ui_*.h file or ui_h file not existing
                    print(Expand, false, "processing \"%s\"...\n", qstoc(uif));

                }
                else
                    print(Expand, false, "no need to process \"%s\"...\n", qstoc(uif));
            }

        }
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
