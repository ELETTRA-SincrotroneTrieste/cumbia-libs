#ifndef BOTCONTROLSERVER_H
#define BOTCONTROLSERVER_H

#include <QObject>

#include "../cumbia-telegram-defs.h" // LOCALSERVER_NAME

class BotControlServerPrivate;
class QLocalSocket;

class BotControlServer : public QObject
{
    Q_OBJECT
public:
    BotControlServer(QObject *parent);

    ~BotControlServer();

    bool error() const;

    QString message() const;

    bool sendControlMessage(QLocalSocket *so, const QString& msg);

signals:
    void newMessage(int uid, int chat_id, ControlMsg::Type t, const QString& msg, QLocalSocket *so);

private slots:
    void onNewConnection();
    void onNewData();

private:
    BotControlServerPrivate *d;

    void m_sendReply(QLocalSocket *so, int uid, int chat_id, ControlMsg::Type t, const QString& msg_received);

    void m_fillSenderData(QJsonObject &jo);
};

#endif // BotControlServer_H
