#include "botdb.h"
#include <QtDebug>
#include <cumacros.h>


BotDb::BotDb()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("/tmp/tbotdb.dat");
    m_err = !m_db.open("tbotdb", "tbotdb");
    if(m_err)
        m_msg = m_db.lastError().text();
    qDebug() << __PRETTY_FUNCTION__ << "database open" << m_db.isOpen() << "tables" << m_db.tables();

    QStringList tables = QStringList() << "users" << "operations" << "hosts";

    foreach(QString t, tables) {
        if(!m_db.tables().contains(t))
            createDb(t);
        if(m_err)
            perr("BotDb: failed to create table: %s: %s", qstoc(t), qstoc(m_msg));
    }

}

BotDb::~BotDb()
{
    predtmp("~BotDb %p: closing db...", this);
    if(m_db.isOpen())
        m_db.close();
}

bool BotDb::addUser(int uid, const QString &uname, const QString &firstName, const QString &lastName)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("INSERT INTO users VALUES(%1,'%2','%3','%4', datetime())").arg(uid).arg(uname).arg(firstName).arg(lastName));
    if(m_err)
        m_msg = q.lastError().text();
    return !m_err;
}

bool BotDb::removeUser(int uid)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();

    return !m_err;
}

bool BotDb::insertOperation(int uid, const QString &name)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();

    qDebug() << __PRETTY_FUNCTION__ << "enter";
    QSqlQuery q(m_db);
    // keep 10 operations buffer for each user
    m_err = !q.exec(QString("SELECT timestamp FROM operations WHERE user_id=%1 ORDER BY timestamp DESC").arg(uid));
    if(m_err) {
        m_msg = q.lastError().text();
    }
    else {
        int i = 0;
        while(i < 9 && q.next()) {
            qDebug() << __PRETTY_FUNCTION__ << "uid" << uid << "op" << name << "datetime " << i <<
                        q.value(0).toDateTime().toString(Qt::ISODate);
            i++;
        }
        if(i == 9 && q.next()) {
            qDebug() << __PRETTY_FUNCTION__ << "deleting old operation " << q.value(0).toDateTime().toString(Qt::ISODate);
            m_err = !q.exec(QString("DELETE FROM operations WHERE user_id=%1 AND timestamp='%2'")
                            .arg(uid).arg(q.value(0).toDateTime().toString(Qt::ISODate)));
            if(m_err) {
                m_msg = q.lastError().text();
                perr("BotDb.insertOperation: error executing %s: %s", qstoc(q.lastQuery()), qstoc(m_msg));
            }
        }
        // operation is defined with UNIQUE (user_id, name) ON CONFLICT REPLACE
        // so we do not have duplicate operations for the same user_id. the timestamp
        // is updated

        printf("\e[1;33moperations table before insert:\e[0m\n");
        m_printTable("operations");


        m_err = !q.exec(QString("INSERT INTO operations VALUES(%1, datetime(), '%2')").arg(uid).arg(name));
        if(m_err)
            m_msg = q.lastError().text();

        printf("\e[1;35moperations table after insert query (%s):\e[0m\n", qstoc(q.lastQuery()));
        m_printTable("operations");

        qDebug() << __PRETTY_FUNCTION__ << "executed" << q.lastQuery() << "success " << !m_err << "msg" << m_msg;
    }
    return !m_err;
}

bool BotDb::lastOperation(int uid, QString& operation, QDateTime &datetime)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT MAX(timestamp),name FROM operations WHERE user_id=%1").arg(uid));
    if(m_err)
        m_msg = q.lastError().text();
    else {
        m_err = !q.next();
        if(!m_err) {
            operation = q.value(1).toString();
            datetime = q.value(0).toDateTime();
        }
    }
    return !m_err;
}

bool BotDb::history(int uid, QStringList &cmd_shortcuts, QStringList &timestamps, QStringList &cmds)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT timestamp,name FROM operations WHERE user_id=%1 ORDER BY timestamp ASC").arg(uid));
    int i = 1;
    while(q.next()) {
        cmd_shortcuts << QString("/command%1").arg(i);
        timestamps << q.value(0).toDateTime().toString();
        cmds << q.value(1).toString();
        i++;
    }
    return !m_err;
}

bool BotDb::commandFromIndex(int uid, int index, QDateTime &dt, QString &operation)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT timestamp,name FROM operations WHERE user_id=%1 ORDER BY timestamp ASC").arg(uid));
    if(!m_err) {
        int i = 0;
        while(q.next() && ++i < index) { // q.next() first!
        }
        if(i == index) {
            dt = q.value(0).toDateTime();
            operation = q.value(1).toString();
        }
    }
    else {
        m_msg = q.lastError().text();
    }
    return !m_err;
}

bool BotDb::error() const
{
    return m_err;
}

QString BotDb::message() const
{
    return m_msg;
}

bool BotDb::userExists(int uid)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();

    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT id FROM users WHERE id=%1").arg(uid));
    if(m_err)
        m_msg = q.lastError().text();

    qDebug() << "query size is " << q.next() << "err" << m_err << m_msg << "user id is " << uid;
    return !m_err && q.next();
}

bool BotDb::setHost(int userid, const QString& host) {
    if(!m_db.isOpen())
        return false;
    m_msg.clear();

    QSqlQuery q(m_db);

}

void BotDb::createDb(const QString& tablename)
{
    if(m_db.isOpen()) {
        printf("- BotDb.createDb: creating tables....\n");
        QSqlQuery q(m_db);
        m_err= false;
        if(tablename == "users") {
            m_err = !q.exec("CREATE TABLE users (id INT PRIMARY_KEY NOT NULL, uname TEXT NOT NULL, first_name TEXT, last_name TEXT, "
                            "join_date DATETIME NOT NULL)");
            if(!m_err)
                printf("- BotDb.createDb: successfully created table \"users\"\n");
        }
        else if(tablename == "hosts") {
            m_err = !q.exec("CREATE TABLE hosts (user_id INT PRIMARY_KEY NOT NULL, host TEXT NOT NULL,"
                            "UNIQUE (user_id) ON CONFLICT REPLACE)");
            if(!m_err)
                printf("- BotDb.createDb: successfully created table \"hosts\"\n");
        }
        else if(tablename == "operations") {
            // unique user_id,operation. timestamp only is updated on repeated operations
            m_err = !q.exec("CREATE TABLE operations (user_id INTEGER NOT NULL, "
                            "timestamp DATETIME NOT NULL, "
                            "name TEXT NOT NULL,"
                            "UNIQUE (user_id, name) ON CONFLICT REPLACE)");
            if(!m_err)
                printf("- BotDb.createDb: successfully created table \"operations\"\n");
        }

        if(m_err)
            m_msg = q.lastError().text();
    }
    else {
        m_msg = "BotDb.createDb: database not open";
        perr("%s", qstoc(m_msg));
    }
}

void BotDb::m_printTable(const QString &table)
{
    if(!m_db.isOpen())
        perr("BotDb.m_printTable: Database is not open");
    else {
        QSqlQuery q(m_db);
        m_err = !q.exec(QString("SELECT * FROM %1").arg(table));
        if(!m_err) {
            printf("\e[1;32m%s\e[0m\n", qstoc(table));
            while(q.next()) {
                QSqlRecord rec = q.record();
                for(int i = 0; i < rec.count(); i++) {
                    printf("%s\t\t", qstoc(q.value(i).toString()));
                }
                printf("\n");
            }
        }
    }
}
