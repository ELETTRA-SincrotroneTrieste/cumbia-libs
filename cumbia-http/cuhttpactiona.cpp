#include "cuhttpactiona.h"
#include "cumbiahttpworld.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QtDebug>

static int reqs_started = 0, reqs_ended = 0;

class CuHTTPActionAPrivate {
public:
};

CuHTTPActionA::CuHTTPActionA(QNetworkAccessManager *nam) {
    d = new CuHTTPActionAPrivate;
    d->nam = nam;
    d->reply = nullptr;
}

CuHTTPActionA::~CuHTTPActionA() {
    pdelete("~CuHTTPActionA %p", this);
    delete d;
}

QNetworkAccessManager *CuHTTPActionA::getNetworkAccessManager() const {
    return d->nam;
}

// data from event source has a combination of fields, one per line
// (event, id, retry, data)
// https://developer.mozilla.org/en-US/docs/Web/API/Server-sent_events/Using_server-sent_events
// Here we extract data
//
QList<QByteArray> CuHTTPActionA::m_extract_data(const QByteArray &in) const {
    QList<QByteArray> jdl; // json data list
    QList<QByteArray> dl = in.split('\n');
    foreach(QByteArray bai, dl) {
        int idx = bai.lastIndexOf("data: ");
        if(idx >= 0) {
            bai.replace(0, idx + strlen("data: "), "");
            jdl << bai;
        }
    }
    return jdl;
}

// discard hello message
bool CuHTTPActionA::m_likely_valid(const QByteArray &ba) const {
    return !ba.startsWith(": hi\n");
}

void CuHTTPActionA::m_on_buf_complete() {
//    printf("\e[1;36mCuHTTPActionA::m_on_buf_complete: %s\e[0m\n", d->buf.data());
    if(m_likely_valid(d->buf)) {  // discard hi:
        QJsonParseError jpe;
        QList<QByteArray> jsonli = m_extract_data(d->buf);
        foreach(const QByteArray &json, jsonli) {
            QJsonDocument jsd = QJsonDocument::fromJson(json, &jpe);
            if(jsd.isNull())
                perr("CuHTTPActionA.m_on_buf_complete: invalid json: %s\n", qstoc(json));
            else {
                decodeMessage(jsd.array());
            }
        }
    }
}

void CuHTTPActionA::onNewData() {
    QByteArray ba = d->reply->readAll();
    d->buf += ba;
    // buf complete?
    if(d->buf.endsWith("\n\n") || d->buf.endsWith("\r\n\r\n")) { // buf complete
        m_on_buf_complete();
        d->buf.clear();
    }
}

void CuHTTPActionA::onReplyFinished() {
    reqs_ended++;
    d->reply->deleteLater();
}

void CuHTTPActionA::onReplyDestroyed(QObject *) {
    d->reply = nullptr;
}

void CuHTTPActionA::onSslErrors(const QList<QSslError> &errors) {
    QString msg;
    foreach(const QSslError &e, errors)
        msg += e.errorString() + "\n";
    decodeMessage(CumbiaHTTPWorld().make_error(msg));
}

void CuHTTPActionA::onError(QNetworkReply::NetworkError code) {
    decodeMessage(CumbiaHTTPWorld().make_error(d->reply->errorString() + QString( "code %1").arg(code)));
}

void CuHTTPActionA::startRequest(const QUrl &src)
{
    QNetworkRequest r = prepareRequest(src);
    if(!d->reply) {
        d->reply = d->nam->get(r);
        reqs_started++;
        connect(d->reply, SIGNAL(readyRead()), this, SLOT(onNewData()));
        connect(d->reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
        connect(d->reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(onSslErrors(const QList<QSslError> &)));
        connect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        connect(d->reply, SIGNAL(destroyed(QObject *)), this, SLOT(onReplyDestroyed(QObject *)));
    }
    else {
        perr("CuHTTPActionA::startRequest: error { already in progress }");
    }
}

void CuHTTPActionA::stopRequest() {
    qDebug() << __PRETTY_FUNCTION__ << "stop request : d->reply" << d->reply;
    if(d->reply) {
        disconnect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, nullptr);
        disconnect(d->reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, nullptr);
        if(d->reply->isOpen())
            d->reply->close();
    }
}

void CuHTTPActionA::cancelRequest() {
    if(d->reply && !d->reply->isFinished()) {
        disconnect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, nullptr);
        disconnect(d->reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, nullptr);
        disconnect(d->reply, SIGNAL(readyRead()), this, nullptr);
        printf("\e[1;31mCuHttpActionA %p cancelRequest: CANCELLING REQUEST!!!! { %s } ABORTING REPLY %p\e[0m\n",
               this, qstoc(getSourceName()), d->reply);
        d->reply->abort();
    }
}
