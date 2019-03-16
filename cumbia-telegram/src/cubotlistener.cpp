#include "cubotlistener.h"

#include <cumacros.h>
#include <cudata.h>
#include <QtDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QIODevice>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonObject>
#include <QTimer>

#include "tbotmsg.h"

class CuBotListenerPrivate {
public:
    QNetworkAccessManager *manager;
    QNetworkRequest netreq;
    bool stop;
    long int last_id;
    QTimer *timer;
    int msg_poll_ms, old_msg_discard_secs;
    QString bot_key;
};

CuBotListener::CuBotListener(QObject *parent, const QString& bot_tok, int msg_poll_t, int msg_discard_old_t) : QObject (parent)
{
    d = new CuBotListenerPrivate;
    d->manager = nullptr;
    d->last_id = -1;
    d->timer = nullptr;
    d->stop = false;
    d->msg_poll_ms = msg_poll_t;
    d->old_msg_discard_secs = msg_discard_old_t;
    d->bot_key = bot_tok;
}

CuBotListener::~CuBotListener()
{
    predtmp("~CuBotListener %p", this);
}

void CuBotListener::start()
{
    if(!d->manager) {
        d->manager = new QNetworkAccessManager(this);
        d->netreq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        d->netreq.setRawHeader("User-Agent", "cumbia-telegram-bot 1.0");
        d->timer = new QTimer(this);
        connect(d->timer, SIGNAL(timeout()), this, SLOT(getUpdates()));
        d->timer->setInterval(d->msg_poll_ms);
        d->timer->setSingleShot(true);
        d->timer->start();
    }
}

void CuBotListener::stop()
{
    d->stop = true;
    if(d->timer)
        d->timer->stop();
}

void CuBotListener::getUpdates()
{
    QString u = QString("https://api.telegram.org/%1/getUpdates?offset=")
            .arg(d->bot_key);
    u += QString::number(++d->last_id);
    d->netreq.setUrl(QUrl(u));
//    qDebug() << __FUNCTION__ << "getting" << u;
    QNetworkReply *reply = d->manager->get(d->netreq);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReply()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError )), this, SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void CuBotListener::onReply()
{
    bool decode_err = false;
    QString message;
    QDateTime now = QDateTime::currentDateTime();
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    const QByteArray ba = reply->readAll();
    QString resp = QString(ba);
    reply->deleteLater();
    QJsonParseError pe;
    QJsonDocument jdoc = QJsonDocument::fromJson(ba, &pe);
    decode_err = (pe.error != QJsonParseError::NoError);
    if(decode_err)
        message = "CuBotListener.onReply" + pe.errorString() + ": offset: " + QString::number(pe.offset);
    else {
        const QJsonValue& result = jdoc["result"];
        decode_err = !jdoc["ok"].toBool();
        if(!decode_err && result.isArray()) {
            QJsonArray result_array = result.toArray();
            if(result_array.size() > 0) {
                for(int i = 0; i < result_array.size(); i++) {
                    const QJsonValue &jv = result_array[i];
                    TBotMsg tmsg(jv);
                    const QJsonValue &j_update_id = jv["update_id"];
                    decode_err = j_update_id.isNull();
                    if(!decode_err) {
                        d->last_id = j_update_id.toInt();
                    }
                    if(tmsg.msg_recv_datetime.secsTo(now) < d->old_msg_discard_secs)
                        emit onNewMessage(tmsg);
                    else {
                        printf("discarding old message from %s dating %s\n", qstoc(tmsg.username),
                               qstoc(tmsg.msg_recv_datetime.toString()));
                    }
                } // for
            }

            if(!d->stop)
                d->timer->start();
        }
    }
    if(decode_err) {
        perr("CuBotListener.onReply: %s", qstoc(message));
        emit onError("CuBotListener.onReply", message);
    }
}

void CuBotListener::onNetworkError(QNetworkReply::NetworkError e)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    perr("CuBotListener.onNetworkError: %s [%d]", reply->errorString().toStdString().c_str(), e);
    reply->deleteLater();
    if(!d->stop && !d->timer->isActive())
        d->timer->start(1000);
}

void CuBotListener::onSSLErrors(const QList<QSslError> &errors)
{
    qDebug() <<__FUNCTION__ << "ssl errors" << errors.size();
    foreach(QSslError e, errors) {
        perr("CuBotListener.onSSLErrors: %s", e.errorString().toStdString().c_str());
    }
    qobject_cast<QNetworkReply *>(sender())->deleteLater();
    if(!d->stop && !d->timer->isActive())
        d->timer->start(1000);
}

