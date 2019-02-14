#include "botlocalserver.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <cumacros.h>
#include <QByteArray>
#include <QtDebug>

#include "../cumbia-telegram-defs.h" // LOCALSERVER_NAME

class BotLocalServerPrivate
{
public:
    bool error;
    QString msg;

};

BotLocalServer::BotLocalServer(QObject *parent) : QObject (parent)
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

BotLocalServer::~BotLocalServer()
{
    QLocalServer *server = findChild<QLocalServer *>();
    if(server && server->isListening())
        server->close();
    delete d;
}

bool BotLocalServer::error() const
{
    return d->error;
}

QString BotLocalServer::message() const
{
    return d->msg;
}

void BotLocalServer::onNewConnection()
{
    QLocalServer *server = findChild<QLocalServer *>();
    if(server && server->isListening()) {
        QLocalSocket *so =  server->nextPendingConnection();
        connect(so, SIGNAL(readyRead()), this, SLOT(onNewData()));
    }
}

void BotLocalServer::onNewData()
{
    QLocalSocket *so = qobject_cast<QLocalSocket *>(sender());
    QByteArray ba = so->readAll();
    qDebug() << __PRETTY_FUNCTION__ << "read" << ba;
}
