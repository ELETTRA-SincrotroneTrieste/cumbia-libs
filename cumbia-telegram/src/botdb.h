#ifndef BOTDB_H
#define BOTDB_H

#include <QString>
#include <QtSql>
#include <QMap>

#include "historyentry.h"
#include "botconfig.h"

class BotDb
{
public:
    BotDb();

    ~BotDb();

    enum Quality { Undefined = -1, Ok, Warning, Alarm, Invalid };

    bool addUser(int uid, const QString& uname, const QString& firstName = QString(), const QString& lastName = QString());

    bool removeUser(int uid);

    bool addToHistory(const HistoryEntry &in);

    bool saveProc(const HistoryEntry &he);

    bool clearProcTable();

    bool unregisterProc(const HistoryEntry &he);

    QList<HistoryEntry> loadProcs();

    HistoryEntry lastOperation(int uid);

    HistoryEntry bookmarkLast(int uid);

    bool removeBookmark(int uid, int index);

    QList<HistoryEntry> history(int uid, const QString &type);

//    QList<HistoryEntry> bookmarks(int uid);

    HistoryEntry commandFromIndex(int uid, const QString &type, int index);

    bool monitorStopped(int chat_id, const QString& src);

    bool error() const;

    QString message() const;

    Quality strToQuality(const QString& qs) const;

    QString qualityToStr(Quality q) const;

    bool userExists(int uid);

    bool setHost(int user_id, int chat_id, const QString &host);

    QString getSelectedHost(int chat_id);

    bool getConfig(QMap<QString, QVariant>& datamap, QMap<QString, QString> &descmap);

private:
    QSqlDatabase m_db;
    bool m_err;
    QString m_msg;

    void createDb(const QString &tablename) ;

    void m_printTable(const QString &table);

    int m_findFirstAvailableIdx(const QList<int> &in_idxs);

};

#endif // BOTDB_H
