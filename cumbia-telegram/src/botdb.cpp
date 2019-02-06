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

    QStringList tables = QStringList() << "users" << "operations" << "hosts" << "host_selection"
                                       << "procs";

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

bool BotDb::insertOperation(const HistoryEntry &in)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();

    int uid = in.user_id;
    const QString &name = in.name;
    const QString& type = in.type;
    const QString& formula = in.formula;
    const QString& host = in.host;

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


        m_err = !q.exec(QString("INSERT INTO operations VALUES(%1, datetime(), '%2', '%3', '%4', '%5')").
                        arg(uid).arg(name).arg(type).arg(formula).arg(host));
        if(m_err)
            m_msg = q.lastError().text();

        printf("\e[1;35moperations table after insert query (%s):\e[0m\n", qstoc(q.lastQuery()));
        m_printTable("operations");

        qDebug() << __PRETTY_FUNCTION__ << "executed" << q.lastQuery() << "success " << !m_err << "msg" << m_msg;
    }
    return !m_err;
}

HistoryEntry BotDb::lastOperation(int uid)
{
    HistoryEntry he;
    if(!m_db.isOpen())
        return he;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT MAX(timestamp),name,type,formula,host FROM operations WHERE user_id=%1").arg(uid));
    if(m_err)
        m_msg = q.lastError().text();
    else {
        if(q.next()) {
            he.user_id = uid;
            he.datetime = q.value(0).toDateTime();
            he.name = q.value(1).toString();
            he.type = q.value(2).toString();
            he.formula = q.value(3).toString();
            he.host = q.value(4).toString();
        }
    }
    return he;
}

QList<HistoryEntry> BotDb::history(int uid)
{
    m_createHistory(uid);
    return m_history[uid];
}

bool BotDb::m_createHistory(int user_id) {
    if(!m_db.isOpen())
        return false;
    m_err = false;
    m_msg.clear();
    m_history[user_id].clear();
    QSqlQuery q(m_db);
    QStringList types = QStringList() << "read" << "alert" << "monitor";
    for(int i = 0; i < types.size() && !m_err; i++) {
        const QString& t = types[i];
        m_err = !q.exec(QString("SELECT user_id,timestamp,name,type,formula,host FROM operations WHERE user_id=%1 AND type='%2' "
                                "ORDER BY timestamp ASC").arg(user_id).arg(t));
        int idx = 1;
        while(q.next()) {
            HistoryEntry he(idx, q.value(0).toInt(), q.value(1).toDateTime(), q.value(2).toString(),
                            q.value(3).toString(), q.value(4).toString(), q.value(5).toString());
            m_history[user_id] << he;
            idx++;
        }
        if(m_err)
            break;
    }
    return !m_err;
}

HistoryEntry BotDb::commandFromIndex(int uid, const QString& type, int index)
{
    m_msg.clear();
    m_err = false;
    if(m_history[uid].isEmpty())
        m_err = !m_createHistory(uid);
    const QList<HistoryEntry> hes = m_history[uid];
    for(int i = 0; i < hes.size(); i++) {
        QString typ(type);
        if(hes[i].index == index && typ.remove('/').startsWith(hes[i].type)) {
            return hes[i];
        }
    }
    if(hes.size() == 0)
        m_msg = "BotDb: history empty";
    else if(index > hes.size())
        m_msg = "BotDb: index out of range";
    return HistoryEntry();
}

bool BotDb::monitorStopped(int chat_id, const QString &src)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("DELETE from read_history WHERE chat_id=%1 AND src='%2'").arg(chat_id).arg(src));
    if(m_err)
        m_msg = "BotDb::monitorStopped: error cleaning read_history for " + QString::number(chat_id) + ": "
                + src + ": " + q.lastError().text();
    return !m_err;
}

