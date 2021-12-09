#include "cuhttpcliidman.h"
#include "cumbiahttpworld.h"

#include <cumacros.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkRequest>
#include <cudata.h>
#include <QTimer>

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
            bool ok = m_get_id_and_ttl() && d->id > 0 && d->ttl > 0; // needs d->buf. d->buf cleared in start
            d->lis->onIdReady(d->id, d->ttl);
            if(ok) {
                printf("CuHttpCliIdMan::onNewData: \e[1;32m ID %llu ttl %lu\e[0m: starting keepalive...\n", d->id, d->ttl);
                m_start_keepalive();
            }
            else
                d->lis->onIdManError(d->error);
        }
    }
    else { // result from keepalive request
        d->bufs[1] += ba;
        printf("CuHttpCliIdMan::onNewData: \e[0;32m keepalive reply: \e[0;33m%s\e[0m\n", d->bufs[1].data());
    }
}

void CuHttpCliIdMan::onReplyFinished() {
    sender()->deleteLater();
}

void CuHttpCliIdMan::onReplyDestroyed(QObject *o) {
    printf("CuHttpCliIdMan::onReplyDestroyed: reply deleted %p\n", o);
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
    perr("CuHttpCliIdMan::onError: error: %s", qstoc(d->error));
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    d->error = r->errorString();
    m_notify_err(r->property("type").toString() == "id_request");
}

void CuHttpCliIdMan::send_keepalive() {
    d->bufs[1].clear();
    QNetworkRequest r(d->url);
    m_make_network_request(&r);
    // curl http://woody.elettra.eu:8001/bu/tok
    printf("CuHttpCliIdMan::send_keepalive: Sending keepalive id %llu\n", d->id);
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

bool CuHttpCliIdMan::m_get_id_and_ttl() {
    CumbiaHTTPWorld w;
    CuData out;
    bool ok = w.json_simple_decode(d->bufs[0], out);
    if(!ok)
        d->error = out.s("msg").c_str();
    else {
        // [{"exp":1638978149,"id":35,"ttl":10}]
        out["id"].to<unsigned long long>(d->id);
        out["ttl"].to<unsigned long>(d->ttl);
        d->ttl *= 1000; // ttl from server is in seconds
    }
    return ok;
}

void CuHttpCliIdMan::m_start_keepalive() {
    QTimer *t = findChild<QTimer*> ("keepalive_tmr");
    if(!t) {
        t = new QTimer(this);
        t->setObjectName("keepalive_tmr");
        t->setInterval(0.95 * d->ttl);
        connect(t, SIGNAL(timeout()), this, SLOT(send_keepalive()));
        t->start();
    }
}

void CuHttpCliIdMan::m_reply_connect(QNetworkReply *reply) {
    connect(reply, SIGNAL(readyRead()), this, SLOT(onNewData()));
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(destroyed(QObject*)), this, SLOT(onReplyDestroyed(QObject*)));
}

void CuHttpCliIdMan::m_make_network_request(QNetworkRequest *r) const {
    r->setRawHeader("Accept", "application/json");
    r->setRawHeader("Content-Type", "application/json");
    r->setHeader(QNetworkRequest::UserAgentHeader, QByteArray("cumbia-http ") + QByteArray(CUMBIA_HTTP_VERSION_STR));
}

void CuHttpCliIdMan::m_notify_err(bool is_id_req) {
    if(is_id_req)
        d->lis->onIdReady(0, 0);
    d->lis->onIdManError(d->error);
}

