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

class HTTPSourceConfiguration {
public:

    HTTPSourceConfiguration();

    void add(const QString& key, const QString &value);

    bool isComplete() const;

    CuData toCuData() const;

    QStringList keys() const;

    void setError(const QString& message);

    bool error() const;

    QString errorMessage() const;

private:
    QMap<QString, QString> m_map;

    QStringList m_keys;

    QString m_errorMsg;
};

class CuHTTPActionReader:  public CuHTTPActionA
{
    Q_OBJECT
public:
    CuHTTPActionReader(const HTTPSource& src, QNetworkAccessManager *qnam, const QString& url);

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

    QNetworkRequest prepareRequest(const QUrl& url) const;

private:
    CuHTTPActionReaderPrivate *d;

};


#endif // CUTREADER_H
