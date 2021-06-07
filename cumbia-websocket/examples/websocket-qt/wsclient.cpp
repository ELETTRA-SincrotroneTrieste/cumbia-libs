
#include "wsclient.h"
#include <QtCore/QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonValue>

QT_USE_NAMESPACE

WsClient::WsClient(const QUrl &wsurl, const QStringList &src_urls, QObject *parent) :
    QObject(parent),
    m_url(wsurl)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &WsClient::onConnected);
    connect(&m_webSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));

    // For your "Content-Length" header
    m_webSocket.open(m_url);

    foreach(QString url, src_urls)
        m_src_urls << QUrl(url);

}

WsClient::~WsClient()
{
    printf("\e[1;31m ~ \e[0mWsClient\n");
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    foreach(QUrl url, m_src_urls) {
        QNetworkRequest request(url);
    //    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        printf("\e[1;35m- - \e[0munsubscribing \"%s\"...\n", url.toString().toStdString().c_str());
        nam->sendCustomRequest(request, QByteArray("UNSUBSCRIBE"));
    }
    m_webSocket.close();
}

void WsClient::onConnected()
{
    printf("\n\e[0;32m -*- websocket connected to \"%s\"\e[0m\n\n", m_url.toString().toStdString().c_str());
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &WsClient::onTextMessageReceived);

    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    foreach(QUrl url, m_src_urls) {
        QNetworkRequest request(url);
    //    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        nam->sendCustomRequest(request, QByteArray("SUBSCRIBE"));
    }
}

void WsClient::onTextMessageReceived(QString message)
{
    QString src = m_getSrc(message.toUtf8());
      printf("\e[1;32;4mWEBSOCKET\e[0m [\e[1;33m%s: \e[1;36m%s\e[0m] \e[1;36m+---->\e[0m %s\n",
             QDateTime::currentDateTime().time().toString().toStdString().c_str(),
             src.toStdString().c_str(),
             message.toStdString().c_str());
}

void WsClient::onSocketError(QAbstractSocket::SocketError e)
{
    printf("\e[1;31msocket error\e[0m: \e[0;31m{\e[0m%s\e[0;31m} (%d)\e[0m\n",
           m_webSocket.errorString().toStdString().c_str(), e);
}

void WsClient::onSocketDisconnected()
{
    printf("\n\e[0;32m -/- websocket disconnected from \"%s\"\e[0m\n\n", m_url.toString().toStdString().c_str());
}

void WsClient::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
        printf("\e[1;31mhttp reply error\e[0m: \e[0;31m{\e[0m%s\e[0;31m}\e[0m\n", reply->errorString().toStdString().c_str());
    else {
        QByteArray dba = reply->readAll();
        QString src = m_getSrc(dba);
        printf("    \e[1;36;4mHTTP\e[0m [\e[1;33m%s: \e[1;36m%s\e[0m] \e[1;36m+---->\e[0m %s\n",
               QDateTime::currentDateTime().time().toString().toStdString().c_str(),
               src.toStdString().c_str(),
               dba.data());
    }
}

QString WsClient::m_getSrc(const QByteArray &jsba)
{
    QString src;
    QJsonDocument d = QJsonDocument::fromJson(jsba);
    if(!d.isNull())
        src = d["E"].toString();
    return src;
}
