#include "cmdlineoptions.h"
#include <QRegularExpression>
#include "qumbia-reader.h" // for Verbosity enum
#include <cumacros.h>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>
#include <QTextStream>
#include <QtDebug>

#define QUMBIA_READER_DOC_URL "https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/qumbia-reader/html/index.html"

CmdLineOptions::CmdLineOptions(bool formula_plugin_enabled, bool historical_db_plugin_enabled)
{
   QStringList ops = QStringList () << "format" << "period" << "truncate" << "max-timers" << "out-level"
            << "single-shot" << "refresh-limit" << "monitor" << "property" << "help" << "help-topic" << "list-options";
   #if defined (HAS_CUHDB)
   ops << "db-profile" << "db-output-file";
#endif
#if defined (CUMBIA_WEBSOCKET_VERSION) || defined (CUMBIA_HTTP_VERSION)
   ops << "url";
#endif

   foreach(const QString& o, ops)
       m_help_map[o] = "";

#ifdef CUMBIA_RANDOM_VERSION
    m_help_map["help-random"] = "cumbia-random module specific help";
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    m_help_map["help-tango"] = "Tango module specific help";
#endif
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
    m_help_map["help-epics"] = "EPICS module specific help";
#endif
    if(formula_plugin_enabled)
        m_help_map["help-formula"] = "formula plugin specific help";
    if(historical_db_plugin_enabled)
        m_help_map["help-hdb"] = "historical database plugin specific help";
}

RConfig CmdLineOptions::parse(const QStringList &args)
{
    QCommandLineOption formatO(QStringList() << "f" << "format", "custom format for numbers, e.g. %d or %.2f", "format");
    QCommandLineOption periodO(QStringList() << "p" << "period", "refresh period is set to <millis>, if the source is not event driven", "millis");
    QCommandLineOption truncO(QStringList() << "t" << "truncate", "truncate output to a maximum <length>" , "length");
    QCommandLineOption maxTimersO(QStringList() << "x" << "max-timers", "limit the number of timers to <max_t>" "max_t");
    QCommandLineOption out_detailO(QStringList() << "l" << "out-level", "increase the default output level to <level> (medium|high|debug)", "level");
    QCommandLineOption singleShotO(QStringList() << "s" << "single-shot", "one shot operation");
    QCommandLineOption refreshLimitO(QStringList() << "r" << "refresh-limit", "read <n> times then exit" , "n");
    QCommandLineOption monitorO(QStringList() << "m" << "monitor", "monitor source(s) until a key is pressed");
    QCommandLineOption propertyO(QStringList() << "q" << "property", "get configuration properties (implies single-shot and l medium if not specified)");
    QCommandLineOption helpO(QStringList() << "h" << "help", "read the manual");
    QCommandLineOption help2O(QStringList() << "i" << "help-topic", "help topic specific help [--help-tango|--help-epics|--help-random]", "topic");
    QCommandLineOption listOptsO(QStringList() << "o" << "list-options", "list application options");

#if defined (HAS_CUHDB)
    QCommandLineOption dbProO(QStringList() << "d" << "db-profile", "set the db profile to <profile>", "profile");
    QCommandLineOption dbOutFileO(QStringList() << "b" << "db-output-file", "write historical db data into <file>", "file");
    m_parser.addOption(dbProO);
    m_parser.addOption(dbOutFileO);
#endif
#if defined (CUMBIA_WEBSOCKET_VERSION) || defined (CUMBIA_HTTP_VERSION)
    QCommandLineOption urlO(QStringList() << "u" << "url", "specify <url> for either http[s] or websocket, example: \"http://my.nginx-nchan.host.eu:8001/mychannel\"", "url");
    m_parser.addOption(urlO);
#endif

    m_parser.addOption(formatO);
    m_parser.addOption(periodO);
    m_parser.addOption(truncO);
    m_parser.addOption(maxTimersO);
    m_parser.addOption(out_detailO);
    m_parser.addOption(singleShotO);
    m_parser.addOption(refreshLimitO);
    m_parser.addOption(monitorO);
    m_parser.addOption(propertyO);
    m_parser.addOption(helpO);
    m_parser.addOption(help2O);
    m_parser.addOption(listOptsO);
    m_parser.parse(args);

    bool ok;
    RConfig o;
    QRegularExpression refreshLimitRe("\\-\\-(\\d+)");
    // fmtRe \-\-format=(%\d*\.{0,1}\d*[hxfegd])
    QRegularExpression fmtRe("(%\\d*\\.{0,1}\\d*[hxfegd])");
    if(m_parser.isSet(periodO) && m_parser.value(periodO).toInt(&ok) > 10 && ok) o.period = m_parser.value(periodO).toInt();
    if(m_parser.isSet(truncO) && m_parser.value(truncO).toInt(&ok) > 0 && ok) o.truncate = m_parser.value(truncO).toInt();
    if(m_parser.isSet(maxTimersO) && m_parser.value(maxTimersO).toInt(&ok) > 0 && ok) o.max_timers = m_parser.value(maxTimersO).toInt();


    if(m_parser.isSet(propertyO)) {
        o.refresh_limit = 1;
        o.property = true;
        if(!m_parser.isSet(out_detailO))
            o.verbosity = QumbiaReader::Medium;
    }
    if(m_parser.isSet(out_detailO)) {
        QString v =  m_parser.value(out_detailO);
        if(v == "medium") o.verbosity = QumbiaReader::Medium;
        else if(v == "high") o.verbosity = QumbiaReader::High;
        else if(v == "debug") o.verbosity = QumbiaReader::Debug;
    }
    if(m_parser.isSet(singleShotO)) o.refresh_limit = 1;
    if(m_parser.isSet(refreshLimitO) && m_parser.value(refreshLimitO).toInt(&ok) && ok) o.refresh_limit = m_parser.value(refreshLimitO).toInt();
    if(m_parser.isSet(formatO)) {
        QString a = m_parser.value(formatO);
        QRegularExpressionMatch ma = fmtRe.match(a);
        if(ma.hasMatch())
            o.format = m_parser.value(formatO);
        else
            perr("CmdLineOptions::parse: invalid -f format option: \"%s\"", qstoc(m_parser.value(formatO)));
    }
    if(m_parser.isSet(monitorO)) o.refresh_limit = 0;
    if(m_parser.isSet(helpO))
        o.usage = true;
    if(m_parser.isSet(help2O)) {
        o.usage = true;
        help(args.first(), m_parser.value(help2O));
    }

    o.help = m_parser.isSet(helpO);

    if(m_parser.isSet(listOptsO)) o.list_options = true;

 #if defined (HAS_CUHDB)
    if(m_parser.isSet(dbProO)) o.db_profile = m_parser.value(dbProO);
    if(m_parser.isSet(dbOutFileO)) o.db_output_file = m_parser.value(dbOutFileO);
#endif

#if defined (CUMBIA_WEBSOCKET_VERSION) || defined (CUMBIA_HTTP_VERSION)
    if(m_parser.isSet(urlO)) o.url = m_parser.value(urlO);
#endif

    foreach(QString pa, m_parser.positionalArguments())
        o.sources.append(pa);

    o.usage = o.sources.isEmpty() && !o.list_options;

    return o;
}

