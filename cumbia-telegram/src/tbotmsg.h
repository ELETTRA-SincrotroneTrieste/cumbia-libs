#ifndef TBOTMSGDATA_H
#define TBOTMSGDATA_H

class QJsonValue;

#include <QString>
#include <QDateTime>

class TBotMsg
{
public:
    TBotMsg();

    TBotMsg(const QJsonValue& v);

    void decode(const QJsonValue& m);

    void print() const;

    void setHost(const QString& h);

    QString host() const;

    bool hasHost() const;


    QString chat_first_name;
    int chat_id;
    QString chat_username;
    QString chat_lang;

    QDateTime dt;

    QString first_name, last_name;
    int user_id;
    QString username ,lang, from, is_bot;

    int message_id;
    QString text;

    int update_id;

private:
    QString m_host;
};

#endif // MSGPRINT_H
