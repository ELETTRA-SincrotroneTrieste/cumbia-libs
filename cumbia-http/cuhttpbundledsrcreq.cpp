#include "cuhttpbundledsrcreq.h"
#include "cuhttpactionfactoryi.h"
#include "cumbiahttpworld.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtDebug>

class CuHttpBundledSrcReqPrivate {
public:
    CuHttpBundledSrcReqPrivate(const QList<SrcItem>& srcs) {
        req_payload = m_json_pack(srcs);
    }

    CuHttpBundledSrcReqPrivate(const QMap<QString, SrcData>& srcs) {
        QList<SrcItem> il;
        foreach(QString s, srcs.keys()) {
            const SrcData& sd = srcs[s];
            il.append(SrcItem(s.toStdString(), sd.lis, sd.method, sd.channel, sd.wr_val));
        }
        req_payload = m_json_pack(il);
    }

    QByteArray buf;
    QByteArray req_payload, cookie;
    QByteArray m_json_pack(const QList<SrcItem>& srcs) const;
    CuHttpBundledSrcReqListener *listener;
};

CuHttpBundledSrcReq::CuHttpBundledSrcReq(const QList<SrcItem> &srcs,
                                         CuHttpBundledSrcReqListener *l,
                                         QObject *parent) : QObject(parent) {
    d = new CuHttpBundledSrcReqPrivate(srcs);
    d->listener = l;
}

CuHttpBundledSrcReq::CuHttpBundledSrcReq(const QMap<QString, SrcData> &targetmap, CuHttpBundledSrcReqListener *l, const QByteArray &cookie, QObject *parent) {
    d = new CuHttpBundledSrcReqPrivate(targetmap);
    d->listener = l;
    d->cookie = cookie;
}

CuHttpBundledSrcReq::~CuHttpBundledSrcReq() {
    delete d;
}

void CuHttpBundledSrcReq::start(const QUrl &url, QNetworkAccessManager *nam)
{
    QNetworkRequest r(url);
    r.setRawHeader("Accept", "application/json");
    r.setHeader(QNetworkRequest::UserAgentHeader, QByteArray("cumbia-http ") + QByteArray(CUMBIA_HTTP_VERSION_STR));
    if(!d->cookie.isEmpty())
        r.setRawHeader("Cookie", d->cookie);

    printf("\e[1;36mCuHttpBundledSrcReq::start: PAYLOAD:\n%s COOKIE %s\e[0m\n", d->req_payload.data(), d->cookie.data());
    QNetworkReply *reply = nam->post(r, d->req_payload);
    reply->setProperty("payload", d->req_payload);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onNewData()));
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(onSslErrors(const QList<QSslError> &)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(destroyed(QObject *)), this, SLOT(onReplyDestroyed(QObject *)));
}

void CuHttpBundledSrcReq::onNewData() {
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    qint64 bytes_avail = r->bytesAvailable();
    QByteArray ba = r->read(bytes_avail);
//    qDebug() << __PRETTY_FUNCTION__ << "received " << ba;
    bool buf_empty = d->buf.isEmpty();
    if(!buf_empty)
        cuprintf("CuHTTPActionA::onNewData: buf completed by \e[1;32m%s\e[0m\n", ba.data());
    d->buf += ba;
    // buf complete?
    if(d->buf.endsWith("\n\n") || d->buf.endsWith("\r\n\r\n")) { // buf complete
        m_on_buf_complete();
        d->buf.clear();
    }
    else
        cuprintf("CuHTTPActionA::onNewData: \e[1;35mbuf \e[0;35m%s\e[1;35m incomplete waiting for next buf from the net\e[0m\n", ba.data());
}

void CuHttpBundledSrcReq::onReplyFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << this << "deleting reply " << sender() << " later";
    sender()->deleteLater();
}

void CuHttpBundledSrcReq::onReplyDestroyed(QObject *o)
{
    qDebug() << __PRETTY_FUNCTION__ << this << "deleted reply " << o ;
}

void CuHttpBundledSrcReq::onSslErrors(const QList<QSslError> &errors) {
    QString msg;
    foreach(const QSslError &e, errors)
        msg += e.errorString() + "\n";
    qDebug() << __PRETTY_FUNCTION__ << msg;
}

void CuHttpBundledSrcReq::onError(QNetworkReply::NetworkError code)
{
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    QJsonObject eo = CumbiaHTTPWorld().make_error(r->errorString() + QString( "code %1").arg(code));
    qDebug() << __PRETTY_FUNCTION__ << "!!!!!!!!!!!!!!" << QJsonValue(eo).toString() << "error string" << r->errorString() <<
                r->property("payload").toString();
}

void CuHttpBundledSrcReq::m_on_buf_complete() {
    if(m_likely_valid(d->buf)) {  // discard hi:
        QJsonParseError jpe;
        QByteArray json = d->buf;
        QJsonDocument jsd = QJsonDocument::fromJson(json, &jpe);
        if(jsd.isNull())
            perr("CuHTTPActionA.m_on_buf_complete: invalid json: %s\n", qstoc(json));
        d->listener->onSrcBundleReplyReady(jsd.toJson());
    }
}

// discard hello message
bool CuHttpBundledSrcReq::m_likely_valid(const QByteArray &ba) const {
    return !ba.startsWith(": hi\n");
}


QByteArray CuHttpBundledSrcReqPrivate::m_json_pack(const QList<SrcItem> &srcs) const
{
    QString channel;
    QJsonObject root_o;
    root_o["type"] = "srcs";
    QJsonArray sa;
    foreach(const SrcItem& i, srcs) {
        QJsonObject so;
        QJsonArray options { "p" };
        so["options"] = options;
        so["method"] = QString::fromStdString(i.method);
        i.method != "write" ? so["src"] = QString::fromStdString(i.src) :
                so["src"] = QString("%1(%2)").arg(i.src.c_str()).arg(i.wr_val.toString().c_str());
        // keys array
        QJsonArray keys { "src", "method", "options" };

        if(channel.size() == 0)
            channel = i.channel;
        else if(channel != i.channel) {
            so["channel"] = i.channel; // specific channel
            keys.append("channel");
        }
        so["keys"] = keys;
        sa.append(so);
    }
    // hopefully an app uses a single channel
    if(channel.size())
        root_o["channel"] = channel;
    root_o["srcs"] = sa;
    QJsonDocument doc(root_o);
    return doc.toJson(QJsonDocument::Compact) + "\r\n\r\n";
}
