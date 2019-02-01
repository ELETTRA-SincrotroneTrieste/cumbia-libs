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

QString TBotMsgDecoder::source() const
{
    return m_source;
}

QString TBotMsgDecoder::text() const
{
    return m_text;
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
    else if(m_text == "/history" || m_text == "history")
        m_type = History;
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
            re.setPattern("/command(\\d{1,2})\\b");
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
                // tango attribute
                CuTangoWorld tw;
                std::vector<std::string> patterns = tw.srcPatterns();
                foreach(std::string s, patterns) {
                    re.setPattern(QString::fromStdString(s));
                    match = re.match(m_text);
                    if(match.hasMatch()) {
                        m_type = Read;
                        m_source = match.captured(0);
                        break;
                    }
                }
            } //  m_cmdLinkIdx < 0
        }
    }

    return m_type;
}

int TBotMsgDecoder::cmdLinkIdx() const
{
    return  m_cmdLinkIdx;
}
