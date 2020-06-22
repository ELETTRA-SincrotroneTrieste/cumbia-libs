#ifndef CUHTTPACTIONWRITER_H
#define CUHTTPACTIONWRITER_H

#include <cuhttpactiona.h>
#include "cuhttpauthmanager.h"

class CuHTTPActionWriterPrivate;
class QNetworkReply;
class QNetworkAccessManager;
class CuHTTPSrc;
class CuHttpAuthManager;

class CuHttpActionWriter : public CuHTTPActionA
{
    Q_OBJECT
public:
    CuHttpActionWriter(const CuHTTPSrc& target, QNetworkAccessManager *qnam, const QString& http_url, CuHttpAuthManager *authman);
    ~CuHttpActionWriter();
    void setWriteValue(const CuVariant& w);
    void setConfiguration(const CuData& co);

private slots:
    void onNetworkReplyFinished(QNetworkReply*);
    void onCredentials(const QString& user, const QString& passwd);
    void onAuthReply(bool authorised, const QString &user, const QString &message, bool encrypted);
    void onAuthError(const QString& errm);


private:
    CuHTTPActionWriterPrivate *d;
    CuData m_make_error_data(const QString &msg);
    void m_notify_result(const CuData& res);

    // CuHTTPActionA
public:
    QString getSourceName() const;
    Type getType() const;
    void addDataListener(CuDataListener *l);
    void removeDataListener(CuDataListener *l);
    size_t dataListenersCount();
    void start();
    bool exiting() const;
    void stop();
    void decodeMessage(const QJsonValue &jsv);
    void notifyActionFinished();
    QNetworkRequest prepareRequest(const QUrl& url) const;
};

#endif // CUHTTPACTIONWRITER_H
