#ifndef CUWSACTIONWRITER_H
#define CUWSACTIONWRITER_H

#include <cuwsactioni.h>
#include <QObject>

class CuWSActionWriterPrivate;
class QNetworkReply;
class CuWSClient;
class WSSource;

class CuWsActionWriter : public QObject, public CuWSActionI
{
    Q_OBJECT
public:
    CuWsActionWriter(const WSSource& target, CuWSClient *wscli, const QString& http_url);
    ~CuWsActionWriter();
    void setWriteValue(const CuVariant& w);
    void setConfiguration(const CuData& co);

private slots:
    void onNetworkReplyFinished(QNetworkReply*);

private:
    CuWSActionWriterPrivate *d;

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
};

#endif // CUWSACTIONWRITER_H
