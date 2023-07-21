#include "cuhttpchannelreceiver.h"
#include <cumacros.h>
#include "cumbiahttpworld.h"
#include <cudatalistener.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMultiMap>
#include <QNetworkAccessManager>
#include <QEventLoop>

#include <QWebSocket>
#include <QAbstractSocket>
#include <cudata.h>
#include <cudataserializer.h>

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

    // web socket
    QWebSocket ws;
};

CuHttpChannelReceiver::CuHttpChannelReceiver(const QString &url, const QString &chan, QNetworkAccessManager *nam) {
    d = new CuHttpChannelReceiverPrivate(url, chan, nam);
}

QString CuHttpChannelReceiver::channel() const {
    return d->chan;
}

QString CuHttpChannelReceiver::url() const {
    QString channel = d->url + "/sub/" + d->chan;
    return channel;
}

QString CuHttpChannelReceiver::ws_url() const {
    QString u(d->url);
    u.replace("https://", "wss://");
    u.replace("http://", "ws://");
    return u + "/sub/" + d->chan + "/ws";
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
        d->reply->setProperty("name", "subscribe reply");
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
    // setup web socket
    connect(&d->ws, &QWebSocket::connected, this, &CuHttpChannelReceiver::onWsConnected);
    connect(&d->ws, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onWsError(QAbstractSocket::SocketError)));
    connect(&d->ws, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
            this, &CuHttpChannelReceiver::onSslErrors);
    d->ws.open(QUrl(ws_url()));
}

void CuHttpChannelReceiver::stop() {
    qDebug() << __PRETTY_FUNCTION__ << "stop request : d->reply" << d->reply;
    if(d->reply) {
        disconnect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, nullptr);
        disconnect(d->reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, nullptr);
        if(d->reply->isOpen())
            d->reply->close();
    }
    if(d->ws.isValid())
        d->ws.close();
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
    //    r.setRawHeader("Connection", "keep-alive");
    //    r.setRawHeader("Pragma", "no-cache");
    //    r.setRawHeader("Cache-Control", "no-cache");

    //    r.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    //    r.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork); // Events shouldn't be cached
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
    if(m_likely_valid(d->buf)) {  // discard hi:
        QJsonParseError jpe;
        QList<QByteArray> jsonli = m_extract_data(d->buf);
        foreach(const QByteArray &json, jsonli) {
            QJsonDocument jsd = QJsonDocument::fromJson(json, &jpe);
            if(jsd.isNull())
                perr("%s: invalid json: %s: %s\n", __PRETTY_FUNCTION__, qstoc(json), qstoc(jpe.errorString()));
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

void CuHttpChannelReceiver::onWsMessageReceived(const QString &m) {
    printf("[ \e[1;32mWS\e[0m ]: %s\n", qstoc(m));
}

void CuHttpChannelReceiver::onWsBinaryMessageReceived(const QByteArray &ba) {
    CuDataSerializer s;
    uint32_t siz = s.size(ba.data());
    CuData da = s.deserialize(ba.data(), siz); // siz for preliminary data len checks
    ///
    /// TEST
    const CuVariant &v = da["value"];
    printf("[ \e[1;36mWS\e[0m ]: BINARY MESSAGE len \e[1;35m%d\e[0m \e[1;37;3m%s\e[0m (type %s fmt %s len %ld)\n", siz, datos(da), v.dataTypeStr(v.getType()).c_str(),
           v.dataFormatStr(v.getFormat()).c_str(), v.getSize());
    /// end test
    ///
    foreach(CuDataListener *l, d->rmap.values(da.s("src").c_str())) {
        l->onUpdate(da);
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
    QString msg("ssl error while connecting to " + d->url);
    foreach(const QSslError &e, errors)
        msg += e.errorString() + "\n";
    decodeMessage(CumbiaHTTPWorld().make_error(msg));
}

void CuHttpChannelReceiver::onWsConnected() {
    connect(&d->ws, &QWebSocket::textMessageReceived, this, &CuHttpChannelReceiver::onWsMessageReceived);
    connect(&d->ws, &QWebSocket::binaryMessageReceived, this, &CuHttpChannelReceiver::onWsBinaryMessageReceived);
}

void CuHttpChannelReceiver::onWsSslErrors(const QList<QSslError> &errors) {
    QString msg("ssl error while connecting websocket to " + d->url);
    foreach(const QSslError &e, errors)
        msg += e.errorString() + "\n";
    perr("%s", qstoc(msg));
    decodeMessage(CumbiaHTTPWorld().make_error(msg));
}

void CuHttpChannelReceiver::onWsError(QAbstractSocket::SocketError socketError) {
    QString msg = QString("error opening websocket: %1: %2").arg(socketError).arg(qobject_cast<QWebSocket *>(sender())->errorString());
    perr("%s", qstoc(msg));
    decodeMessage(CumbiaHTTPWorld().make_error(msg));
}

void CuHttpChannelReceiver::onError(QNetworkReply::NetworkError code) {
    decodeMessage(CumbiaHTTPWorld().make_error(d->reply->errorString() + QString( "code %1").arg(code)));
    perr("CuHttpChannelReceiver::onError: %s", qstoc(d->reply->errorString()));
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
