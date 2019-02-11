#include "botdb.h"
#include <QtDebug>
#include <QSqlRecord>
#include <cumacros.h>

/**
 * @brief BotDb::BotDb class constructor.
 *
 * Creates database tables if they do not exist.
 *
 * \section Tables
 *
 * \par proc table
 *
 * procs table saves active monitors/aletrs in order to be restored if the server goes down and up again later.
 * Must contain all the fields necessary to restart a monitor / alert exactly as it was before:
 * - user and chat id
 * - date time started
 * - command (monitor|alert|...) plus source plus formula
 * - host
 *
 * Inserts are made by registerProc *only if a service has been successfully started*
 * CuBotServer must deal with this, and registerProc is called from CuBotServer::onSrcMonitorStarted
 *
 * \par users table
 * Stores information about registered users
 *
 * \par history table
 * Stores the per user history of reads/monitors/alerts and other commands.
 * History is a circular buffer, older entries are deleted.
 * history table entries where bookmark field is true are not deleted.
 * When a bookmark is removed, bookmark field is set to false and that entry
 * is treated as a normal history entry, so it will be deleted if old.
 *
 * \par hosts table
 * Stores the name, the description and the creation date of the configured hosts (i.e. TANGO_HOSTs)
 *
 * \par host_selection
 * Records the currently selected host for each registered user
 *
 */
BotDb::BotDb()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("/tmp/tbotdb.dat");
    m_err = !m_db.open("tbotdb", "tbotdb");
    if(m_err)
        m_msg = m_db.lastError().text();
    qDebug() << __PRETTY_FUNCTION__ << "database open" << m_db.isOpen() << "tables" << m_db.tables();

    QStringList tables = QStringList() << "users" << "history" << "hosts" << "host_selection"
                                       << "proc" << "config" << "bookmarks";

    foreach(QString t, tables) {
        if(!m_db.tables().contains(t))
            createDb(t);
        if(m_err)
            perr("BotDb: failed to create table: %s: %s", qstoc(t), qstoc(m_msg));
    }

}

BotDb::~BotDb()
{
    printf("\e[1;31m~BotDb %p: closing db...\e[0m", this);
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
/**
 * @brief BotDb::registerProc registers a new monitor/alert or any other service that
 *        needs to be restored if CuBotServer goes down and then up again.
 *
 * @param he HistoryEntry filled by the CuBotServer when a new service has been successfully started.
 *        The field chat_id *must be > -1*
 *
 * @return true if successful, false otherwise
 */
bool BotDb::saveProc(const HistoryEntry &he)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    // (user_id INTEGER NOT NULL, chat_id INTEGER NOT NULL,
    // start_dt DATETIME NOT NULL, name TEXT NOT NULL,
    // formula TEXT, host TEXT NOT NULL)
    m_err = !q.exec(QString("INSERT INTO proc VALUES(%1, %2, '%3', '%4', '%5', '%6', '%7')").
                    arg(he.user_id).arg(he.chat_id).arg(he.datetime.toString("yyyy-MM-dd hh:mm:ss")).
                    arg(he.name).arg(he.type).arg(he.formula).arg(he.host));
    printf("\e[1;32m---> registered process: \"%s\"\e[0m SUCCESS: %d\n", qstoc(q.lastQuery()), !m_err);
    if(m_err)
        m_msg = "BotDb.registerProc: failed to register process " + he.toCommand() + ": " + q.lastError().text();
    return !m_err;
}

bool BotDb::clearProcTable()
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec("DELETE FROM proc");
    if(m_err)
        m_msg = "BotDb.clearProcTable: failed to clear proc table: " + q.lastError().text();
    return !m_err;
}

/**
 * @brief BotDb::unregisterProc called from the CuBotServer when a process (monitor or alert) stops
 *
 * @param he a partially filled HistoryEntry containing only the fields necessary to individuate and
 *        delete a process from the proc table
 *        History entry contains
 *        - user_id
 *        - src
 *        - host
 *        Formula is not necessary because there can be only one monitor with that source at one time,
 *        unregarding the formula
 *
 * @return true if the database proc table has been successfully updated, false otherwise
 */
bool BotDb::unregisterProc(const HistoryEntry &he)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    // (user_id INTEGER NOT NULL, chat_id INTEGER NOT NULL,
    // start_dt DATETIME NOT NULL, name TEXT NOT NULL, host TEXT NOT NULL)
    m_err = !q.exec(QString("DELETE FROM proc WHERE user_id=%1 AND name='%2'").
                    arg(he.user_id).arg(he.name));
    printf("\e[0;35m<-- unregistered process: \"%s\"\e[0m SUCCESS: %d\n", qstoc(q.lastQuery()), !m_err);

    if(m_err)
        m_msg = "BotDb.registerProc: failed to register process " + he.toCommand() + ": " + q.lastError().text();
    return !m_err;
}

