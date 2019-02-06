#ifndef BOTDB_H
#define BOTDB_H

#include <QString>
#include <QtSql>
#include <QMap>

#include "historyentry.h"

class BotDb
{
public:
    BotDb();

    ~BotDb();

    enum Quality { Undefined = -1, Ok, Warning, Alarm, Invalid };

    bool addUser(int uid, const QString& uname, const QString& firstName = QString(), const QString& lastName = QString());

    bool removeUser(int uid);

    bool insertOperation(const HistoryEntry &in);

    HistoryEntry lastOperation(int uid);

    QList<HistoryEntry> history(int uid);

    HistoryEntry commandFromIndex(int uid, const QString &type, int index);

    bool monitorStopped(int chat_id, const QString& src);

    bool readUpdate(int chat_id, const QString& src, const QString& value, const QString &quality);

    bool error() const;

    QString message() const;

    Quality strToQuality(const QString& qs) const;

    QString qualityToStr(Quality q) const;

    bool userExists(int uid);

    bool setHost(int user_id, int chat_id, const QString &host);

    QString getSelectedHost(int chat_id);

private:
    QSqlDatabase m_db;
    bool m_err;
    QString m_msg;
    QMap<int, QList<HistoryEntry> > m_history;

    void createDb(const QString &tablename) ;

    void m_printTable(const QString &table);

    bool m_createHistory(int user_id);

};

#endif // BOTDB_H
