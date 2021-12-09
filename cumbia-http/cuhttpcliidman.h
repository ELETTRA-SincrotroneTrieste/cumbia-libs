#ifndef CUHTTPCLIIDMAN_H
#define CUHTTPCLIIDMAN_H

#include <QObject>
#include <QNetworkReply>
#include <QSslError>

class CuHttpCliIdManListener {
public:
    virtual void onIdReady(const unsigned long long& client_id, const time_t ttl) = 0;
    virtual void onIdManError(const QString& err) = 0;
};

class CuHttpCliIdManPrivate;

class CuHttpCliIdMan : public QObject
{
    Q_OBJECT
public:
    explicit CuHttpCliIdMan(const QString& url, QNetworkAccessManager *nm, CuHttpCliIdManListener *li);
    ~CuHttpCliIdMan();

    void start();
    void renew();

    QString error() const;

protected slots:
    virtual void onNewData();
    virtual void onReplyFinished();
    virtual void onReplyDestroyed(QObject *);
    virtual void onSslErrors(const QList<QSslError> &errors);
    virtual void onError(QNetworkReply::NetworkError code);

    void send_keepalive();

private:
    CuHttpCliIdManPrivate* d;
    QByteArray m_json(unsigned long long id) const;
    bool m_get_id_and_ttl();
    void m_start_keepalive();
    void m_reply_connect(QNetworkReply *reply);
    void m_make_network_request(QNetworkRequest *r) const;
    void m_notify_err(bool is_id_req);
};

#endif // CUHTTPCLIIDMAN_H
