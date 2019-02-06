#ifndef HISTORYENTRY_H
#define HISTORYENTRY_H

#include <QString>
#include <QDateTime>

class HistoryEntry {
public:
    HistoryEntry(int index, int u_id, const QDateTime& ts, const QString& nam,
                 const QString& ty, const QString& f,
                 const QString& host = QString());

    HistoryEntry(int u_id, const QString &nam,
        const QString& typ, const QString& formu = QString(),
        const QString& _host = QString());



    HistoryEntry();

    bool isValid() const;

    QString toCommand() const;

    bool hasHost() const;

    bool hasFormula() const;

    int index, user_id;
    QString name, type, formula, host;
    QDateTime datetime;

    bool is_active;
};


#endif // HISTORYENTRY_H
