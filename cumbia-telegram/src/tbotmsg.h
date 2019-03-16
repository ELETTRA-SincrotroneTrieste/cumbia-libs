#ifndef TBOTMSGDATA_H
#define TBOTMSGDATA_H

class QJsonValue;
class HistoryEntry;

#include <QString>
#include <QDateTime>

class TBotMsg
{
public:
    TBotMsg();

    TBotMsg(const QJsonValue& v);

    TBotMsg(const HistoryEntry &he);

    void decode(const QJsonValue& m);

    void print() const;

    void setHost(const QString& h);

    QString host() const;

    bool hasHost() const;

    QString text() const;

    void setText(const QString& t);


    QString chat_first_name;
    int chat_id;
    QString chat_username;
    QString chat_lang;

    QDateTime msg_recv_datetime;
    QDateTime start_dt;

    QString first_name, last_name;
    int user_id;
    QString username ,lang, from, is_bot;

    int message_id;

    int update_id;

    bool from_history;
    bool from_real_msg;

    bool from_private_chat;

private:
    QString m_host;
    QString m_text;
};

#endif // MSGPRINT_H
