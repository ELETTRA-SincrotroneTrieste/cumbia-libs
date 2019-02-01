#ifndef CUBOTLISTENER_H
#define CUBOTLISTENER_H

#include <QObject>
#include <QNetworkReply>
#include <QSslError>

class CuBotListenerPrivate;
class TBotMsg;

class CuBotListener : public QObject
{
    Q_OBJECT
public:

    explicit CuBotListener(QObject *parent);

    ~CuBotListener();

public slots:
    void start();
    void stop();

signals:
    void onNewMessage(const TBotMsg &msg);

    void onError(const QString& src, const QString& message);

protected slots:
    void onReply();
    void onNetworkError(QNetworkReply::NetworkError);
    void onSSLErrors(const QList<QSslError> & errors);

    void getUpdates();

private:
    CuBotListenerPrivate *d;

};

#endif // CUBOTLISTENER_H
