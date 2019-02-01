#ifndef BOTDB_H
#define BOTDB_H

#include <QString>
#include <QtSql>

class BotDb
{
public:
    BotDb();

    ~BotDb();

    bool addUser(int uid, const QString& uname, const QString& firstName = QString(), const QString& lastName = QString());

    bool removeUser(int uid);

    bool insertOperation(int uid, const QString& name);

    bool lastOperation(int uid, QString& operation, QDateTime& datetime);

    bool history(int uid, QStringList& cmd_shortcuts, QStringList& timestamps, QStringList & cmds);

    bool commandFromIndex(int uid, int index, QDateTime& dt, QString& operation);

    bool error() const;

    QString message() const;

    bool userExists(int uid);

private:
    QSqlDatabase m_db;
    bool m_err;
    QString m_msg;

    void createDb(const QString &tablename) ;

    void m_printTable(const QString &table);

};

#endif // BOTDB_H
