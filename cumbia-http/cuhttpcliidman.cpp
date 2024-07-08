#include "cuhttpcliidman.h"
#include "cumbiahttpworld.h"

#include <cumacros.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkRequest>
#include <cudata.h>
#include <QTimer>
#include <QEventLoop>

class CuHttpCliIdManPrivate {
public:
    CuHttpCliIdManPrivate(QNetworkAccessManager *nm, const QString& u, CuHttpCliIdManListener *l)
        : nam(nm), url(u), id(0), ttl(0), lis(l) {}
    QNetworkAccessManager* nam;
    const QString url;
    unsigned long long id;
    unsigned long ttl;
    QByteArray bufs[2];
    QString error;
    CuHttpCliIdManListener* lis;
};

CuHttpCliIdMan::CuHttpCliIdMan(const QString& url, QNetworkAccessManager *nm, CuHttpCliIdManListener *li)
    : QObject{nullptr} {
    d = new CuHttpCliIdManPrivate(nm, url, li);
}

CuHttpCliIdMan::~CuHttpCliIdMan() {
    delete d;
}

void CuHttpCliIdMan::start()
{
    d->bufs[0].clear();
    QNetworkRequest r(d->url);
    m_make_network_request(&r);
    // curl http://woody.elettra.eu:8001/bu/tok
    QNetworkReply *reply = d->nam->post(r, QByteArray());
    reply->setProperty("type", "id_request");
    m_reply_connect(reply);
}

void CuHttpCliIdMan::unsubscribe(bool blocking) {
    m_stop_keepalive();
    QNetworkRequest r(d->url);
    m_make_network_request(&r);
    // curl http://woody.elettra.eu:8001/bu/tok
    printf("CuHttpCliIdMan.unsubscribe: posting \e[1;35m%s\e[0m to \e[1;32m%s\e[0m\n",
           m_json_unsub().data(), qstoc(d->url));
    QNetworkReply *reply = d->nam->post(r, m_json_unsub());
    if(blocking) {
        // do not m_reply_connect(reply): we block and then
        /// query error conditions and read reply
        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        QNetworkReply::NetworkError err = reply->error();
        if(err != QNetworkReply::NoError)
            perr("cuhttp-cli-id-man: network reply error: %s", qstoc(reply->errorString()));
        QByteArray ba = reply->readAll();
        cuprintf("cuhttp-cli-id-man:unsubscribe --> \"%s\"\n", ba.data());
    }
    else
        m_reply_connect(reply);
}

QString CuHttpCliIdMan::error() const {
    return d->error;
}

void CuHttpCliIdMan::onNewData()
{
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    int clen = r->header(QNetworkRequest::ContentLengthHeader).toInt();
    qint64 bytes_avail = r->bytesAvailable();
    QByteArray ba = r->read(bytes_avail);
    if(r->property("type").toString() == "id_request") {
        d->bufs[0] += ba;
        // buf complete?
        if(d->bufs[0].length() == clen) { // buf complete
            cuprintf("\e[1;32mCuHttpCliIdMan.onNewData: received buf %s\e[0m\n", d->bufs[0].data());
            bool ok = m_get_id_and_ttl() && d->id > 0 && d->ttl > 0; // needs d->buf. d->buf cleared in start
            d->lis->onIdReady(d->id, d->ttl);
            if(ok) {
                m_start_keepalive();
            }
            else
                d->lis->onIdManError(d->error);
        }
    }
    else { // result from keepalive request
        //        d->bufs[1] += ba;
        //        printf("CuHttpCliIdMan::onNewData: \e[0;32m keepalive reply: \e[0;33m%s\e[0m\n", d->bufs[1].data());
    }
}

void CuHttpCliIdMan::onReplyFinished() {
    sender()->deleteLater();
}

void CuHttpCliIdMan::onReplyDestroyed(QObject *o) {
}

