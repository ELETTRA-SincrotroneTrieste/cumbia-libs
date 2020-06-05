#ifndef CUHTTPACTIONCONF_H
#define CUHTTPACTIONCONF_H

#include <cuhttpactiona.h>

class CuHttpActionConfPrivate;
class QNetworkReply;
class HTTPSource;
class QNetworkAccessManager;
class CuHttpAuthManager;

class CuHttpActionConf : public CuHTTPActionA {
    Q_OBJECT
public:
    CuHttpActionConf(const HTTPSource& src, QNetworkAccessManager *qnam, const QString& http_url, CuHttpAuthManager *aman = nullptr);
    ~CuHttpActionConf();

    // CuHTTPActionI interface
public:
    HTTPSource getSource() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();
    void start();
    bool exiting() const;
    void stop();
    void decodeMessage(const QJsonDocument &json);

private:
    CuHttpActionConfPrivate *d;
};

#endif // CUHTTPACTIONWRITER_H
