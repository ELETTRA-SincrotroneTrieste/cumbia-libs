#ifndef WSCLIENT_H
#define WSCLIENT_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
class QNetworkReply;

class WsClient : public QObject
{
    Q_OBJECT
public:
    explicit WsClient(const QUrl &ws_url, const QStringList& src_urls, QObject *parent = nullptr);

    ~WsClient();

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString message);
    void onSocketError(QAbstractSocket::SocketError);
    void onSocketDisconnected();
    void replyFinished(QNetworkReply* r);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    QList<QUrl> m_src_urls;

    QString m_getSrc(const QByteArray &jsba);
};

#endif // WIDGET_H
