#ifndef CUHTTPACTIONREADER_H
#define CUHTTPACTIONREADER_H

#include <string>
#include <cuthreadlistener.h>
#include <cuhttpactiona.h>
#include <cuhttpactionfactoryi.h>
#include <cuhttp_source.h>
#include <QMap>

class CuHTTPActionReaderPrivate;
class CuActivityManager;
class CuDataListener;
class QNetworkAccessManager;
class QNetworkReply;
class CuHttpChannelReceiver;

class CuHTTPActionReader:  public CuHTTPActionA
{
    Q_OBJECT
public:
    CuHTTPActionReader(const HTTPSource& src, CuHttpChannelReceiver *chan_recv, QNetworkAccessManager *qnam, const QString& url);
    ~CuHTTPActionReader();

    CuData getToken() const;
    HTTPSource getSource() const;
    CuHTTPActionA::Type getType() const;
    void start();
    void stop();

    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();

    void decodeMessage(const QJsonDocument& json);

    bool exiting() const;
    void setOptions(const CuData& o);

private slots:
    void onUnsubscribeReplyFinished();

private:
    CuHTTPActionReaderPrivate *d;
};


#endif // CUTREADER_H