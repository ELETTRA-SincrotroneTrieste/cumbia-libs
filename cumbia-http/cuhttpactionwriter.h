#ifndef CUHTTPACTIONWRITER_H
#define CUHTTPACTIONWRITER_H

#include <cuhttpactiona.h>

class CuHTTPActionWriterPrivate;
class QNetworkReply;
class QNetworkAccessManager;
class HTTPSource;

class CuHttpActionWriter : public CuHTTPActionA
{
    Q_OBJECT
public:
    CuHttpActionWriter(const HTTPSource& target, QNetworkAccessManager *qnam, const QString& http_url);
    ~CuHttpActionWriter();
    void setWriteValue(const CuVariant& w);
    void setConfiguration(const CuData& co);

private slots:
    void onNetworkReplyFinished(QNetworkReply*);

private:
    CuHTTPActionWriterPrivate *d;

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
};

#endif // CUHTTPACTIONWRITER_H
