#ifndef CUWSACTIONWRITERCONF_H
#define CUWSACTIONWRITERCONF_H

#include <cuwsactioni.h>
class CuWsActionWriterConfPrivate;
class QNetworkReply;
class WSSource;
class CuWSClient;

#include <QObject>

class CuWsActionWriterConf : public QObject, public CuWSActionI
{
    Q_OBJECT
public:
    CuWsActionWriterConf(const WSSource& src, CuWSClient *wscli, const QString& http_url);

private slots:
    void onNetworkReplyFinished(QNetworkReply*);


    // CuWSActionI interface
public:
    WSSource getSource() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();
    void start();
    bool exiting() const;
    void stop();
    void decodeMessage(const QJsonDocument &json);


private:
    CuWsActionWriterConfPrivate *d;
};

#endif // CUWSACTIONWRITER_H
