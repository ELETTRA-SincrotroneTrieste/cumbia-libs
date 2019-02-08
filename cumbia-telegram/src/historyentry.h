#ifndef HISTORYENTRY_H
#define HISTORYENTRY_H

#include <QString>
#include <QDateTime>

class HistoryEntry {
public:
    HistoryEntry(int index, int u_id, const QDateTime& ts, const QString& nam,
                 const QString& ty, const QString& f,
                 const QString& host);

    HistoryEntry(int u_id, const QString &nam,
        const QString& typ, const QString& formu,
        const QString& _host);



    HistoryEntry();

    bool isValid() const;

    QString toCommand() const;

    bool hasHost() const;

    bool hasFormula() const;

    bool hasChatId() const;

    void fromDbProc(int u_id, int chatid, const QString& name, const QString& type,
                    const QString& formula, const QString& host,
                    const QDateTime& dt);

    int index; // general purpose index, used by BotDb::m_createHistory(int user_id)
    int user_id, chat_id;
    QString name, type, formula, host;
    QDateTime datetime;

    bool is_active;
};


#endif // HISTORYENTRY_H
