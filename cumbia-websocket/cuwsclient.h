#ifndef CUWSCLIENT_H
#define CUWSCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QQueue>

class CuData;

class CuWSClientPrivate;

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

    bool isOpen() const;

signals:
    void closed();

public slots:

    void suspend();

    void start();

    void open();

    void close();

    void sendMessage(const QString& msg);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QString& message);
    void onSocketError(QAbstractSocket::SocketError se);
    void deliverMsgs();

private:
    CuWSClientPrivate *d;
};

#endif // CUWSCLIENT_H
