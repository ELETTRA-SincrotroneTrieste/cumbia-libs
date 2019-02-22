#include "dbctrl.h"

#include <QtDebug>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDateTime>

DbCtrl::DbCtrl(const QString &dbfile)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbfile);
    m_err = !m_db.open("tbotdb", "tbotdb");
    if(m_err)
        m_msg = m_db.lastError().text();
}

bool DbCtrl::userExists(int uid)
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

bool DbCtrl::setAuthorized(int uid, bool auth)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    m_err = !userExists(uid);

    QSqlQuery q(m_db);
    if(m_err) {
        m_msg = QString("user %1 does not exist").arg(uid);
    }
    else {
        int au;
        auth ? au = 1 : au = 0;

        m_err = !q.exec(QString("INSERT OR REPLACE INTO auth VALUES(%1,%2,datetime())").arg(uid).arg(au));
        if(!m_err) {
            m_err = !q.exec(QString("SELECT id,uname,first_name,last_name,join_date,authorized FROM users,"
                                    "auth WHERE id=%1 AND user_id=id").arg(uid));
            if(!m_err && q.next()) {
                QSqlRecord r = q.record();
                m_msg = QString("user \"%1\" name \"%2\" last name \"%3\" join date %4 AUTHORIZED: %5")
                        .arg(q.value("uname").toString()).arg(q.value("first_name").toString())
                        .arg(q.value("last_name").toString())
                        .arg(q.value("join_date").toDateTime().toString())
                        .arg(q.value("authorized").toBool());
            }
        }
        if(m_err)
            m_msg = q.lastQuery() + ":" + q.lastError().text();
    }

    return !m_err;
}

bool DbCtrl::getUserInfo(QList<QMap<QString, QString> > &in_map)
{
    if(!m_db.isOpen())
        return false;
    m_msg.clear();
    QSqlQuery q(m_db);
    QList<int> auth_list;
    m_err = !q.exec(QString("SELECT id,uname,first_name,last_name,join_date,authorized FROM users,"
                            "auth WHERE user_id=id ORDER BY authorized ASC"));
    QMap<QString, QString> map;
    while(!m_err && q.next()) {
        QSqlRecord r = q.record();
        for(int i = 0; i < r.count(); i++)
            map[r.fieldName(i)] = r.value(r.fieldName(i)).toString();
        in_map << map;
        auth_list << r.value("id").toInt();
    }
    if(m_err)
        m_msg = q.lastQuery() + ":" + q.lastError().text();
    else {
        m_err = !q.exec("SELECT id,uname,first_name,last_name,join_date FROM users");
        while(!m_err && q.next()) {
            int userid = q.value(0).toInt();
            if(!auth_list.contains(userid)) {
                QSqlRecord r = q.record();
                for(int i = 0; i < r.count(); i++) {
                    map[r.fieldName(i)] = r.value(r.fieldName(i)).toString();
                }
                map["authorized"] = "-1";
                in_map << map;
            }
        }
    }
    if(m_err)
        m_msg = q.lastQuery() + ":" + q.lastError().text();

    return !m_err;
}

bool DbCtrl::error() const
{
    return m_err;
}

bool DbCtrl::hasMessage() const
{
    return m_msg.length() > 0;
}

QString DbCtrl::message() const
{
    return m_msg;
}
