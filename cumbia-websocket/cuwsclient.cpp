#include "cuwsclient.h"
#include <cumacros.h>
#include <QtDebug>
#include <QAbstractSocket>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <cudata.h>

CuWSClient::CuWSClient(const QUrl &url, CuWSClientListener * listener, QObject *parent) : QObject(parent)
{
    m_url = url;
    m_listener = listener;
    m_socket_open = false;
    connect(&m_webSocket, &QWebSocket::connected, this, &CuWSClient::onConnected);
    connect(&m_webSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &CuWSClient::onMessageReceived);
}

CuWSClient::~CuWSClient()
{
    pdelete("~CuWSClient %p", this);
    close();
}

bool CuWSClient::isOpen() const {
    return m_webSocket.isValid();
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
    pviolet2tmp("+ opening web socket %s", qstoc(m_url.toString()));
    m_webSocket.open(m_url);
}

void CuWSClient::close()
{
    pviolet2tmp("X closing web socket %s", qstoc(m_url.toString()));
    m_webSocket.close();
}

void CuWSClient::sendMessage(const QString &msg) {
    if(m_webSocket.isValid())
        m_webSocket.sendTextMessage(msg);
    else
        m_msg_queue.enqueue(msg);
}

void CuWSClient::onConnected()
{
    pviolet2tmp("-*- CuWSClient: connected to %s\n", qstoc(m_url.toString()));
    while(!m_msg_queue.isEmpty() && m_webSocket.isValid())
        m_webSocket.sendTextMessage(m_msg_queue.dequeue());
}

void CuWSClient::onDisconnected()
{
    pviolet2tmp("-/- CuWSClient: disconnected from %s\n", qstoc(m_url.toString()));
}

void CuWSClient::onMessageReceived(const QString &message)
{
    // listener is CumbiaWebSocket
    //
    m_listener->onUpdate(message);
}

void CuWSClient::onSocketError(QAbstractSocket::SocketError se)
{
    perr("CuWSClient.onSocketError: %s [err: %d] URL: %s", qstoc(m_webSocket.errorString()), se, qstoc(m_url.toString()));
}


