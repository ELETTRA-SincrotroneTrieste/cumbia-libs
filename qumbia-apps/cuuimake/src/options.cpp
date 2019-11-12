#include "options.h"
#include "defs.h" // for qstoc
#include "cuuimake.h" // for print
#include <QSettings>
#include <QCoreApplication>
#include <QtDebug>

#include <unistd.h>
#include <stdio.h>
#include <termios.h>

// --param-name=param,list regexp: \-\-[A-Za-z_\.\-]+=[A-Za-z_0-9,\.\-]+
#define LIST_PARAM_RE "\\-\\-[A-Za-z_\\.\\-]+=[A-Za-z_0-9,\\.\\-]+"

Options::Options(const QStringList& args)
{
    m_error = false;
    // boolean options
    m_helpMap.insert("--help", "print usage");
    m_helpMap.insert("--qmake", "execute qmake before analysing and expanding ui/ui_*.h files");
    m_helpMap.insert("-jN", "execute make -jN after analysing and expanding ui/ui_*.h files (--make is implied)");
    m_helpMap.insert("--make", "execute make after analysing and expanding ui/ui_*.h files");
    m_helpMap.insert("--configure", "run the configuration wizard. All other options will be ignored");
    m_helpMap.insert("--show-config", "shows the configuration according to the application settings and the command line parameters specified. "
                                      "No Analysis nor Expansion is performed.");
    m_helpMap.insert("--refresh", "remove ui_*.h files to refresh them before bulding the project again.");
    m_helpMap.insert("--clean", "execute make clean and remove ui_*.h files and exit.");
    m_helpMap.insert("--pre-clean", "like \"--clean\" but do not exit.");
    m_helpMap.insert("--plain-text-output", "disable colored output.");
    m_helpMap.insert("--debug", "additional information is printed while operations are performed");

    // options with --option=something
    // m_helpMap.insert("--add-params=par1,par2,...", "skip analysis and just add the specified parameters to the cumbia widgets at construction time");


    QStringList params = args;
    params.removeFirst(); // app name
    QString key;
    QStringList list;
    QStringList helpKeys = m_helpMap.keys();

    // Settings from QSettings
    QSettings s;
    QRegExp make_j_re("\\-j[0-9]{1,2}");
    QRegExp re(LIST_PARAM_RE);
    foreach(QString hk, helpKeys)
    {
        if(hk.startsWith("--") && hk.contains(re)) {
            // options like --option=something store data into QString
            list = hk.split("=");
            QString  k = list.first().remove("--");
            if(s.contains(k))
            {
                QVariant v = s.value(k, "");
                m_map.insert(k, v.toString());
            }
        }
        else if(hk.startsWith("--"))
        {
            QString  k = hk.remove("--");
            m_map.insert(k, s.value(k, false).toBool());
        }
        else if(hk.startsWith("-j")) // special case
        {
            QString jopt = s.value("makej", "").toString();
            if(!jopt.isEmpty())
            {
                m_map.insert("makej", jopt);
                m_map.insert("make", true);
            }
        }

    }

    // command line arguments override QSettings
    foreach(QString a, params)
    {
        if(a.startsWith("--") && a.contains(QRegExp(LIST_PARAM_RE)))
        {
            list = a.split("=");
            key = list.first().remove("--");
            m_map.insert(key, list.last());

        }
        else if(a.startsWith("--"))
            m_map.insert(a.remove("--"), true);
        else if(a.contains(make_j_re))
        {
            m_map.insert("makej", a);
            m_map.insert("make", true);
        }
        else
        {
            m_error = true;
            m_lastError = "Options.Options: invalid command line parameter: \"" + a + "\"";
        }


    }
}

QVariant Options::getopt(const QString &name)
{
    return m_map.value(name);
}

void Options::printHelp(const CuUiMake& cm) const
{
    bool plain_text = m_map["plain-text-output"].toBool();
    char color[16];
    char white[16];
    char italic[16];
    plain_text ? strcpy(color, "") : strcpy(color, "\e[1;33m");
    plain_text ? strcpy(white, "") : strcpy(white, "\e[0m");
    plain_text ? strcpy(italic, "") : strcpy(italic, "\e[1;37;3m");

    cm.print(CuUiMake::Help, false, plain_text, "\n%s command line options:\n\n", qstoc(qApp->applicationName()));
    QStringList lines;
    QString help;
    foreach(QString k, m_helpMap.keys())
    {
        help = m_helpMap[k];
        lines = help.split("\n");
        foreach(QString l, lines)
            cm.print(CuUiMake::Help, false, plain_text, "%s%s%s:%s\t%s\n", color, qstoc(k), white, italic, qstoc(l), white);
    }
    cm.print(CuUiMake::Help, false, plain_text, "\n\n");
    cm.print(CuUiMake::Help, false, plain_text, "\e[%s***\e[0m\tif a file with a name matching \"cuuimake[.*].conf\" is found in the\n", color);
    cm.print(CuUiMake::Help, false, plain_text, "\e[%s***\e[0m\tsame directory as cuuimake is run, then it is used to expand cumbia objects\n", color);
    cm.print(CuUiMake::Help, false, plain_text, "\e[%s***\e[0m\tthat have not been automatically detected.\n", color);
    cm.print(CuUiMake::Help, false, plain_text, "\e[%s***\e[0m\tExample of line: \"TDialWrite, cumbiatango, CumbiaTango *, CuTWriterFactory\"\n", color);
    cm.print(CuUiMake::Help, false, plain_text, "\e[%s***\e[0m\t                 \"ClassName, factory [cumbiatango,cumbiaepics,cumbiapool], first_param, second_param, ...\"\n", color);
    printf("\e[0m\n");


}

