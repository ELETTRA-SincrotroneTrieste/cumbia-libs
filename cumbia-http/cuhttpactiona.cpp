#include "cuhttpactiona.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QtDebug>

class CuHTTPActionAPrivate {
public:
    QNetworkAccessManager *nam;
    CuHTTPActionListener *listener;
    QByteArray buf;
};

CuHTTPActionA::CuHTTPActionA(QNetworkAccessManager *nam) {
    d = new CuHTTPActionAPrivate;
    d->nam = nam;
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
    int idx = d->buf.lastIndexOf("\ndata: ");
    if(idx > 0) {
        QByteArray jsonba = d->buf.replace(0, idx + strlen("\ndata: "), "");
        cuprintf("outta \e[1;35m%s\e[0m working on \e[1;32m%s\e[0m\n", d->buf.data(), jsonba.data());
        QJsonDocument jsd = QJsonDocument::fromJson(jsonba, &jpe);
        std::string src;
        jsd["event"].toString().length() > 0 ? src = jsd["event"].toString().toStdString() : src = jsd["src"].toString().toStdString();
        decodeMessage(jsd);
    }
    d->buf.clear();
}

QNetworkRequest CuHTTPActionA::prepareRequest(const QUrl &url) const {
    QNetworkRequest r(url);
    r.setRawHeader("Accept", "application/json");
    r.setHeader(QNetworkRequest::UserAgentHeader, QByteArray("cumbia-http ") + QByteArray(CUMBIA_HTTP_VERSION_STR));
    return r;
}

void CuHTTPActionA::onNewData() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    d->buf = reply->readAll();
    qDebug() << __PRETTY_FUNCTION__ << d->buf;
    if(true) // buf complete
        m_on_buf_complete();
}

void CuHTTPActionA::onReplyFinished() {
    if(exiting()) {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        d->listener->onActionFinished(getSource().getName(), getType());
        reply->deleteLater();
    }
}

void CuHTTPActionA::onSslErrors(const QList<QSslError> &errors) {
    foreach(const QSslError &e, errors)
        qDebug() << __PRETTY_FUNCTION__ << e.errorString();
}

void CuHTTPActionA::onError(QNetworkReply::NetworkError code) {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qDebug() << __PRETTY_FUNCTION__ << reply->errorString() << "code" << code;
}

void CuHTTPActionA::startRequest(const QUrl &src)
{
    qDebug () << __PRETTY_FUNCTION__ << src;
    QNetworkRequest r = prepareRequest(src);
    QNetworkReply *reply = d->nam->get(r);
    reply->setParent(this);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onNewData()));
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(onSslErrors(const QList<QSslError> &)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

void CuHTTPActionA::stopRequest() {
    QNetworkReply *r = findChild<QNetworkReply *>();
    if(r) {
        cuprintf("CuHTTPActionA.stopRequest: closing %p\n", r);
        disconnect(this, SLOT(onError(QNetworkReply::NetworkError)));
        r->close();
    }
}

