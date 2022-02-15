#include "cuhttpbundledsrcreq.h"
#include "cuhttpactionfactoryi.h"
#include "cumbiahttpworld.h"
#include <cudata.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

// debug
#include <QEventLoop>
#include <QFile>
#include <QtDebug>
#include <qtimer.h>
#include <qustringlist.h>

class CuHttpBundledSrcReqPrivate {
public:
    CuHttpBundledSrcReqPrivate(const QList<SrcItem>& srcs, unsigned long long cli_id) {
        req_payload = m_json_pack(srcs, cli_id);
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
        req_payload = m_json_pack(il, 0);
    }

    QByteArray buf;
    QByteArray req_payload, cookie, channel;
    QByteArray m_json_pack(const QList<SrcItem>& srcs, unsigned long long client_id);
    CuHttpBundledSrcReqListener *listener;
    bool blocking;


    ///
    /// TEST
    ///
    QTimer *timer;
    QNetworkReply *reply;
    unsigned usecnt = 0;
    QFile *f;
    QTextStream *out;
};

CuHttpBundledSrcReq::CuHttpBundledSrcReq(const QList<SrcItem> &srcs,
                                         CuHttpBundledSrcReqListener *l, unsigned long long client_id,
                                         QObject *parent) : QObject(parent) {
    d = new CuHttpBundledSrcReqPrivate(srcs, client_id);
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
    d->buf.clear();
    d->usecnt++;
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

    d->timer = new QTimer(this);
    d->timer->setInterval(2000);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(m_test_check_reply()));
    d->timer->start();

    /// TEST
    ///
    d->reply = reply;
    printf("\e[1;31m*\e[0m %s CuHttpBundledSrcReq %p started: (rq. use cnt: %d)\n", qstoc(objectName()),
           this, /*d->req_payload.toStdString().c_str(),*/ d->usecnt);
    d->f = new QFile("/tmp/sequencer/" + objectName() + ".log", this);
    d->f->open(QIODevice::Text|QIODevice::WriteOnly);
    d->out = new QTextStream (d->f);
    *d->out << "REQ. " << objectName() << this << ":\n" << d->req_payload << "\n";
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
//        d->buf.clear(); buf cleared in start
    }
}

void CuHttpBundledSrcReq::onReplyFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << this << "deleting reply " << sender() << " later";
    if(d->buf.length() < 300) {
        printf("\e[1;31m*\e[0m %s CuHttpBundledSrcReq %p short reply: \"\e[1;31m%s\e[0m\" (rq. use cnt: %d)\n"
               , qstoc(objectName()),
               this, d->req_payload.toStdString().c_str(), d->usecnt);

        printf("\nHEADER\n-----------------------------------");
        foreach(const QByteArray& ba, d->reply->rawHeaderList() )
            printf("%s\n", ba.data());
        printf("\n--------------------------------------------\n");
    }
    *d->out << "REP. " << objectName() << this << ":\n" << d->buf << "\n";
    sender()->deleteLater();
    d->reply = nullptr;
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

void CuHttpBundledSrcReq::onError(QNetworkReply::NetworkError code) {
    QNetworkReply *r = qobject_cast<QNetworkReply *>(sender());
    CuData da("msg", r->errorString().toStdString());
    da.set("err", true);
    da.set("data", d->buf.toStdString());
    da.set("payload", r->property("payload").toString().toStdString());
    d->listener->onSrcBundleReplyError(da);
    printf("\e[1;31m*\e[0m %s CuHttpBundledSrcReq %p \e[1;31mCuHttpBundledSrcReq::onError: %s request was \e[0;31m%s\e[0m\n",
           qstoc(objectName()), this, qstoc(r->errorString()), qstoc(r->property("payload").toString()));

    *d->out << "ERR REP. " << objectName() << this << ": buf\n" << d->buf << "\n err: " << r->errorString();
}

void CuHttpBundledSrcReq::m_test_check_reply() {
    if(d->reply)
        printf("\e[1;31m*\e[0m %s CuHttpBundledSrcReq %p: \e[1;35mstill waiting for reply\e[0m (req use cnt: %d)\n",
               qstoc(objectName()),
               this, d->usecnt);
    else  {
        printf("\e[1;31m*\e[0m %s CuHttpBundledSrcReq %p finished (req use cnt: %d)\n",
               qstoc(objectName()), this, d->usecnt);
        if(strlen(d->buf.data()) < 300)
            printf("\e[1;31m*\e[0m %s CuHttpBundledSrcReq %p quite a short reply len %ld: >>> \e[1;35m%s\e[0m <<<\n", qstoc(objectName()), this,
                   strlen(d->buf.data()), d->buf.data());
        d->timer->stop();
        delete d->out;
        d->f->close();
        delete d->f;
    }
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

QByteArray CuHttpBundledSrcReqPrivate::m_json_pack(const QList<SrcItem> &srcs, unsigned long long client_id)
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
    if(client_id > 0)
        root_o["id"] = QString::number(client_id);
    root_o["srcs"] = sa;
    QJsonDocument doc(root_o);
    return doc.toJson(QJsonDocument::Compact);
}
