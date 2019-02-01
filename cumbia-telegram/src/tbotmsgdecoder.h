#ifndef TBOTMSGDECODER_H
#define TBOTMSGDECODER_H

class TBotMsg;

#include <QString>

class TBotMsgDecoder
{
public:


    enum Type { Invalid = 0, Start, Stop, Host, Read, Monitor, Properties, History, Last, CmdLink, MaxType = 32 };

    const char types[MaxType][32] = { "Invalid", "Start", "Stop", "Host", "Read",
                                      "Monitor", "Properties", "History", "Last", "CmdLink",
                                      "MaxType"};

    TBotMsgDecoder();

    TBotMsgDecoder(const TBotMsg &msg);

    Type type() const;

    QString host() const;

    QString source() const;

    QString text() const;

    Type decode(const TBotMsg &msg);

    int cmdLinkIdx() const;

private:
    Type m_type;

    QString m_host;

    QString m_source;

    QString m_text;

    int m_cmdLinkIdx;
};

#endif // TBOTMSGDECODER_H
