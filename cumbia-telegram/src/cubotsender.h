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
    explicit CuBotSender(QObject *parent, const QString &bot_tok);

signals:

public slots:
    /**
     * @brief sendMessage
     * @param chat_id
     * @param msg
     * @param silent if true, sends the message silently. Users will receive a notification with no sound.
     */
    void sendMessage(int chat_id, const QString& msg, bool silent = false, bool wait_for_reply = false);

    void sendPic(int chat_id, const QByteArray& imgBytes, bool silent = false);

private slots:
    void onNetworkError(QNetworkReply::NetworkError e);
    void onReply();

private:
    CuBotSenderPrivate *d;

    QString m_truncateMsg(const QString &in);
};

#endif // CUBOTSENDER_H
