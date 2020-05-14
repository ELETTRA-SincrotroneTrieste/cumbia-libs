#ifndef CUHTTPACTIONCONF_H
#define CUHTTPACTIONCONF_H

#include <cuhttpactiona.h>
class CuHttpActionConfPrivate;
class QNetworkReply;
class HTTPSource;
class QNetworkAccessManager;


class CuHttpActionConf : public CuHTTPActionA
{
    Q_OBJECT
public:
    CuHttpActionConf(const HTTPSource& src, QNetworkAccessManager *qnam, const CuHTTPActionA::Type action_type, const QString& http_url);
    ~CuHttpActionConf();

protected slots:


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
