#ifndef HISTORYENTRY_H
#define HISTORYENTRY_H

#include <QString>
#include <QDateTime>

class HistoryEntry {
public:
    HistoryEntry(int index, int u_id, const QDateTime& ts,
                 const QString& cmd,
                 const QString& ty,
                 const QString& host);

    HistoryEntry(int u_id, const QString &cmd,
        const QString& typ,
        const QString& _host);



    HistoryEntry();

    bool isValid() const;

    QString toCommand() const;

    bool hasHost() const;

    bool hasChatId() const;

    void fromDbProc(int u_id, int chatid, const QString& command, const QString& type, const QString& host,
                    const QDateTime& dt);

    int index; // general purpose index, used by BotDb::m_createHistory(int user_id)
    int user_id, chat_id;
    QString command, type, host;
    QDateTime datetime;

    bool is_active;
};


#endif // HISTORYENTRY_H
