#ifndef CUBOTSENDER_H
#define CUBOTSENDER_H

#include <QObject>
#include <QNetworkReply>

class CuBotSenderPrivate;
class CuData;

class CuBotSender : public QObject
{
    Q_OBJECT
public:
    explicit CuBotSender(QObject *parent = nullptr);

signals:

public slots:
    void sendMessage(int chat_id, const QString& msg);

private slots:
    void onNetworkError(QNetworkReply::NetworkError e);
    void onReply();

private:
    CuBotSenderPrivate *d;
};

#endif // CUBOTSENDER_H