void CuHttpCliIdMan::onSslErrors(const QList<QSslError> &errors) {
    perr("CuHttpCliIdMan::onSslErrors: errors: %s", qstoc(d->error));
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    d->error.clear();
    foreach(const QSslError &e, errors)
        d->error += e.errorString() + "\n";
    m_notify_err(r->property("type").toString() == "id_request");
}

void CuHttpCliIdMan::onError(QNetworkReply::NetworkError ) {
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    d->error = r->errorString();
    m_notify_err(r->property("type").toString() == "id_request");
    QByteArray b = r->readAll();
    perr("CuHttpCliIdMan::onError: error: %s: data: %s", qstoc(d->error), b.data());
}

void CuHttpCliIdMan::send_keepalive() {
    d->bufs[1].clear();
    QNetworkRequest r(d->url);
    m_make_network_request(&r);
    // curl http://woody.elettra.eu:8001/bu/tok
    QNetworkReply *reply = d->nam->post(r, m_json(d->id));
    reply->setProperty("type", "id_renew");
    m_reply_connect(reply);
}

QByteArray CuHttpCliIdMan::m_json(unsigned long long id) const {
    //  curl http://woody.elettra.eu:8001/bu/tok -d $'{"id":"30"}'
    QJsonObject root_o;
    root_o["id"] = QString::number(id);
    QJsonDocument doc(root_o);
    return doc.toJson(QJsonDocument::Compact);
}

QByteArray CuHttpCliIdMan::m_json_unsub() const {
    //  curl http://woody.elettra.eu:8001/bu/tok -d $'{"id":"30", "method":"u" }'
    QJsonObject root_o;
    root_o["id"] = QString::number(d->id);
    root_o["method"] = "u";
    QJsonDocument doc(root_o);
    return doc.toJson(QJsonDocument::Compact);
}

bool CuHttpCliIdMan::m_get_id_and_ttl() {
    CumbiaHTTPWorld w;
    CuData out;
    bool ok = w.json_simple_decode(d->bufs[0], out);
    if(!ok)
        d->error = out.s(TTT::Message).c_str();  // out.s("msg")
    else {
        // [{"exp":1638978149,"id":35,"ttl":10}]
        out["id"].to<unsigned long long>(d->id);
        out["ttl"].to<unsigned long>(d->ttl);
        d->ttl *= 1000 * 0.95; // ttl from server is in seconds
    }
    return ok;
}

void CuHttpCliIdMan::m_start_keepalive() {
    QTimer *t = findChild<QTimer*> ("keepalive_tmr");
    if(!t) {
        t = new QTimer(this);
        t->setObjectName("keepalive_tmr");
        t->setInterval(d->ttl); // interval in seconds from m_get_id_and_ttl
        connect(t, SIGNAL(timeout()), this, SLOT(send_keepalive()));
        t->start();
    }
}

void CuHttpCliIdMan::m_stop_keepalive() {
    QTimer *t = findChild<QTimer*> ("keepalive_tmr");
    if(t) t->stop();
}

void CuHttpCliIdMan::m_reply_connect(QNetworkReply *reply) {
    connect(reply, SIGNAL(readyRead()), this, SLOT(onNewData()));
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
#else
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
#endif
    connect(reply, SIGNAL(destroyed(QObject*)), this, SLOT(onReplyDestroyed(QObject*)));
}

void CuHttpCliIdMan::m_make_network_request(QNetworkRequest *r) const {
    r->setRawHeader("Accept", "application/json");
    r->setRawHeader("Content-Type", "application/json");
    r->setRawHeader("Connection", "Close");
    r->setHeader(QNetworkRequest::UserAgentHeader, QByteArray("cumbia-http ") + QByteArray(CUMBIA_HTTP_VERSION_STR));
}

void CuHttpCliIdMan::m_notify_err(bool is_id_req) {
    if(is_id_req)
        d->lis->onIdReady(0, 0);
    d->lis->onIdManError(d->error);
}

