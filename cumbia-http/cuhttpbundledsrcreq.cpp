#include "cuhttpbundledsrcreq.h"
#include "cuhttpactionfactoryi.h"
#include "cumbiahttpworld.h"
#include <cudata.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

// debug
#include <QEventLoop>
#include <QtDebug>
#include <qustringlist.h>

class CuHttpBundledSrcReqPrivate {
public:
    CuHttpBundledSrcReqPrivate(const QList<SrcItem>& srcs) {
        req_payload = m_json_pack(srcs);
        blocking = false;
    }

    CuHttpBundledSrcReqPrivate(const QMap<QString, SrcData>& srcs) {
        QList<SrcItem> il;
        foreach(QString s, srcs.keys()) {
            const SrcData& sd = srcs[s];
            il.append(SrcItem(s.toStdString(), sd.lis, sd.method, sd.channel, sd.wr_val, sd.options));
            if(channel.isEmpty() && !sd.channel.isEmpty())
                channel = sd.channel.toLatin1();
            else if(channel != sd.channel.toLatin1())
                perr("CuHttpBundledSrcReqPrivate: cannot mix channels in the same request: load balanced service instances may misbehave");
        }
        req_payload = m_json_pack(il);
    }

    QByteArray buf;
    QByteArray req_payload, cookie, channel;
    QByteArray m_json_pack(const QList<SrcItem>& srcs);
    CuHttpBundledSrcReqListener *listener;
    bool blocking;
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
    r.setRawHeader("Content-Type", "application/json");
    r.setHeader(QNetworkRequest::UserAgentHeader, QByteArray("cumbia-http ") + QByteArray(CUMBIA_HTTP_VERSION_STR));
    if(!d->cookie.isEmpty())
        r.setRawHeader("Cookie", d->cookie);
    if(!d->channel.isEmpty()) {
        r.setRawHeader("X-Channel", d->channel);
    }
    QNetworkReply *reply = nam->post(r, d->req_payload);
    reply->setProperty("payload", d->req_payload);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onNewData()));
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(onSslErrors(const QList<QSslError> &)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(destroyed(QObject *)), this, SLOT(onReplyDestroyed(QObject *)));
    if(d->blocking) {
        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
    }
}

void CuHttpBundledSrcReq::setBlocking(bool b) {
    d->blocking = b;
}

void CuHttpBundledSrcReq::onNewData() {
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    int clen = r->header(QNetworkRequest::ContentLengthHeader).toInt();
    qint64 bytes_avail = r->bytesAvailable();
    QByteArray ba = r->read(bytes_avail);
    d->buf += ba;
    // buf complete?
    if(d->buf.length() == clen || d->buf.endsWith("\n\n") || d->buf.endsWith("\r\n\r\n")) { // buf complete
        m_on_buf_complete();
        d->buf.clear();
    }
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
    perr("CuHttpBundledSrcReq::onSslErrors: errors: %s", qstoc(msg));
}

void CuHttpBundledSrcReq::onError(QNetworkReply::NetworkError code)
{
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    QJsonObject eo = CumbiaHTTPWorld().make_error(r->errorString() + QString( "code %1").arg(code));
    perr("CuHttpBundledSrcReq::onError: %s/%s", qstoc(r->errorString()), qstoc(r->property("payload").toString()));
}

void CuHttpBundledSrcReq::m_on_buf_complete() {
    if(m_likely_valid(d->buf)) {  // discard hi:
        QJsonParseError jpe;
        QByteArray json = d->buf;
        QJsonDocument jsd = QJsonDocument::fromJson(json, &jpe);
        if(jsd.isNull())
            perr("CuHttpBundledSrcReq.m_on_buf_complete: invalid json: %s\n", qstoc(json));
        d->listener->onSrcBundleReplyReady(jsd.toJson());
    }
}

// discard hello message
bool CuHttpBundledSrcReq::m_likely_valid(const QByteArray &ba) const {
    return !ba.startsWith(": hi\n");
}

QByteArray CuHttpBundledSrcReqPrivate::m_json_pack(const QList<SrcItem> &srcs)
{
    QJsonObject root_o;
    QJsonArray sa;
    foreach(const SrcItem& i, srcs) {
        QJsonObject so;
        QJsonArray options;
        const std::vector<std::string> &o = i.options.keys();
        for(const std::string& s : o) {
            QJsonArray v_ops;
            options.append(s.c_str());
            const CuVariant &v = i.options[s];
            if(v.getFormat() == CuVariant::Vector) { // option is a vector
                const std::vector<std::string> &vs = i.options[s].toStringVector();
                for(size_t i = 0; i < vs.size(); i++)
                    v_ops.append(vs[i].c_str());
                so[s.c_str()] = v_ops;
            }
            else // scalar option
                so[s.c_str()] = i.options[s].toString().c_str();
        }

        so["options"] = options;
        so["method"] = QString::fromStdString(i.method);
        i.method != "write" ? so["src"] = QString::fromStdString(i.src) :
                so["src"] = QString("%1(%2)").arg(i.src.c_str()).arg(i.wr_val.toString().c_str());
        if(channel.size() == 0 && !i.channel.isEmpty())
            channel = i.channel.toLatin1();
        else if(!i.channel.isEmpty() && channel != i.channel) {
            so["channel"] = i.channel; // specific channel
            perr("CuHttpBundledSrcReqPrivate.m_json_pack: cannot mix channels (%s/%s) in the same request: load balanced service instances may misbehave",
                 channel.data(), qstoc(i.channel));
        }
        sa.append(so);
    }
    // hopefully an app uses a single channel
    if(channel.size())
        root_o["channel"] = QString(channel);
    root_o["srcs"] = sa;
    QJsonDocument doc(root_o);
    return doc.toJson(QJsonDocument::Compact);
}
