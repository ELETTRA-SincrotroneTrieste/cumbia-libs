#include "cuhttpactiona.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QtDebug>

class CuHTTPActionAPrivate {
public:
    QNetworkAccessManager *nam;
    QNetworkReply *reply;
    CuHTTPActionListener *listener;
    QByteArray buf;
};

CuHTTPActionA::CuHTTPActionA(QNetworkAccessManager *nam) {
    d = new CuHTTPActionAPrivate;
    d->nam = nam;
    d->reply = nullptr;
    d->listener = nullptr;
}

CuHTTPActionA::~CuHTTPActionA() {
    pdelete("~CuHTTPActionA %p", this);
    delete d;
}

void CuHTTPActionA::setHttpActionListener(CuHTTPActionListener *l) {
    d->listener = l;
}

CuHTTPActionListener *CuHTTPActionA::getHttpActionListener() const {
    return d->listener;
}

void CuHTTPActionA::m_on_buf_complete(){
    QJsonParseError jpe;
    QByteArray json = m_extract_data(d->buf);
    QJsonDocument jsd = QJsonDocument::fromJson(json, &jpe);
    decodeMessage(jsd);
    d->buf.clear();
}

// data from event source has a combination of fields, one per line
// (event, id, retry, data)
// https://developer.mozilla.org/en-US/docs/Web/API/Server-sent_events/Using_server-sent_events
// Here we extract data
//
QByteArray CuHTTPActionA::m_extract_data(const QByteArray &in) const {
    QByteArray jd(in);
    int idx = in.lastIndexOf("\ndata: ");
    if(idx > 0)
        jd.replace(0, idx + strlen("\ndata: "), "");
    QJsonParseError e;
    QJsonDocument d = QJsonDocument::fromJson(jd, &e);
    return jd;
}

QNetworkRequest CuHTTPActionA::prepareRequest(const QUrl &url) const {
    QNetworkRequest r(url);
    r.setRawHeader("Accept", "application/json");
    r.setHeader(QNetworkRequest::UserAgentHeader, QByteArray("cumbia-http ") + QByteArray(CUMBIA_HTTP_VERSION_STR));
    return r;
}

void CuHTTPActionA::onNewData() {
    d->buf = d->reply->readAll();
    if(true) // buf complete
        m_on_buf_complete();
}

void CuHTTPActionA::onReplyFinished() {
    d->reply->deleteLater();
}

void CuHTTPActionA::onReplyDestroyed(QObject *) {
    if(exiting())
        d->listener->onActionFinished(getSource().getName(), getType());
    d->reply = nullptr;
}

void CuHTTPActionA::onSslErrors(const QList<QSslError> &errors) {
    foreach(const QSslError &e, errors)
        qDebug() << __PRETTY_FUNCTION__ << e.errorString();
}

void CuHTTPActionA::onError(QNetworkReply::NetworkError code) {
    qDebug() << __PRETTY_FUNCTION__ << d->reply->errorString() << "code" << code;
}

void CuHTTPActionA::startRequest(const QUrl &src)
{
    qDebug () << __PRETTY_FUNCTION__ << src;
    QNetworkRequest r = prepareRequest(src);
    if(!d->reply) {
        d->reply = d->nam->get(r);
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
    if(d->reply) {
        cuprintf("CuHTTPActionA.stopRequest: closing %p\n", d->reply);
        disconnect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)));
        d->reply->close();
    }
}

