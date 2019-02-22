#include "botcontrolserver.h"
#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <cumacros.h>
#include <QByteArray>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>

class BotControlServerPrivate
{
public:
    bool error;
    QString msg;

};

BotControlServer::BotControlServer(QObject *parent) : QObject (parent)
{
    d = new BotControlServerPrivate();
    QLocalServer *server = new QLocalServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    d->error = !server->listen(LOCALSERVER_NAME);
    if(d->error) {
        d->msg = server->errorString();
        perr("BotControlServer: error opening local socket: %s", qstoc(d->msg));

        if(server->serverError() == QAbstractSocket::AddressInUseError) {
            perr("BotControlServer: AddressInUseError: please remove the file \"\e[1;31m%s\e[0m\""
                 "\nand restart the application", LOCALSERVER_NAME);
        }
    }
    else {
        printf("BotControlServer: listening on \e[0;32m%s\e[0m\n", qstoc(server->serverName()));
    }
}

BotControlServer::~BotControlServer()
{
    QLocalServer *server = findChild<QLocalServer *>();
    if(server && server->isListening())
        server->close();
    delete d;
}

bool BotControlServer::error() const
{
    return d->error;
}

QString BotControlServer::message() const
{
    return d->msg;
}

bool BotControlServer::sendControlMessage(QLocalSocket *so, const QString &msg)
{
    QJsonObject jo;
    m_fillSenderData(jo); // app name, pid, cmd line
    jo["msg"] = msg;
    jo["ctrl_type"] = static_cast<int>(ControlMsg::Statistics);
    d->error = !so->write(QJsonDocument(jo).toJson());
    if(d->error) {
        d->msg = "BotControlServer.sendControlMessage: error sending stats: " + so->errorString();
    }
    return !d->error;
}

void BotControlServer::onNewConnection()
{
    QLocalServer *server = findChild<QLocalServer *>();
    if(server && server->isListening()) {
        QLocalSocket *so =  server->nextPendingConnection();
        connect(so, SIGNAL(readyRead()), this, SLOT(onNewData()));
    }
}

void BotControlServer::onNewData()
{
    QLocalSocket *so = qobject_cast<QLocalSocket *>(sender());
    QByteArray ba = so->readAll();
    QJsonDocument jd;
    jd = QJsonDocument::fromJson(ba);
    if(!jd.isEmpty()) {
        ControlMsg::Type t = static_cast<ControlMsg::Type>(jd["ctrl_type"].toInt(-1));
        int uid = jd["uid"].toInt(-1);
        int chat_id = jd["chat_id"].toInt(-1);
        QString msg = jd["msg"].toString("");
        // ControlMsg::Type -1 is undefined
        emit newMessage(uid, chat_id, t, msg, so);
        bool reply = jd["reply"].toBool(false);
        if(reply) {
            m_sendReply(so, uid, chat_id, t, msg);
        }
    }
}

void BotControlServer::m_sendReply(QLocalSocket *so, int uid, int chat_id,
                                   ControlMsg::Type t, const QString &msg_received)
{
    Q_UNUSED(uid);
    Q_UNUSED(chat_id);
    Q_UNUSED(t);
    Q_UNUSED(msg_received);
    if(so->isOpen() && so->isValid()) {
        // send a reply according to type, ...

    }
}

void BotControlServer::m_fillSenderData(QJsonObject &jo)
{
    jo["sender"] = qApp->applicationName();
    jo["sender_cmd_line"] = qApp->arguments().first();
    jo["sender_pid"] = qApp->applicationPid();
}
