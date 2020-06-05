#include "cuhttpauthmanager.h"
#include "cuhttpauthmanagertools.h"
#include "cuhttpauthcookiestore.h"
#include "cumbiahttpworld.h"
#include <cumbia.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QCoreApplication>
#include <QUrlQuery>
#include <QtDebug>
#include <cumacros.h>
#include <cudata.h>

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QDialog>
#endif

class CuHttpAuthManagerPrivate {
public:
    QNetworkAccessManager *netman;
    QNetworkReply *reply;
    QUrl auth_url;
    CuHttpAuthCookieStore cookie_store;
};

CuHttpAuthManager::CuHttpAuthManager(QNetworkAccessManager *netman, QObject *parent) : QObject(parent) {
    d = new CuHttpAuthManagerPrivate;
    d->netman = netman;
    d->reply = nullptr;
}

CuHttpAuthManager::~CuHttpAuthManager() {
    delete d;
}

void CuHttpAuthManager::tryAuthorize(const QString &user,
                                     const QString &pass) {
    QNetworkRequest r;
    r.setRawHeader("Accept", "application/json");
    r.setHeader(QNetworkRequest::UserAgentHeader, QByteArray("cumbia-http ") + QByteArray(CUMBIA_HTTP_VERSION_STR));
    QUrlQuery q;
    q.addQueryItem("username", user);
    q.addQueryItem("elettra_ldap_password", pass);
    d->auth_url.setQuery(q);
    r.setUrl(d->auth_url);
    qDebug () << __PRETTY_FUNCTION__ << user << pass << d->auth_url.toString();
    if(!d->reply) {
        d->reply = d->netman->post(r, q.toString(QUrl::FullyEncoded).toUtf8());
        d->reply->setProperty("user", user);
        connect(d->reply, SIGNAL(readyRead()), this, SLOT(onNewData()));
        connect(d->reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
        connect(d->reply, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(onSslErrors(const QList<QSslError> &)));
        connect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        connect(d->reply, SIGNAL(destroyed(QObject *)), this, SLOT(onReplyDestroyed(QObject *)));
        connect(d->reply, SIGNAL(encrypted()), this, SLOT(onReplyEncrypted()));
    }
    else {
        perr("CuHttpAuthManager::startRequest: error { already in progress }");
    }
}

QByteArray CuHttpAuthManager::getCookie() const {
    return d->cookie_store.get();
}

void CuHttpAuthManager::authPrompt(const QString &auth_url, bool cli)
{
    d->auth_url = auth_url;
    CuHttpAuthInputFactory auif;
    CuHttpAuthPrompt_I *aup = auif.create_prompt(cli, nullptr);
    connect(aup->qobj(), SIGNAL(onCredsReady(QString, QString)), this, SLOT(onCredsReady(QString, QString)));
    aup->getCreds();
}

void CuHttpAuthManager::onNewData() {
}

void CuHttpAuthManager::onReplyFinished()
{
    QByteArray ba = d->reply->readAll();
    QString msg;
    qDebug() << __PRETTY_FUNCTION__ << ba;
    bool authorised = !ba.startsWith("login failed");
    if(authorised) {
        QByteArray cookie = d->reply->rawHeader("Set-Cookie");
        if(!cookie.isEmpty()) {
            d->cookie_store.save(cookie);
        }
        else {
            msg = "CuHttpAuthManager: did not receive the cookie";
        }
    }
    else
        msg = "CuHttpAuthManager: login failed";
    d->reply->deleteLater();
    emit authReply(authorised, d->reply->property("user").toString(), msg, d->reply->property("ssl").toBool());
}

void CuHttpAuthManager::onSslErrors(const QList<QSslError> &errors) {
    QString msg;
    foreach(const QSslError &e, errors)
        msg += e.errorString() + "\n";
    emit error(msg);
}

void CuHttpAuthManager::onError(QNetworkReply::NetworkError e) {
    emit error(d->reply->errorString() + QString( "code %1").arg(e));
}

void CuHttpAuthManager::onReplyDestroyed(QObject * o) {
    qDebug()  <<__PRETTY_FUNCTION__ << o << "destroyed";
    d->reply = nullptr;
}

void CuHttpAuthManager::onReplyEncrypted()
{
    d->reply->setProperty("ssl", true);
}

void CuHttpAuthManager::onCredsReady(const QString &user, const QString &passwd) {
    emit credentials(user, passwd);
}

CuData CuHttpAuthManager::m_make_error_data(const QString &msg) const {
    CuData ed("err", true);
    ed.set("msg", msg.toStdString()).set("src", "auth_manager").putTimestamp();
    if(d->reply)
        ed.set("encrypted", d->reply->property("ssl").toBool());
    return ed;
}


