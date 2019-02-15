#include "botcontrolserver.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <cumacros.h>
#include <QByteArray>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>

class BotLocalServerPrivate
{
public:
    bool error;
    QString msg;

};

BotControlServer::BotControlServer(QObject *parent) : QObject (parent)
{
    d = new BotLocalServerPrivate();
    QLocalServer *server = new QLocalServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    d->error = !server->listen(LOCALSERVER_NAME);
    if(d->error) {
        d->msg = server->errorString();
        perr("BotLocalServer: error opening local socket: %s", qstoc(d->msg));

        if(server->serverError() == QAbstractSocket::AddressInUseError) {
            perr("BotLocalServer: AddressInUseError: please remove the file \"\e[1;31m%s\e[0m\""
                 "\nand restart the application", qstoc(server->serverName()));
        }
    }
    else {
        printf("BotLocalServer: listening on \e[0;32m%s\e[0m\n", qstoc(server->serverName()));
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
        int uid = jd["uid"].toInt(-1);
        int chat_id = jd["chat_id"].toInt(-1);
        QString msg = jd["msg"].toString("");
        // ControlMsg::Type -1 is undefined
        ControlMsg::Type t = static_cast<ControlMsg::Type>(jd["ctrl_type"].toInt(-1));
        emit newMessage(uid, chat_id, t, msg);
    }
}
