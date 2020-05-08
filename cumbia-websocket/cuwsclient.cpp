#include "cuwsclient.h"
#include <cumacros.h>
#include <QtDebug>
#include <QAbstractSocket>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <cudata.h>

/// TEST
#include <QThread>
#include <QTimer>

class CuWSClientPrivate {
public:
    QWebSocket m_webSocket;
    QUrl m_url;
    CuWSClientListener *m_listener;
    QQueue<QString> m_out_msg_queue, m_in_msg_queue;
    bool m_socket_open;
    QTimer *m_tmr_queue;
};

CuWSClient::CuWSClient(const QUrl &url, CuWSClientListener * listener, QObject *parent) : QObject(parent)
{
    d = new CuWSClientPrivate;
    d->m_url = url;
    d->m_listener = listener;
    d->m_socket_open = false;
    connect(&d->m_webSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&d->m_webSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&d->m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(&d->m_webSocket, SIGNAL(textMessageReceived(const QString&)), this, SLOT(onMessageReceived(const QString&)));
}

CuWSClient::~CuWSClient()
{
    pdelete("~CuWSClient %p", this);
    delete d;
    close();
}

bool CuWSClient::isOpen() const {
    return d->m_webSocket.isValid();
}

/*! \brief shortcut for close. Neeeded by QmlAppStateManager to close the socket when
 *         the application becomes inactive/hidden/suspended
 *
 * @see close
 */
void CuWSClient::suspend() {
    close();
}

/*! \brief shortcut for open. Neeeded by QmlAppStateManager to re open the socket
 *         after the application is active again
 *
 * @see open
 */
void CuWSClient::start() {
    open();
}

void CuWSClient::open()
{
    pviolet2tmp("+ opening web socket %s [CuWSClient.open]", qstoc(d->m_url.toString()));
    d->m_webSocket.open(d->m_url);
}

void CuWSClient::close()
{
    pviolet2tmp("X closing web socket %s [CuWSClient.close]", qstoc(d->m_url.toString()));
    d->m_webSocket.close();
}

void CuWSClient::sendMessage(const QString &msg) {
    if(d->m_webSocket.isValid())
        d->m_webSocket.sendTextMessage(msg);
    else
        d->m_out_msg_queue.enqueue(msg);
}

void CuWSClient::onConnected()
{
    pviolet2tmp("-*- connected to %s [CuWSClient.onConnected]", qstoc(d->m_url.toString()));
    while(!d->m_out_msg_queue.isEmpty() && d->m_webSocket.isValid())
        d->m_webSocket.sendTextMessage(d->m_out_msg_queue.dequeue());
    /// TEST
    d->m_tmr_queue = new QTimer(this);
    d->m_tmr_queue->setInterval(1000);
    connect(d->m_tmr_queue, SIGNAL(timeout()), this, SLOT(deliverMsgs()));
    d->m_tmr_queue->start();
}

void CuWSClient::onDisconnected()
{
    pviolet2tmp("-/- disconnected from %s [CuWSClient.onDisconnected]", qstoc(d->m_url.toString()));
}

void CuWSClient::onMessageReceived(const QString &message)
{
    d->m_in_msg_queue.enqueue(message);
}

void CuWSClient::onSocketError(QAbstractSocket::SocketError se)
{
    perr("socket error: %s [err: %d] URL: %s [CuWSClient.onSocketError]", qstoc(d->m_webSocket.errorString()), se, qstoc(d->m_url.toString()));
}

void CuWSClient::deliverMsgs() {
    // listener is CumbiaWebSocket
    while(!d->m_in_msg_queue.isEmpty())
        d->m_listener->onUpdate(d->m_in_msg_queue.dequeue());
}
