#ifndef TBOTMSGDECODER_H
#define TBOTMSGDECODER_H

class TBotMsg;

#include <QStringList>

class TBotMsgDecoder
{
public:


    enum Type { Invalid, Start, Stop, Host, QueryHost,
                Read, Monitor, Alert, StopMonitor, Properties,
                ReadHistory, MonitorHistory, AlertHistory,
                Bookmarks, AddBookmark, DelBookmark,
                Last, CmdLink, Search, AttSearch, ReadFromAttList,
                Plot,
                // please include HelpXXX entries within Help and HelpSearch
                Help, HelpMonitor, HelpAlerts, HelpHost, HelpSearch,
                MaxType = 32 };

    const char types[MaxType][32] = { "Invalid", "Start", "Stop", "Host", "QueryHost",
                                      "Read", "Monitor", "Alert", "StopMonitor", "Properties",
                                      "ReadHistory", "MonitorHistory", "AlertHistory",
                                      "Bookmarks", "AddBookmark", "DelBookmark",
                                      "Last", "CmdLink", "Search", "AttSearch", "ReadFromAttList",
                                      "Plot",
                                      "Help", "HelpMonitor", "HelpAlerts", "HelpHost", "HelpSearch",
                                      "MaxType" };

    TBotMsgDecoder();

    TBotMsgDecoder(const TBotMsg &msg, const QString& normalizedFormulaPattern);

    Type type() const;

    QString host() const;

    QString source() const;

    QString text() const;

    Type decode(const TBotMsg &msg);

    int cmdLinkIdx() const;

    bool error() const;

    QString message() const;

    QString toHistoryTableType( Type t) const;

    QStringList getArgs() const;

    QStringList detectedSources() const;

private:

    bool m_tryDecodeFormula(const QString& text);

    Type m_decodeSrcCmd(const QString& text);

    Type m_StrToCmdType(const QString& cmd);

    QString m_findSource(const QString& text);

    QString m_findDevice(const QString &text);

    QString m_findByPatterns(const QString& text, const QStringList &patterns);

    QString m_getFormula(const QString& f);

    Type m_type;

    QString m_host;
    QString m_source;
    QString m_text;

    int m_cmdLinkIdx;

    QString m_msg;

    QString m_normalizedFormulaPattern;

    QStringList m_detectedSources;
};

#endif // TBOTMSGDECODER_H
