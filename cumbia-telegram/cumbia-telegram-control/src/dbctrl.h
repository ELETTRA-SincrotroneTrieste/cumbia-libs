#ifndef DBCTRL_H
#define DBCTRL_H

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QMap>

class DbCtrl
{
public:
    DbCtrl(const QString& dbfile);

    bool userExists(int uid);

    bool setAuthorized(int uid, bool auth);

    bool getUserInfo(QList<QMap<QString, QString> >& in_map);

    bool error() const;

    bool hasMessage() const;

    QString message() const;

private:
    QSqlDatabase m_db;
    bool m_err;
    QString m_msg;
};

#endif // DBCTRL_H
