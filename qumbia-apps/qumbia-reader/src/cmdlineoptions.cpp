#include "cmdlineoptions.h"
#include <QRegularExpression>
#include "qumbia-reader.h" // for Verbosity enum
#include <cumacros.h>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>
#include <QTextStream>

#define QUMBIA_READER_DOC_URL "https://elettra-sincrotronetrieste.github.io/cumbia-libs/html/qumbia-reader/html/index.html"

CmdLineOptions::CmdLineOptions(bool formula_plugin_enabled, bool historical_db_plugin_enabled)
{
    m_help_map["-p x --period=x"] = "specify a custom period [x ms] for polled sources. Default: 1sec";
    m_help_map["--truncate"] = "truncate output from arrays to 12 elements";
    m_help_map["--truncate=x"] = "truncate output from arrays to x elements";
    m_help_map["--max-timers=x"] = "use at most x timers for polled sources";
    m_help_map["--l=low|medium|high|debug"] = "set output information detail level";
    m_help_map["--out-level=low|medium|high|debug"] = "same as --l";
    m_help_map["--single-shot"] = "read each source once and exit";
    m_help_map["--x"] = "read each source x times and exit (0 = --monitor)";
    m_help_map["-m | --monitor"] = "monitor source until a key is pressed";
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    m_help_map["--tango-property"] = "all sources are intended as Tango device or attribute property names."
                                    "Implies --single-shot";
    m_help_map["--tp"] = "shortcut for --tango-property";
#endif
    m_help_map["--property -p"] = "print the configuration properties of the sources (if available from the engine) and exit";
    m_help_map["--format=fmt"] = "format numbers into the specified format (e.g. %g, %.1f, %.0f)";

    if(historical_db_plugin_enabled) {
        m_help_map["--db-profile=dbprofile"] = "use the specified historical db profile. "
                                               "See hdb-db-profile-manager for more information";
        m_help_map["--db-output-file=filename"] = "write into filename the data fetched from hdb";
    }

    m_help_map["--help"] = "print this help";
#ifdef CUMBIA_RANDOM_VERSION
    m_help_map["--help-random"] = "cumbia-random module specific help";
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    m_help_map["--help-tango"] = "Tango module specific help";
#endif
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
    m_help_map["--help-epics"] = "EPICS module specific help";
#endif
    if(formula_plugin_enabled)
        m_help_map["--help-formula"] = "formula plugin specific help";
    if(historical_db_plugin_enabled)
        m_help_map["--help-hdb"] = "historical database plugin specific help";
}

RConfig CmdLineOptions::parse(const QStringList &args) const
{
    bool ok;
    RConfig o;
    QRegularExpression refreshLimitRe("\\-\\-(\\d+)");
    // fmtRe \-\-format=(%\d*\.{0,1}\d*[hxfegd])
    QRegularExpression fmtRe("\\-\\-format=(%\\d*\\.{0,1}\\d*[hxfegd])");
    foreach(QString a, args) {
        if(a.startsWith("--period=")) {
            QString t(a);
            t.remove("--period=");
            if(t.toInt(&ok) && ok)
                o.period = t.toInt();
        }
        else if(a == "--truncate")
            o.truncate = 12;
        else if(a.startsWith("--truncate=")) {
            QString t(a);
            t.remove("--truncate=");
            if(t.toInt(&ok) && ok)
                o.truncate = t.toInt();
            else
                o.truncate = 12;
        }
        else if(a.startsWith("--max-timers=")) {
            QString t(a);
            t.remove("--max-timers=");
            if(t.toInt(&ok) && ok) {
                o.max_timers = t.toInt();
            }
        }
        else if(a.startsWith("--out-level=") || a.startsWith("--l=")) {
            QString t(a);
            t.remove("--out-level=").remove("--l=");
            if(t == "medium") o.verbosity = QumbiaReader::Medium;
            else if(t == "high")  o.verbosity = QumbiaReader::High;
            else if(t == "debug")  o.verbosity = QumbiaReader::Debug;
        }
        else if(a == "--single-shot")
            o.refresh_limit = 1;
        else if(a.contains(refreshLimitRe)) {
            QRegularExpressionMatch ma = refreshLimitRe.match(a);
            if(ma.capturedTexts().size() == 2)
                o.refresh_limit = ma.captured(1).toInt();
        }
        else if(a.contains(fmtRe)) {
            QRegularExpressionMatch ma = fmtRe.match(a);
            if(ma.capturedTexts().size() == 2)
                o.format = ma.captured(1);
        }
        else if(a == "--property" || a == "-p") {
            // read "property" data type only
            o.setPropertyOnly();
        }
        else if(a == ("--monitor") || a == "-m") {
            o.refresh_limit = 0;
        }
        else if(a.startsWith("--help-")) {
            o.usage = true;
            help(args.first(), a.remove("--help-"));
        }
        else if(args.size() == 1 || a == "--help") {
            o.usage = true;
            if(a == "--help")
                help(args.first(), "");
        }
        else if(a == ("--list-options")) {
            o.list_options = true;
        }
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
        else if(a == "--tango-property" || a == "--tp")
            o.setTangoProperty();
#endif
        else if(a.startsWith("--db-profile=")) {
            o.db_profile = a.remove("--db-profile=");
        }
        else if(a.startsWith("--db-output-file=")) {
            o.db_output_file = a.remove("--db-output-file=");
        }
        else if(a.startsWith("--websocket-address=")) {
            o.ws_addr = a.remove("--websocket-address=");
        }
        else if(a.startsWith("--websocket-port="))
            o.ws_port = a.remove("--websocket-port=").toInt();
        else if(a.startsWith("--websocket-http-address="))
            o.ws_http_addr = a.remove("--websocket-http-address=");
        else if(!a.startsWith("-"))
            o.sources.append(a);
        else if(!o.list_options) { // !o.list_options: do not mess up with auto completion
            printf("\n");
            printf("\nCmdLineOptions::parse: unrecognized option \"%s\"", qstoc(a));
            printf("\n");
            o.usage = true;
        }
    }
    return o;
}

void CmdLineOptions::usage(const QString& appname) const
{
    printf("\n\nUsage: %s sources [options]\n\n", qstoc(appname));
    foreach(QString hk, m_help_map.keys()) {
        printf(" \e[1;32m%-35s\e[0m|\e[1;3m%s\e[0m\n", qstoc(hk), qstoc(m_help_map[hk]));
    }
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
    QString opt;
    foreach(QString o, m_help_map.keys()) {
        o.count("=") > 0 ? opt = o.section('=', 0, 0) + "=" : opt = o;
        printf("%s ", qstoc(opt));
    }
    printf("\n");
}
