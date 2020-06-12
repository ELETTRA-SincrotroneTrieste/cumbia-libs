#include "cuhttpactiona.h"
#include "cumbiahttpworld.h"
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

QNetworkAccessManager *CuHTTPActionA::getNetworkAccessManager() const {
    return d->nam;
}

QNetworkRequest CuHTTPActionA::prepareRequest(const QUrl &url) const {
    QNetworkRequest r(url);
    r.setRawHeader("Accept", "application/json");
    r.setHeader(QNetworkRequest::UserAgentHeader, QByteArray("cumbia-http ") + QByteArray(CUMBIA_HTTP_VERSION_STR));
    return r;
}

/*!
 * \brief the default implementation calls onActionFinished on the registered CuHttpListener
 *
 * This is normally called from onReplyDestroyed, but CuHttpActionWriter can call this
 * method to unregister tha action after the authentication fails
 */
void CuHTTPActionA::notifyActionFinished() {
    d->listener->onActionFinished(getSourceName().toStdString(), getType());
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

CuHTTPActionA::MsgFmt CuHTTPActionA::m_likely_format(const QByteArray &ba) const {
    if(ba.startsWith("<html>")) return FmtHtml;
    return FmtJson;
}

bool CuHTTPActionA::m_likely_valid(const QByteArray &ba) const {
    return !ba.startsWith(": hi\n");
}

void CuHTTPActionA::m_on_buf_complete() {
    if(m_likely_valid(d->buf)) {
        QJsonParseError jpe;
        QByteArray json = m_extract_data(d->buf);
        QJsonDocument jsd = QJsonDocument::fromJson(json, &jpe);
        if(jsd.isNull())
            perr("CuHTTPActionA.m_on_buf_complete: invalid json: %s\n", qstoc(json));
        decodeMessage(jsd);
    }
}

void CuHTTPActionA::onNewData() {
    QByteArray ba = d->reply->readAll();
    bool buf_empty = d->buf.isEmpty();
    if(!buf_empty)
        cuprintf("CuHTTPActionA::onNewData: buf completed by \e[1;32m%s\e[0m\n", ba.data());
    d->buf += ba;
    // buf complete?
    if(d->buf.endsWith("\n\n")) { // buf complete
        m_on_buf_complete();
        d->buf.clear();
    }
    else
        cuprintf("CuHTTPActionA::onNewData: \e[1;35mbuf \e[0;35m%s\e[1;35m incomplete waiting for next buf from the net\e[0m\n", ba.data());
}

void CuHTTPActionA::onReplyFinished() {
    qDebug() << __PRETTY_FUNCTION__ << getType() << "deleting reply later";
    d->reply->deleteLater();
}

void CuHTTPActionA::onReplyDestroyed(QObject *) {
    if(exiting())
        notifyActionFinished();
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
        disconnect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, nullptr);
        disconnect(d->reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, nullptr);
        d->reply->close();
    }
}

