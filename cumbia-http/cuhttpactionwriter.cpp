#include "cumbiahttpworld.h"
#include "cuhttpactionreader.h"
#include "cuhttpactionwriter.h"
#include "cuhttp_source.h"
#include "cuhttpauthmanager.h"
#include "cumbiahttpworld.h"

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include "cuhttpprotocolhelpers.h"
#include "cuhttpprotocolhelper_i.h"
#include "cuhttp_source.h"
#include <set>

#include <cudatalistener.h>
#include <qustring.h>

class CuHTTPActionWriterPrivate
{
public:
    CuHTTPActionWriterPrivate() : exit(false), nam(nullptr),
        proto_helper_i(nullptr), proto_helpers(nullptr) {

    }

    std::set<CuDataListener *> listeners;
    QString http_target, prepared_http_target;
    QString url;
    bool exit;
    CuData options;
    QNetworkAccessManager *nam;
    ProtocolHelper_I *proto_helper_i;
    CuHttpProtocolHelpers *proto_helpers;
    CuVariant w_val;
    CuHttpAuthManager *auth_manager;
};

CuHttpActionWriter::CuHttpActionWriter(const CuHTTPSrc &target,
                                       QNetworkAccessManager *qnam,
                                       const QString &http_url,
                                       CuHttpAuthManager *authman)
    : CuHTTPActionA(qnam)
{
    d = new CuHTTPActionWriterPrivate;
    d->nam = qnam;
    d->url = http_url;
    d->http_target = QString::fromStdString(target.getName());
    d->auth_manager = authman;
    d->prepared_http_target = QString::fromStdString(target.prepare());
    connect(d->auth_manager, SIGNAL(credentials(QString, QString)), this, SLOT(onCredentials(QString, QString)));
    connect(d->auth_manager, SIGNAL(authReply(bool, QString, QString, bool)), this, SLOT(onAuthReply(bool, QString, QString, bool)));
    connect(d->auth_manager, SIGNAL(error(const QString&)), this, SLOT(onAuthError(const QString&)));
}

CuHttpActionWriter::~CuHttpActionWriter()
{
    pdelete("~CuHttpActionWriter \"%s\" %p", qstoc(d->http_target), this);
    delete d;
}

void CuHttpActionWriter::setWriteValue(const CuVariant &w) {
    d->w_val = w;
}

void CuHttpActionWriter::setConfiguration(const CuData &co) {
    d->options = co;
}

void CuHttpActionWriter::onNetworkReplyFinished(QNetworkReply *) {

}

void CuHttpActionWriter::onCredentials(const QString &user, const QString &passwd) {
    qDebug() << __PRETTY_FUNCTION__ << user << passwd;
    if(user.isEmpty()) {
        m_notify_result(m_make_error_data("invalid user name").set("is_result", true));
        notifyActionFinished(); // unregister action
    }
    else {
        d->auth_manager->tryAuthorize(user, passwd);
    }

}

QString CuHttpActionWriter::getSourceName() const {
    return d->http_target;
}

CuHTTPActionA::Type CuHttpActionWriter::getType() const {
    return CuHTTPActionA::Writer;
}

void CuHttpActionWriter::addDataListener(CuDataListener *l) {
    d->listeners.insert(l);
}

void CuHttpActionWriter::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuHttpActionWriter::dataListenersCount() {
    return d->listeners.size();
}

void CuHttpActionWriter::start() {
    QString src;
    src = QString("/x/write/%1").arg(d->prepared_http_target.isEmpty() ? d->http_target : d->prepared_http_target);
    if(d->w_val.isValid())
        src += QString("(%1)").arg(d->w_val.toString().c_str());
    startRequest(d->url + src);
}

bool CuHttpActionWriter::exiting() const {
    return d->exit;
}

void CuHttpActionWriter::stop() {
    d->exit = true;
    d->listeners.clear();
}

void CuHttpActionWriter::decodeMessage(const QJsonValue &jsv) {
    CuData res("src", d->http_target.toStdString());
    CumbiaHTTPWorld httpw;
    httpw.json_decode(jsv, res);
    if(res["err"].toBool() && !res["authorized"].toBool()) {
        // manage authorization. Get a dialog to insert user and password. Pass the auth url
        d->auth_manager->authPrompt(QuString(res, "auth_url"), false); // false: use dialog not cli
    }
    else {
        d->exit = res["is_result"].toBool();
        if(res["err"].toBool() || res["is_result"].toBool())
            m_notify_result(res);
    }
    if(d->exit) d->listeners.clear();
}

void CuHttpActionWriter::notifyActionFinished() {
    stop(); // need d->exit true
    CuHTTPActionA::notifyActionFinished();
}

QNetworkRequest CuHttpActionWriter::prepareRequest(const QUrl &url) const {
    QNetworkRequest req(url);
    QByteArray ba = d->auth_manager->getCookie();
    qDebug() << __PRETTY_FUNCTION__ << "COOKIE " << ba;
    if(!ba.isEmpty())
        req.setRawHeader("Cookie", ba);
    return req;
}

void CuHttpActionWriter::onAuthReply(bool authorised, const QString &user, const QString &message, bool encrypted) {
    qDebug() << __PRETTY_FUNCTION__ << authorised << user << message << encrypted;
    if(authorised) start();
    else {
        m_notify_result(m_make_error_data("not authorised").set("is_result", true));
        notifyActionFinished(); // unregister action
    }
}

void CuHttpActionWriter::onAuthError(const QString &errm) {
    qDebug() << __PRETTY_FUNCTION__ << errm;
    m_notify_result(m_make_error_data(errm).set("is_result", true));
    notifyActionFinished(); // unregister action
}

CuData CuHttpActionWriter::m_make_error_data(const QString &msg) {
    CuData da("msg", msg.toStdString());
    da.set("err", true).set("src", d->http_target.toStdString()).putTimestamp();
    return da;
}

void CuHttpActionWriter::m_notify_result(const CuData &res) {
    for(std::set<CuDataListener *>::iterator it = d->listeners.begin(); it != d->listeners.end(); ++it) {
        (*it)->onUpdate(res);
    }
}