QList<HistoryEntry> BotDb::loadProcs()
{
    QList<HistoryEntry> hel;
    if(!m_db.isOpen())
        return hel;
    m_msg.clear();
    QSqlQuery q(m_db);
    // (user_id INTEGER NOT NULL, chat_id INTEGER NOT NULL,
    // start_dt DATETIME NOT NULL, name TEXT NOT NULL,
    // formula TEXT, host TEXT NOT NULL)
    m_err = !q.exec(QString("SELECT user_id,name,type,formula,host,chat_id,start_dt FROM proc"));
    if(m_err)
        m_msg = "BotDb::loadProcs: database error: " + q.lastError().text();
    else {
        while(q.next()) {
            HistoryEntry he;
            QSqlRecord r = q.record();
            // fromDbProc(int u_id, int chatid,
            // const QString& name, const QString& type,
            // const QString& formula, const QString& host,
            // const QDateTime& dt)
            he.fromDbProc(r.value("user_id").toInt(), r.value("chat_id").toInt(),
                          r.value("name").toString(),  r.value("type").toString(),
                          r.value("formula").toString(), r.value("host").toString(),
                          r.value("start_dt").toDateTime());
            hel << he;
        }
    }
    return hel;
}

bool BotDb::addToHistory(const HistoryEntry &in)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();

    int uid = in.user_id;
    const QString &name = in.name;
    const QString& type = in.type;
    const QString& formula = in.formula;
    const QString& host = in.host;
    int history_len = 8; // pick from db!
    QList<int> h_idxs;

    qDebug() << __PRETTY_FUNCTION__ << "enter";
    QSqlQuery q(m_db);
    // first try to see if there is a matching row into the database for the new entry
    // if yes, only the timestamp field needs update
    m_err = !q.exec(QString("SELECT timestamp,h_idx FROM history WHERE "
                            "user_id=%1 AND name='%2' AND type='%3' AND formula='%4' "
                            "AND host='%5'").arg(uid).arg(name)
                    .arg(type).arg(formula).arg(host));

    bool update_timestamp_only = q.next() && !m_err;


    printf("\e[1;33mexecuted %s to see if found entry.. found? %d\e[0m\n", qstoc(q.lastQuery()), update_timestamp_only);

    if(update_timestamp_only) {
        int b_idx = q.value(1).toInt(); // PRIMARY KEY(user_id,type,h_idx)
        m_err = !q.exec(QString("UPDATE history SET timestamp=datetime() "
                               "where user_id=%1 AND type='%2' AND h_idx=%3;")
                       .arg(uid).arg(type).arg(b_idx));
    }

    if(!m_err && !update_timestamp_only) {
        QSqlQuery bookmarks_q(m_db);
        // keep history_len history buffer for each user

        //                                 0          1           2
        m_err = !q.exec(QString("SELECT timestamp,h_idx,_rowid_ FROM history WHERE "
                                " user_id=%1 "
                                " ORDER BY timestamp DESC").arg(uid));
        if(!m_err)
            m_err = !bookmarks_q.exec("SELECT history_rowid FROM bookmarks");

        if(m_err) {
            m_msg =  "BotDb.addToHistory: " + q.lastError().text();
        }
        else {
            int i = 0;
            QList<int> bookmarks_idxs; // list of history._rowid_ that are bookmarked
            while(bookmarks_q.next())
                bookmarks_idxs << bookmarks_q.value(0).toInt();

            while(q.next()) {
                int history_rowid = q.value(2).toInt();
                h_idxs  << q.value(1).toInt();

                qDebug() << __PRETTY_FUNCTION__ << "uid" << uid << "op" << name << "datetime " << i <<
                            q.value(0).toDateTime().toString(Qt::ISODate);

                bool is_bookmark = bookmarks_idxs.contains(history_rowid);

                if(!is_bookmark)
                    i++;

                if(i >= history_len && !is_bookmark) {
                    QSqlQuery delq(m_db);
                    m_err = !delq.exec(QString("DELETE FROM history WHERE user_id=%1 AND timestamp='%2'")
                                       .arg(uid).arg(q.value(0).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
                    if(m_err) {
                        m_msg = "BotDb.addToHistory: " +  delq.lastError().text();
                        perr("BotDb.insertOperation: error executing %s: %s", qstoc(delq.lastQuery()), qstoc(m_msg));
                    }
                    else {
                        printf("\e[1;35mBotDb.insertOperation: removed old history entry from %s (%s)\e[0m\n",
                               qstoc(q.value(0).toDateTime().toString()), qstoc(delq.lastQuery()));
                    }
                }
            }
            printf("\e[1;33mhistory table before insert:\e[0m\n");
            m_printTable("history");

            // calculate first per history index available
            int h_idx = m_findFirstAvailableIdx(h_idxs);

            m_err = !q.exec(QString("INSERT INTO history VALUES(%1, datetime(), '%2', '%3', '%4', '%5', %6)").
                            arg(uid).arg(name).arg(type).arg(formula).arg(host).arg(h_idx));
            if(m_err)
                m_msg =  "BotDb.addToHistory: " + q.lastError().text();

            printf("\e[1;35mhistory table after insert query (%s):\e[0m\n", qstoc(q.lastQuery()));
            m_printTable("history");

            qDebug() << __PRETTY_FUNCTION__ << "executed" << q.lastQuery() << "success " << !m_err << "msg" << m_msg;
        }
    }

    if(m_err) {
        perr("BotDb.addToHistory: database error: query %s %s",  qstoc(q.lastQuery()), qstoc(m_msg));
    }

    return !m_err;
}

QList<HistoryEntry> BotDb::history(int user_id, const QString& type) {
    QList<HistoryEntry> hel;
    if(!m_db.isOpen())
        return hel;
    m_err = false;
    m_msg.clear();
    QSqlQuery q(m_db);
    QString t = type;
    if(type != "bookmarks") {
        m_err = !q.exec(QString("SELECT user_id,timestamp,name,type,formula,host,h_idx,_rowid_"
                                " FROM history WHERE user_id=%1 AND type='%2' "
                                " ORDER BY timestamp ASC").arg(user_id).arg(t));
    }
    else { // query with no AND type='%s'
        m_err = !q.exec(QString("SELECT user_id,history.timestamp,name,type,formula,host,h_idx,history.rowid"
                                " FROM history,bookmarks WHERE user_id=%1 "
                                " AND bookmarks.history_rowid=history.rowid "
                                " ORDER BY history.timestamp ASC").arg(user_id));
    }
    printf("executed %s\n", qstoc(q.lastQuery()));
    while(!m_err && q.next()) {
        HistoryEntry he(q.value(6).toInt(), q.value(0).toInt(), q.value(1).toDateTime(), q.value(2).toString(),
                        q.value(3).toString(), q.value(4).toString(), q.value(5).toString());
        hel << he;
    }
    if(m_err) {
        m_msg = "BotDb.history: " + q.lastError().text();
        perr("BotDb.history: database error: %s", qstoc(m_msg));
    }
    return hel;
}

//QList<HistoryEntry> BotDb::bookmarks(int uid)
//{
//    QList<HistoryEntry> hel;
//    if(!m_db.isOpen())
//        return hel;
//    m_err = false;
//    m_msg.clear();
//    QSqlQuery q(m_db);
//    m_err = !q.exec(QString("SELECT user_id,timestamp,name,type,formula,host,h_idx"
//                            " FROM history WHERE user_id=%1 AND bookmark_idx>0 "
//                            " ORDER BY timestamp ASC").arg(uid));
//    while(!m_err && q.next()) {
//        HistoryEntry he(q.value(6).toInt(), q.value(0).toInt(), q.value(1).toDateTime(), q.value(2).toString(),
//                        q.value(3).toString(), q.value(4).toString(), q.value(5).toString());
//        hel << he;
//    }
//    if(m_err)
//        m_msg = "BotDb.history: " + q.lastError().text();
//    return hel;
//}

HistoryEntry BotDb::lastOperation(int uid)
{
    HistoryEntry he;
    if(!m_db.isOpen())
        return he;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT MAX(timestamp),name,type,formula,host FROM history WHERE user_id=%1").arg(uid));
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

HistoryEntry BotDb::bookmarkLast(int uid)
{
    HistoryEntry he;
    if(!m_db.isOpen())
        return he;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("SELECT MAX(timestamp),name,type,formula,host,rowid FROM history WHERE user_id=%1").arg(uid));
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
            int rowid = q.value(5).toInt();
            // mark last entry as bookmark!
            // OK, we found most recent entry in table
            // find available per user bookmark index
            QList<int> bkm_indexes;
            QSqlQuery bookmarksQ(m_db);
            m_err = !q.exec(QString("INSERT INTO bookmarks VALUES(%1,datetime())").arg(rowid));
        }
    }
    if(m_err)
        m_msg = "BotDb.bookmarkLast: database error: " + q.lastError().text();
    return he;
}

