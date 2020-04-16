#ifndef CUWSACTIONCONF_H
#define CUWSACTIONCONF_H

#include <cuwsactioni.h>
class CuWsActionConfPrivate;
class QNetworkReply;
class WSSource;
class CuWSClient;

#include <QObject>

class CuWsActionConf : public QObject, public CuWSActionI
{
    Q_OBJECT
public:
    CuWsActionConf(const WSSource& src, CuWSClient *wscli, const CuWSActionI::Type action_type, const QString& http_url);
    ~CuWsActionConf();
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
    CuWsActionConfPrivate *d;
};

#endif // CUWSACTIONWRITER_H