QString CmdLineOptions::help() {
    return m_parser.helpText();
}

void CmdLineOptions::help(const QString& appname, const QString &modulenam) const
{
    if(modulenam.isEmpty()) {
        QFile f(":/help/man.txt");
        if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QTextStream out(&f);
            QString txt = out.readAll();
            QRegularExpression re("(\\$.*\\n)");
            QRegularExpressionMatchIterator match_i = re.globalMatch(txt);
            while(match_i.hasNext()) {
                QRegularExpressionMatch ma = match_i.next();
                if(ma.capturedTexts().size() > 1)
                    txt.replace(ma.captured(1), QString("\e[1;32m%1\e[0m").arg(ma.captured(1)));
            }

            re.setPattern("\\n#\\s+(.+)\\n");
            match_i = re.globalMatch(txt);
            while(match_i.hasNext()) {
                QRegularExpressionMatch ma = match_i.next();
                if(ma.capturedTexts().size() > 1)
                    txt.replace(ma.captured(0), QString("\n\e[1;32;4m%1\e[0m\n").arg(ma.captured(1)));
            }
            re.setPattern("\\n##\\s+(.+)\\n");
            match_i = re.globalMatch(txt);
            while(match_i.hasNext()) {
                QRegularExpressionMatch ma = match_i.next();
                if(ma.capturedTexts().size() > 1)
                    txt.replace(ma.captured(0), QString("\n\e[1;32;4m%1\e[0m\n").arg(ma.captured(1)));
            }
            re.setPattern("\\n###\\s+(.+)\\n");
            match_i = re.globalMatch(txt);
            while(match_i.hasNext()) {
                QRegularExpressionMatch ma = match_i.next();
                if(ma.capturedTexts().size() > 1)
                    txt.replace(ma.captured(0), QString("\n\e[1;34;3m%1\e[0m\n").arg(ma.captured(1)));
            }

            printf("%s\n", qstoc(txt));
            f.close();
        }
        else
            perr("CmdLineOptions.usage: unable to open file %s: %s\n", "man.txt", qstoc(f.errorString()));
    }
    else if(modulenam == "formula") {
        printf("\n\n\e[1;4mFormula plugin\e[0m\n\n");
        printf("Example 1. \"formula://{test/device/1/double_scalar,test/device/2/double_scalar} function(a,c) { return a+c; }\"\n");
    }

    printf("\e[1;32;4monline doc\e[0m: see \e[0;32m%s\e[0m for more information\n", QUMBIA_READER_DOC_URL);
}

void CmdLineOptions::list_options() const
{
    foreach(QString o, m_help_map.keys()) {
        printf("--%s ", qstoc(o));
    }
    printf("\n");
}
