#include "tbotmsg.h"
#include <QJsonValue>
#include <cumacros.h>
#include <QDateTime>

TBotMsg::TBotMsg()
{
    chat_id = -1;
    user_id = -1;
    is_bot = false;
    update_id = -1;
    message_id = -1;
}

TBotMsg::TBotMsg(const QJsonValue &v)
{
    decode(v);
}

void TBotMsg::decode(const QJsonValue &m)
{
    QJsonValue msg = m["message"];
    QJsonValue chat = msg["chat"];
    QJsonValue chat_type = chat["type"].toString();
    chat_first_name = chat["first_name"].toString();
    chat_id = chat["id"].toInt();
    chat_username = chat["username"].toString();
    chat_lang = chat["language_code"].toString();

    dt = QDateTime::fromTime_t(msg["date"].toInt());

    QJsonValue from = msg["from"];
    first_name = chat["first_name"].toString();
    user_id = from["id"].toInt();
    username = from["username"].toString();
    lang = from["language_code"].toString();
    is_bot = from["is_bot"].toString();

    message_id = msg["message_id"].toInt();
    text  = msg["text"].toString();

    update_id = m["update_id"].toInt();
}

void TBotMsg::print() const
{
    printf("\e[1;32m--> \e[0;32m%d\e[0m:\t[%s]\tfrom:\e[1;32m%s\e[0m [\e[1;34m%s\e[0m]: \e[1;37;4mMSG\e[0m\t"
           "\e[0;32m[\e[0m\"%s\"\e[0;32m]\e[0m\t[msg.id: %d]\n",
           update_id, qstoc(dt.toString()), qstoc(username), qstoc(first_name),
           qstoc(text), message_id);
}

void TBotMsg::setHost(const QString &h)
{
    m_host = h;
}

QString TBotMsg::host() const
{
    return m_host;
}

bool TBotMsg::hasHost() const
{
    return !m_host.isEmpty();
}