bool BotDb::removeBookmark(int uid, int index)
{
    m_msg.clear();
    m_err = false;
    QSqlQuery q(m_db);
    m_err = !q.exec(QString("DELETE FROM bookmarks WHERE history_rowid=(SELECT history.rowid FROM history "
                            "WHERE user_id=%1 AND h_idx=%2)").arg(uid).arg(index));
    if(m_err) {
        m_msg = "BotDb.removeBookmark: query  " + q.lastQuery() + "error: " + q.lastError().text();
        perr(qstoc(m_msg));
    }
    return !m_err;
}

HistoryEntry BotDb::commandFromIndex(int uid, const QString& type, int index)
{
    m_msg.clear();
    m_err = false;
    QString typ(type);
    typ.remove('/').remove(QString::number(index));
    const QList<HistoryEntry> hes = history(uid, typ);
    for(int i = 0; i < hes.size(); i++) {
        if(hes[i].index == index && typ.startsWith(hes[i].type)) {
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

/**
 * @brief BotDb::getConfig fills in two maps that are passed by reference by BotConfig
 *
 * \par Note
 * The function does not clear the input maps before filling them in.
 * This allows BotConfig to prepare a map with defaults that will remain untouched if the
 * corresponding keys are not configured into the database (e.g. ttl)
 *
 */
bool BotDb::getConfig(QMap<QString, QVariant> &datamap, QMap<QString, QString> &descmap)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    m_err = !q.exec("SELECT * FROM config");
    if(m_err)
        m_msg = "BotDb.getConfig: error in query " + q.lastError().text();
    else {
        QMap<QString, QVariant> map;
        while(!m_err && q.next()) {
            QSqlRecord rec = q.record();
            m_err = rec.count() != 3;
            if(!m_err) {
                datamap[rec.value(0).toString()] = rec.value(1);
                descmap[rec.value(0).toString()] = rec.value(2).toString();
            }
            else {
                m_msg = "BotDb.getConfig: unexpected number of columns in table config";
            }
        }
    }
    return !m_err;
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
        else if(tablename == "history") {
            // unique user_id,operation. timestamp only is updated on repeated operations
            m_err = !q.exec("CREATE TABLE history (user_id INTEGER NOT NULL, "
                            "timestamp DATETIME NOT NULL, "
                            "name TEXT NOT NULL,"
                            "type TEXT NOT NULL,"
                            "formula TEXT NOT NULL,"
                            "host TEXT DEFAULT '',"
                            "h_idx INTEGER NOT NULL,"
                            "UNIQUE (user_id, name, type, host, h_idx) ON CONFLICT REPLACE,"
                            "PRIMARY KEY(user_id,type,h_idx) )");
        }
        else if(tablename == "proc") {
            // procs table saves active monitors/aletrs in order to be restored
            // if the server goes down and up again later.
            m_err = !q.exec("CREATE TABLE proc (user_id INTEGER NOT NULL, chat_id INTEGER NOT NULL, "
                            " start_dt DATETIME NOT NULL, name TEXT NOT NULL, "
                            " type TEXT NOT NULL, formula TEXT DEFAULT '', host TEXT NOT NULL,"
                            " UNIQUE(user_id,chat_id,name,type,host) ON CONFLICT REPLACE)");
        }
        else if(tablename == "config") {
            // procs table saves active monitors/aletrs in order to be restored
            // if the server goes down and up again later.
            m_err = !q.exec("CREATE TABLE config (name TEXT UNIQUE NOT NULL, "
                            "value DOUBLE NOT NULL, description TEXT NOT NULL)");
        }
        else if(tablename == "bookmarks") {
            m_err = !q.exec("CREATE TABLE bookmarks (history_rowid INTEGER NOT NULL,"
                            " timestamp DATETIME NOT NULL,"
                            " UNIQUE(history_rowid) ON CONFLICT REPLACE)");
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

int BotDb::m_findFirstAvailableIdx(const QList<int>& in_idxs)
{
    int a_idx = -1, i;
    for(i = 1; i <= in_idxs.size() && a_idx < 0; i++) {
        if(!in_idxs.contains(i))
            a_idx = i;
    }
    if(a_idx < 0)
        a_idx = i;
    return a_idx;
}
