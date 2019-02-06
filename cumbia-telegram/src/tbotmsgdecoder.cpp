#include "tbotmsgdecoder.h"
#include "tbotmsg.h"

#include <QString>
#include <QRegularExpression>
#include <cutango-world.h>

TBotMsgDecoder::TBotMsgDecoder()
{
    m_type = Invalid;
    m_cmdLinkIdx = -1;
}

TBotMsgDecoder::TBotMsgDecoder(const TBotMsg& msg)
{
    m_type = decode(msg);
}

TBotMsgDecoder::Type TBotMsgDecoder::type() const
{
    return m_type;
}

QString TBotMsgDecoder::host() const
{
    return m_host;
}

QString TBotMsgDecoder::source() const
{
    return m_source;
}

QString TBotMsgDecoder::text() const
{
    return m_text;
}

QString TBotMsgDecoder::formula() const
{
    return m_formula;
}

TBotMsgDecoder::Type TBotMsgDecoder::decode(const TBotMsg &msg)
{
    m_cmdLinkIdx = -1;
    m_type = Invalid;
    m_text = msg.text;
    if(m_text == "/start") m_type = Start;
    else if(m_text == "/stop") m_type = Stop;
    else if(m_text == "/last" || m_text == "last")
        m_type = Last;
    else if(m_text == "/reads" || m_text == "reads")
        m_type = ReadHistory;
    else if(m_text == "/monitors" || m_text == "monitors")
        m_type = MonitorHistory;
    else if(m_text == "/alerts" || m_text == "alerts")
        m_type = AlertHistory;
    else if(m_text == "stop") m_type = StopMonitor; // will stop all monitors for the chat
    else if(m_text == "host" || m_text == "/host") m_type = QueryHost;
    else {
        // host
        // a. host srv-tango-srf:20000
        // b. host = srv-tango-srf:20000
        // c. host=srv-tango-srf:20000
        // \bhost(?:\s+|\s*=\s*)([A-Za-z_0-9\:\.\-]*)
        QRegularExpression re("\\bhost(?:\\s+|\\s*=\\s*)([A-Za-z_0-9\\:\\.\\-]*)");
        QRegularExpressionMatch match = re.match(m_text);
        if(match.hasMatch()) {
            m_host = match.captured(1);
            m_type = Host;
        }
        else {
            re.setPattern("/(?:read|monitor|alert)(\\d{1,2})\\b");
            match = re.match(m_text);
            if(match.hasMatch()) {
                m_cmdLinkIdx = match.captured(1).toInt();
            }
            else  { // try "/c1, /c2, ...
                re.setPattern("/c(\\d{1,2})\\b");
                match = re.match(m_text);
                if(match.hasMatch())
                    m_cmdLinkIdx = match.captured(1).toInt();
            } // try /c1, /c2 ...

            // the message received is not a link to a previous command
            // try to detect a tango attribute then

            if(m_cmdLinkIdx > 0) {
                m_type = CmdLink;
            }
            else {
                // invoke m_decodeSrcCmd helper function with the trimmed text
                m_type = m_decodeSrcCmd(m_text.trimmed());
            } //  m_cmdLinkIdx < 0
        }
    }

    return m_type;
}

TBotMsgDecoder::Type TBotMsgDecoder::m_decodeSrcCmd(const QString &txt)
{
    m_type = Invalid;
    QStringList cmd_parts = txt.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QRegularExpression re;
    QRegularExpressionMatch match;
    // 1. text only contains a tango source
    if(cmd_parts.size() == 1) {
        // must find a tango attribute pattern.
        // single Read
        m_source = m_findSource(txt);
        if(!m_source.isEmpty())
            m_type = Read;
    }
    else if(cmd_parts.size() > 1) {
        // monitor|stop or some other action on tango source
        // or single read + formula
        int formula_start_idx = 1;
        const QString &first = cmd_parts.first();
        m_source = m_findSource(first); // first param is source?
        if(m_source.isEmpty()) {
            // first parameter is a command?
            m_type = m_StrToCmdType(first);
            if(m_type != Invalid) { // yes it is
                m_source = m_findSource(cmd_parts.at(1)); // find source in second param
                formula_start_idx = 2;
            }
        }
        else {
            // first parameter is source: Read
            m_type = Read;
        }
        if(m_type != Invalid) {
            QString f; // join what remains of txt after [cmd and] source
            for(int i = formula_start_idx; i < cmd_parts.size(); i++)
                f += cmd_parts[i] + " ";
            m_formula = m_getFormula(f.trimmed());
        }
    }
    return m_type;
}

TBotMsgDecoder::Type TBotMsgDecoder::m_StrToCmdType(const QString &cmd)
{
    m_msg.clear();
    if(cmd == "monitor" || cmd == "mon")
        return  Monitor;
    else if(cmd == "alert")
        return Alert;
    if(cmd == "stop")
        return StopMonitor;
    m_msg = "TBotMsgDecoder: invalid command \"" + cmd + "\"";
    return Invalid;
}

QString TBotMsgDecoder::m_findSource(const QString &text)
{
    m_msg.clear();
    QString src;
    // admitted chars for Tango names
    const char* tname_pattern = "[A-Za-z0-9_\\-\\.]+";
    // tango attribute pattern: join tname_pattern with three '/'
    const QString tango_attr_src_pattern = QString("%1/%1/%1/%1").arg(tname_pattern);
    // allow multiple pattern search in the future (commands?)
    QStringList patterns = QStringList() << tango_attr_src_pattern;

    QRegularExpression re;
    QRegularExpressionMatch match;
    for(int i = 0; i < patterns.size() && src.isEmpty(); i++) {
        QString s = patterns[i];
        re.setPattern(s);
        match = re.match(text);
        if(match.hasMatch()) {
            src = match.captured(0);
        }
    } // for
    if(src.isEmpty())
        m_msg = "TBotMsgDecoder: \"" + text + "\" is not a valid source";
    return src;
}

QString TBotMsgDecoder::m_getFormula(const QString &f)
{
    QString validated_formula;
    // do some validation checks in the future?
    validated_formula = f;
    return validated_formula;
}


int TBotMsgDecoder::cmdLinkIdx() const
{
    return  m_cmdLinkIdx;
}

bool TBotMsgDecoder::error() const
{
    return m_type == Invalid;
}

QString TBotMsgDecoder::message() const
{
    return m_msg;
}
