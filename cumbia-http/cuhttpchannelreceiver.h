#ifndef CUHTTPCHANNELRECEIVER_H
#define CUHTTPCHANNELRECEIVER_H

#include "cuhttpactiona.h"

class CuHttpChannelReceiverPrivate;
class CuHTTPActionReader;

/*!
 * \brief Receive messages on the channel and distributes them
 */
class CuHttpChannelReceiver : public CuHTTPActionA
{
    Q_OBJECT
public:
    explicit CuHttpChannelReceiver(const QString &url, const QString& chan, QNetworkAccessManager *nam);

    QString channel() const;
    void registerReader(const QString& src, CuHTTPActionReader *r);
    void unregisterReader(const QString& src);
    void setDataExpireSecs(time_t secs);
    time_t dataExpiresSecs() const;

signals:

    // CuHTTPActionA interface
public:
    QString getSourceName() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();
    void start();
    bool exiting() const;
    void stop();
    void decodeMessage(const QJsonDocument &json);
    QNetworkRequest prepareRequest(const QUrl& url) const;

private:
    CuHttpChannelReceiverPrivate *d;

    bool m_data_fresh(const double timestamp_ms, time_t *diff_t) const;
};

#endif // CUHTTPCHANNELRECEIVER_H