bool BotDb::readUpdate(int chat_id, const QString &src, const QString &value, const QString& quality)
{
    // QTime t;
    // t.start();
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    // chat_id timestamp src value  quality
    m_err = !q.exec(QString("INSERT INTO read_history VALUES(%1, datetime(), '%2', '%3', %4)").arg(chat_id)
                    .arg(src).arg(value).arg(static_cast<int>(strToQuality(quality))));
    if(m_err)
        m_msg = "BotDb::readUpdate: error updating read_history for " + QString::number(chat_id) + ": "
                + src + ": " + q.lastError().text();
    //qDebug() << __PRETTY_FUNCTION__;
    //m_printTable("read_history");
    //qDebug() << __PRETTY_FUNCTION__ << "took" << t.elapsed() << "ms";
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

BotDb::Quality BotDb::strToQuality(const QString &qs) const
{
    if(qs.contains("alarm", Qt::CaseInsensitive) || qs.contains("error", Qt::CaseInsensitive))
        return Alarm;
    if(qs.contains("warning", Qt::CaseInsensitive))
        return Warning;
    if(qs.contains("invalid", Qt::CaseInsensitive))
        return Invalid;
    return Ok;
}

QString BotDb::qualityToStr(BotDb::Quality q) const
{
    switch (q) {
    case Alarm:
        return "alarm";
    case Warning:
        return "warning";
    case Invalid:
        return "invalid";
    case Ok:
        return "ok";
    default:
        return "undefined";
    }
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
    return !m_err && q.next();
}

bool BotDb::setHost(int user_id, int chat_id, const QString& host) {
    if(!m_db.isOpen())
        return false;
    m_msg.clear();

    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT id FROM hosts WHERE name='%1'").arg(host));
    if(!m_err) {
        int host_id = -1;
        if(q.next())
            host_id = q.value(0).toInt();
        // is user allowed to set host  database
        bool allowed = user_id > 0;
        if(host_id > 0 && allowed) {
            m_err = !q.exec(QString("INSERT INTO host_selection VALUES(%1, %2, datetime())").arg(host_id).arg(chat_id));
            if(m_err)
                m_msg = "BotDb.setHost: failed to select host \"" + host + "\": " + q.lastError().text();
        }
        else {
            m_err = true;
            m_msg = "BotDb.setHost: user is not allowed to set host to \"" + host + "\"";
        }
    }
    return !m_err;
}

QString BotDb::getSelectedHost(int chat_id)
{
    QString host;
    if(!m_db.isOpen())
        return host; // empty
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT name FROM hosts,host_selection where chat_id=%1 AND host_id=hosts.id").arg(chat_id));
    if(!m_err) {
        if(q.next())
            host = q.value(0).toString();
        // having an empty result is not an error. The application will pick up TANGO_HOST from env
    }
    else
        m_msg = "BotDb.setHost: failed to select host \"" + host + "\": " + q.lastError().text();

    return host;
}

void BotDb::createDb(const QString& tablename)
{
    if(m_db.isOpen()) {
        printf("- BotDb.createDb: creating tables....\n");
        QSqlQuery q(m_db);
        m_err= false;
        if(tablename == "users") {
            m_err = !q.exec("CREATE TABLE users (id INTEGER PRIMARY_KEY NOT NULL, uname TEXT NOT NULL, first_name TEXT, last_name TEXT, "
                            "join_date DATETIME NOT NULL)");
        }
        else if(tablename == "hosts") {
            m_err = !q.exec("CREATE TABLE hosts (id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL, description TEXT, created DATETIME NOT NULL)");
        }
        else if(tablename == "host_selection") {
            m_err = !q.exec(" CREATE TABLE host_selection (host_id INTEGER NOT NULL,chat_id INTEGER NOT NULL,"
                            " timestamp DATETIME NOT NULL, UNIQUE(chat_id) ON CONFLICT REPLACE)");
        }
        else if(tablename == "operations") {
            // unique user_id,operation. timestamp only is updated on repeated operations
            m_err = !q.exec("CREATE TABLE operations (user_id INTEGER NOT NULL, "
                            "timestamp DATETIME NOT NULL, "
                            "name TEXT NOT NULL,"
                            "type TEXT NOT NULL,"
                            "formula TEXT NOT NULL,"
                            "host TEXT DEFAULT NULL,"
                            "UNIQUE (user_id, name, type) ON CONFLICT REPLACE)");
        }
        else if(tablename == "read_history") {
            // unique chat_id,src. timestamp,value, quality are updated throughout readings
            m_err = !q.exec("CREATE TABLE read_history (chat_id INTEGER NOT NULL, "
                            "timestamp DATETIME NOT NULL, "
                            "src TEXT NOT NULL,"
                            "value TEXT,"
                            "quality INTEGER NOT NULL,"
                            "UNIQUE (chat_id, src) ON CONFLICT REPLACE)");
        }

        if(m_err)
            m_msg = q.lastError().text();
        else
            printf("- BotDb.createDb: successfully created table \"%s\"\n", qstoc(tablename));
    }
    else {
        m_msg = "BotDb.createDb: database not open";
    }
    if(m_err)
        perr("BotDb.createDb: failed to create table %s: %s", qstoc(tablename), qstoc(m_msg));
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
