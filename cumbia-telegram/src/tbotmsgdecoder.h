#ifndef TBOTMSGDECODER_H
#define TBOTMSGDECODER_H

class TBotMsg;

#include <QString>

class TBotMsgDecoder
{
public:


    enum Type { Invalid, Start, Stop, Host, QueryHost,
                Read, Monitor, Alert, StopMonitor, Properties,
                ReadHistory, MonitorHistory, AlertHistory,
                Bookmarks, AddBookmark, DelBookmark,
                Last, CmdLink, Search, AttSearch, ReadFromAttList,
                MaxType = 32 };

    const char types[MaxType][32] = { "Invalid", "Start", "Stop", "Host", "QueryHost",
                                      "Read", "Monitor", "Alert", "StopMonitor", "Properties",
                                      "ReadHistory", "MonitorHistory", "AlertHistory",
                                      "Bookmarks", "AddBookmark", "DelBookmark",
                                      "Last", "CmdLink", "Search", "AttSearch", "ReadFromAttList",
                                      "MaxType" };

    TBotMsgDecoder();

    TBotMsgDecoder(const TBotMsg &msg);

    Type type() const;

    QString host() const;

    QString source() const;

    QString text() const;

    QString formula() const;

    Type decode(const TBotMsg &msg);

    int cmdLinkIdx() const;

    bool error() const;

    QString message() const;

    QString toHistoryTableType( Type t) const;

private:

    Type m_decodeSrcCmd(const QString& text);

    Type m_StrToCmdType(const QString& cmd);

    QString m_findSource(const QString& text);

    QString m_findDevice(const QString &text);

    QString m_findByPatterns(const QString& text, const QStringList &patterns);

    QString m_getFormula(const QString& f);

    Type m_type;

    QString m_host;
    QString m_source;
    QString m_formula;
    QString m_text;

    int m_cmdLinkIdx;

    QString m_msg;
};

#endif // TBOTMSGDECODER_H
