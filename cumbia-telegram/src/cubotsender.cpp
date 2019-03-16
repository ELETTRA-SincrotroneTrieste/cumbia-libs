#include "cubotsender.h"
#include <cudata.h>
#include <QString>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QIODevice>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <cumacros.h>
#include <unistd.h>
#include <QEventLoop>

#define TELEGRAM_MAX_MSGLEN 4096

class CuBotSenderPrivate {
public:
    QString key;
    QNetworkAccessManager *manager;
    QNetworkRequest netreq;
};

CuBotSender::CuBotSender(QObject *parent, const QString& bot_tok) : QObject(parent)
{
    d = new CuBotSenderPrivate;
    d->manager = new QNetworkAccessManager(this);
    d->netreq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    d->netreq.setRawHeader("User-Agent", "cumbia-telegram-bot 1.0");
    d->key = bot_tok;
}

void CuBotSender::sendMessage(int chat_id, const QString &msg, bool silent, bool wait_for_reply)
{
    QString u = QString("https://api.telegram.org/%1/sendMessage").arg(d->key);
    QUrlQuery params;
    params.addQueryItem("chat_id", QString::number(chat_id));
    params.addQueryItem("parse_mode", "HTML");
    params.addQueryItem("text", msg);
    if(msg.length() > TELEGRAM_MAX_MSGLEN) {
         params.addQueryItem("text", m_truncateMsg(msg));
    }
    else {
        params.addQueryItem("text", msg);
    }
    if(silent)
        params.addQueryItem("disable_notification", "true");

    // disable link preview (currently only help would contain links)
    params.addQueryItem("disable_web_page_preview", "true");

    QUrl url(u);
    url.setQuery(params);
    d->netreq.setUrl(url);

    QNetworkReply *reply = d->manager->get(d->netreq);
    if(wait_for_reply) {
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        reply->deleteLater();
    }
    else {
        connect(reply, SIGNAL(readyRead()), this, SLOT(onReply()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError )), this, SLOT(onNetworkError(QNetworkReply::NetworkError)));
    }
}

void CuBotSender::sendPic(int chat_id, const QByteArray &imgBytes, bool silent)
{
    QHttpMultiPart *mpart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imgPart;
    imgPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"photo\";"
                                                                                  "filename=\"plot.png\"")));
    imgPart.setHeader(QNetworkRequest::ContentTypeHeader, QMimeDatabase().mimeTypeForData(imgBytes).name());
    imgPart.setBody(imgBytes);
    mpart->append(imgPart);

    QString u = "https://api.telegram.org/bot635922604:AAEgG6db_3kkzYZqh-LBxi-ubvl5UIEW7gE/sendPhoto";
    QUrlQuery params;
    params.addQueryItem("chat_id", QString::number(chat_id));
    if(silent)
        params.addQueryItem("disable_notification", "true");
    QUrl url(u);
    url.setQuery(params);
    d->netreq.setUrl(url);
    QNetworkReply *reply = d->manager->post(d->netreq, mpart);
    mpart->setParent(reply);  // delete the multiPart with the reply
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
//    printf("CuBotSender.onReply: \e[1;32mgot %s\e[0m going to delete reply later\n", qstoc(QString(reply->readAll())));
    reply->deleteLater();
}

QString CuBotSender::m_truncateMsg(const QString &in)
{
    QString trunc = in;
    QString suffix = " ... \n\n(<b>msg too long</b>)";
    trunc.truncate(TELEGRAM_MAX_MSGLEN - suffix.length());
    // remove all html tags
    trunc.remove("<b>").remove("</b>").remove("<i>").remove("</i>");
    return trunc + suffix;
}
