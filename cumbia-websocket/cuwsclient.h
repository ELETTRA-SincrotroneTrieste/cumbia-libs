#ifndef CUWSCLIENT_H
#define CUWSCLIENT_H

#include <QObject>
#include <QWebSocket>

class CuData;

class CuWSClientListener {
public:
    virtual void onUpdate(const QString& message) = 0;
};

class CuWSClient : public QObject
{
    Q_OBJECT
public:
    explicit CuWSClient(const QUrl& url, CuWSClientListener * listener, QObject *parent = nullptr);

    ~CuWSClient();

signals:
    void closed();

public slots:

    void suspend();

    void start();

    void open();

    void close();

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QString& message);
    void onSocketError(QAbstractSocket::SocketError se);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    CuWSClientListener *m_listener;
    bool m_socket_open;
};

#endif // CUWSCLIENT_H
