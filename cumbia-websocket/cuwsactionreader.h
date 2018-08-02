#ifndef CUWSACTIONREADER_H
#define CUWSACTIONREADER_H

#include <string>
#include <cuthreadlistener.h>
#include <cuwsactioni.h>
#include <cuwsactionfactoryi.h>
#include <ws_source.h>
#include <QObject>
#include <QMap>

class CuWSActionReaderPrivate;
class CuActivityManager;
class CuDataListener;
class CumbiaWebSocket;
class QNetworkReply;

class WSSourceConfiguration {
public:

    WSSourceConfiguration();

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

class CuWSActionReader: public QObject,  public CuWSActionI
{
    Q_OBJECT
public:
    CuWSActionReader(const WSSource& src, CumbiaWebSocket *ct);

    ~CuWSActionReader();

    CuData getToken() const;

    WSSource getSource() const;

    CuWSActionI::Type getType() const;

    void start();

    void stop();

    void addDataListener(CuDataListener *l);

    void removeDataListener(CuDataListener *l);

    size_t dataListenersCount();

    void decodeMessage(const QJsonDocument& json);

    bool exiting() const;

private slots:
    void onNetworkReplyFinished(QNetworkReply *reply);

private:
    CuWSActionReaderPrivate *d;

};


#endif // CUTREADER_H
