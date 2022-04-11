#include "cuhttpchannelreceiver.h"
#include <cumacros.h>
#include "cumbiahttpworld.h"
#include <cudatalistener.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMultiMap>
#include <QNetworkAccessManager>
#include <cudata.h>

static int reqs_started = 0, reqs_ended = 0;

class CuHttpChannelReceiverPrivate {
public:
    CuHttpChannelReceiverPrivate(const QString &_url,  const QString &_chan, QNetworkAccessManager *_nam) :
        url(_url), chan(_chan), nam(_nam), reply(nullptr) {}

    QMultiMap<QString, CuDataListener *> rmap;
    QString url, chan;

    QNetworkAccessManager *nam;
    QNetworkReply *reply;
    QByteArray buf;
};

CuHttpChannelReceiver::CuHttpChannelReceiver(const QString &url, const QString &chan, QNetworkAccessManager *nam) {
    d = new CuHttpChannelReceiverPrivate(url, chan, nam);
}

QString CuHttpChannelReceiver::channel() const {
    return d->chan;
}

QString CuHttpChannelReceiver::chan() const {
    QString channel = d->url + "/sub/" + d->chan;
    return channel;
}

void CuHttpChannelReceiver::addDataListener(const QString &src, CuDataListener *l) {
    d->rmap.insert(src, l);
}

void CuHttpChannelReceiver::removeDataListener(CuDataListener *l) {
    QMutableMapIterator<QString, CuDataListener* > it(d->rmap);
    while(it.hasNext()) {
        it.next();
        if(it.value() == l) {
            it.remove();
        }
    }
}

size_t CuHttpChannelReceiver::dataListenersCount() {
    return d->rmap.size();
}

void CuHttpChannelReceiver::start() {
    const QString& url(d->url + "/sub/" + d->chan);
    QNetworkRequest r = prepareRequest(url);
    if(!d->reply) {
        d->reply = d->nam->get(r);
        reqs_started++;
        connect(d->reply, SIGNAL(readyRead()), this, SLOT(onNewData()));
        connect(d->reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
        connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
        connect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        connect(d->reply, SIGNAL(destroyed(QObject*)), this, SLOT(onReplyDestroyed(QObject*)));
    }
    else {
        perr("%s: error { already in progress }", __PRETTY_FUNCTION__);
    }
}

void CuHttpChannelReceiver::stop() {
    qDebug() << __PRETTY_FUNCTION__ << "stop request : d->reply" << d->reply;
    if(d->reply) {
        disconnect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, nullptr);
        disconnect(d->reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, nullptr);
        if(d->reply->isOpen())
            d->reply->close();
    }
}

/*!
 * \brief CuHttpChannelReceiver::decodeMessage decodes the json array passed as QJsonValue
 *        and updates the listeners of the involved sources.
 * \param v Json array.
 *
 * Invoked by CuHTTPActionA.
 *
 * \note  To avoid receiving older messages which were published before subscribing to channel,
 *        use the directive *nchan_subscriber_first_message newest; * in the *sub* section of
 *        *nginx.conf*
 */
void CuHttpChannelReceiver::decodeMessage(const QJsonValue &v) {
    const QJsonArray a = v.toArray(); // data arrives within an array
    if(a.size() == 1) {
        const QString& src = a.at(0)["src"].toString();
        if(d->rmap.contains(src)) {
            CuData res("src", src.toStdString());
            CumbiaHTTPWorld httpw;
            httpw.json_decode(a.at(0), res);
            foreach(CuDataListener *l, d->rmap.values(src)) {
                l->onUpdate(res);
            }
        }
    }
}

QNetworkRequest CuHttpChannelReceiver::prepareRequest(const QUrl &url) const {
    /*
     * -- sniffed from JS EventSource -- tcpdump -vvvs 1024 -l -A -i lo port 8001 -n
     * .^...^..GET /sub/subscribe/hokuto:20000/test/device/1/double_scalar HTTP/1.1
        Host: woody.elettra.eu:8001
        User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:76.0) Gecko/20100101 Firefox/76.0
        Accept: text/event-stream
        Accept-Language: en-US,en;q=0.5
        Accept-Encoding: gzip, deflate
        Origin: http://woody:8001
        DNT: 1
        Connection: keep-alive
        Referer: http://woody:8001/
        Pragma: no-cache
        Cache-Control: no-cache
    */
    QNetworkRequest r(url);
    r.setRawHeader("Accept", "text/event-stream");
    r.setRawHeader("Accept-Encoding", "gzip, deflate");
    r.setRawHeader("Connection", "keep-alive");
    r.setRawHeader("Pragma", "no-cache");
    r.setRawHeader("Cache-Control", "no-cache");

    r.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    r.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork); // Events shouldn't be cached
    return r;
}


// data from event source has a combination of fields, one per line
// (event, id, retry, data)
// https://developer.mozilla.org/en-US/docs/Web/API/Server-sent_events/Using_server-sent_events
// Here we extract data
//
QList<QByteArray> CuHttpChannelReceiver::m_extract_data(const QByteArray &in) const {
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
bool CuHttpChannelReceiver::m_likely_valid(const QByteArray &ba) const {
    return !ba.startsWith(": hi\n");
}

void CuHttpChannelReceiver::m_on_buf_complete() {
//    printf("\e[1;36mCuHTTPActionA::m_on_buf_complete: %s\e[0m\n", d->buf.data());
    if(m_likely_valid(d->buf)) {  // discard hi:
        QJsonParseError jpe;
        QList<QByteArray> jsonli = m_extract_data(d->buf);
        foreach(const QByteArray &json, jsonli) {
            QJsonDocument jsd = QJsonDocument::fromJson(json, &jpe);
            if(jsd.isNull())
                perr("%s: invalid json: %s\n", __PRETTY_FUNCTION__, qstoc(json));
            else {
                decodeMessage(jsd.array());
            }
        }
    }
}

void CuHttpChannelReceiver::onNewData() {
    QByteArray ba = d->reply->readAll();
    d->buf += ba;
    // buf complete?
    if(d->buf.endsWith("\n\n") || d->buf.endsWith("\r\n\r\n")) { // buf complete
        m_on_buf_complete();
        d->buf.clear();
    }
}

void CuHttpChannelReceiver::onReplyFinished() {
    reqs_ended++;
    d->reply->deleteLater();
}

void CuHttpChannelReceiver::onReplyDestroyed(QObject *) {
    d->reply = nullptr;
}

void CuHttpChannelReceiver::onSslErrors(const QList<QSslError> &errors) {
    QString msg;
    foreach(const QSslError &e, errors)
        msg += e.errorString() + "\n";
    decodeMessage(CumbiaHTTPWorld().make_error(msg));
}

void CuHttpChannelReceiver::onError(QNetworkReply::NetworkError code) {
    decodeMessage(CumbiaHTTPWorld().make_error(d->reply->errorString() + QString( "code %1").arg(code)));
}

void CuHttpChannelReceiver::cancelRequest() {
    if(d->reply && !d->reply->isFinished()) {
        disconnect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, nullptr);
        disconnect(d->reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, nullptr);
        disconnect(d->reply, SIGNAL(readyRead()), this, nullptr);
        pretty_pri("cancelling request { %s }\e[0m\n", qstoc(d->reply->request().url().toString()));
        d->reply->abort();
    }
}
