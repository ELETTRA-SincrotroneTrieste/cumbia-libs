#ifndef CUHTTPCHANNELRECEIVER_H
#define CUHTTPCHANNELRECEIVER_H

#include <QString>
#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>

class CuHttpChannelReceiverPrivate;
class CuHTTPActionReader;
class QNetworkAccessManager;
class CuDataListener;

/*!
 * \brief Receive messages on the channel and distributes them
 */
class CuHttpChannelReceiver : public QObject
{
    Q_OBJECT
public:
    explicit CuHttpChannelReceiver(const QString &url, const QString& chan, QNetworkAccessManager *nam);

    QString channel() const;
    void registerReader(const QString& src, CuHTTPActionReader *r);
    void unregisterReader(const QString& src);

    QString url() const;
    void addDataListener(const QString& src, CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();
    void start();
    void stop();
    void decodeMessage(const QJsonValue &json);
    QNetworkRequest prepareRequest(const QUrl& url) const;

protected slots:
    virtual void onNewData();
    virtual void onReplyFinished();
    virtual void onReplyDestroyed(QObject *);
    virtual void onSslErrors(const QList<QSslError> &errors);
    virtual void onError(QNetworkReply::NetworkError code);
    virtual void cancelRequest();

private:

    void m_on_buf_complete();
    QList<QByteArray>  m_extract_data(const QByteArray& in) const;
    bool m_likely_valid(const QByteArray& ba) const;

    CuHttpChannelReceiverPrivate *d;
};

#endif // CUHTTPCHANNELRECEIVER_H
