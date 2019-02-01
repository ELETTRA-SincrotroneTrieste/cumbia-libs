#include "cubotsender.h"
#include <cudata.h>
#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QIODevice>
#include <cumacros.h>

class CuBotSenderPrivate {
public:
    QNetworkAccessManager *manager;
    QNetworkRequest netreq;
};

CuBotSender::CuBotSender(QObject *parent) : QObject(parent)
{
    d = new CuBotSenderPrivate;
    d->manager = new QNetworkAccessManager(this);
    d->netreq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    d->netreq.setRawHeader("User-Agent", "cumbia-telegram-bot 1.0");
}

void CuBotSender::sendMessage(int chat_id, const QString &msg)
{
    QString u = "https://api.telegram.org/bot635922604:AAEgG6db_3kkzYZqh-LBxi-ubvl5UIEW7gE/sendMessage?parse_mode=HTML&chat_id=";
    u += QString::number(chat_id) + "&text=" + msg;
    d->netreq.setUrl(QUrl(u));
    QNetworkReply *reply = d->manager->get(d->netreq);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReply()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError )), this, SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void CuBotSender::onNetworkError(QNetworkReply::NetworkError nerr)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    perr("CuBotSender::onNetworkError: %s [%d]", qstoc(reply->errorString()), nerr);
    reply->deleteLater();
}

void CuBotSender::onReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    printf("CuBotSender.onReply: got %s\n", qstoc(QString(reply->readAll())));
    reply->deleteLater();
}