void Options::printOptionsList(const CuUiMake &) const
{
    foreach(QString k, m_helpMap.keys()) {
        printf("%s ", k.toStdString().c_str());
    }
    printf("\n");
}

void Options::printOptions(const CuUiMake &cm, const QStringList &theseOptions) const
{
    bool plain_text = m_map["plain-text-output"].toBool();
    cm.print(CuUiMake::Conf, false, plain_text, "%s options:\n", qstoc(qApp->applicationName()));
    QRegExp re(LIST_PARAM_RE);
    QRegExp make_j_re("\\-j[0-9]{1,2}");

    QStringList optionNames;
    if(!theseOptions.isEmpty())
        optionNames = theseOptions;
    else
        optionNames = m_helpMap.keys(); // all options
    foreach(QString k, optionNames)
    {
        QString help = m_helpMap[k];
        QString settingKey;
        QString val;
        if(k.contains(re))
        {
            settingKey = k.split("=").first().remove("--");
            if(m_map.contains(settingKey))
                cm.print(CuUiMake::Conf, false, plain_text, "\e[1;33m%s\e[0m\t[\e[0;32;4m%s\e[0m]\t[\e[1;37;3m%s\e[0m]\n", qstoc(settingKey), qstoc(m_map.value(settingKey).toString()), qstoc(help));
        }
        else if(k.startsWith("-j"))
        {
            settingKey = "makej";
            if(m_map.contains("makej"))
                cm.print(CuUiMake::Conf, false, plain_text, "\e[1;33mmake -j\e[0m\t[\e[0;32;4m%s\e[0m]\t[\e[1;37;3m%s\e[0m]\n",
                         qstoc(m_map.value("makej").toString()), qstoc(m_helpMap["-jN"]));
        }
        else if(k.startsWith("--"))
        {
            settingKey = k.remove("--");
            m_map.contains(settingKey) && m_map.value(settingKey).toBool() ? val = "true" : val = "false";
            cm.print(CuUiMake::Conf, false, plain_text, "\e[1;33m%s\e[0m\t[\e[0;32;4m%s\e[0m]\t[\e[1;37;3m%s\e[0m]\n", qstoc(settingKey), qstoc(val), qstoc(help));

        }
    }
}

bool Options::configure(const CuUiMake &cm)
{
    bool plain_text = m_map["plain-text-output"].toBool();
    QStringList configurableOptions = QStringList() << "--qmake" << "--make" << "--pre-clean" << "--refresh";
    unsigned char c;

    struct termios old_tio, new_tio;
    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO,&old_tio);

    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;

    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &=(~ICANON & ~ECHO);

    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

    QSettings s;
    QString val;
    foreach(QString k, configurableOptions)
    {
        k.remove("--");
        m_map.contains(k) && m_map.value(k).toBool() ? val = "true" : val = "false";
        do{
            cm.print(CuUiMake::Conf, false, plain_text, "\n");
            cm.print(CuUiMake::Conf, false,plain_text, "* option \"--%s\": [\e[1;37;3m%s\e[0m]: current setting: [\e[0;32;4m%s\e[0m] enable [y/n]? ",
                     qstoc(k), qstoc(m_helpMap.value("--" + k)), qstoc(val));
            c = getchar();
            printf("\n");
            cm.print(CuUiMake::Conf, false, plain_text, "*           %s : [\e[0;34m%s\e[0m] --> [\e[1;32m%s\e[0m]\n",
                     qstoc(k), qstoc(val), c == 'y' ? "true" : "false");
        } while(c != 'y' && c != 'n');
        s.setValue(k, (c == 'y'));
    }
    /* restore the former settings */
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

    // make -jN option
    if(s.value("make").toBool())
    {
        int j;
        int match;
        QString makejopt = s.value("makej").toString();
        if(makejopt.isEmpty())
            makejopt = "unset";
        do{
            printf("\n");
            cm.print(CuUiMake::Conf, false, plain_text, "* make -jN option: [\e[1;37;3m%s\e[0m]: current setting: [\e[0;32;4m%s\e[0m]: set new integer [1-20]: ",
                 qstoc(m_helpMap.value("-jN")), qstoc(makejopt));

            match = scanf("%d", &j);
            while(getchar() != '\n');

        }while(match < 1 || j < 1 || j > 20);
        s.setValue("makej", "-j" + QString::number(j));
    }

    printf("\n");
    cm.print(CuUiMake::Info, false, plain_text, "\e[1;33;4mnote\e[0m\n");
    cm.print(CuUiMake::Info, false, plain_text, "1. configuration can be overridden by command line arguments\n");
    cm.print(CuUiMake::Info, false, plain_text, "2. execute \"%s --show-config\" to show the application settings\n", qstoc(qApp->arguments().at(0)));
    printf("\n");

    return true;
}
