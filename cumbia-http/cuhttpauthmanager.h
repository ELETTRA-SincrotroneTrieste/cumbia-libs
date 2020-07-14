#ifndef CUHTTPAUTHMANAGER_H
#define CUHTTPAUTHMANAGER_H

#include <QObject>
#include <QSslError>
#include <QList>
#include <QNetworkReply>

class QNetworkAccessManager;
class CuHttpAuthManagerPrivate;
class QString;
class CuData;

class CuHttpAuthManListener {
public:
    virtual void onCredsReady(const QString& user, const QString& passwd) = 0;
    virtual void onAuthReply(bool authorised, const QString &user, const QString &message, bool encrypted) = 0;
    virtual void onAuthError(const QString& errm) = 0;
};

class CuHttpAuthManager : public QObject
{
    Q_OBJECT
public:

    CuHttpAuthManager(QNetworkAccessManager *netman, CuHttpAuthManListener *l, QObject *parent = nullptr);
    ~CuHttpAuthManager();

    void tryAuthorize(const QString &user, const QString &pass);
    QByteArray getCookie() const;
    void authPrompt(const QString& auth_url, bool cli = false);

signals:
    void credentials(const QString& user, const QString& password);
    void authReply(bool authorised, const QString& user, const QString& message, bool encrypted);
    void error(const QString& msg);

protected slots:
    void onCredsReady(const QString &user, const QString &passwd);
private slots:
    void onNewData();
    void onReplyFinished();
    void onSslErrors(const QList<QSslError> &);
    void onError(QNetworkReply::NetworkError);
    void onReplyDestroyed(QObject *);
    void onReplyEncrypted();


private:
    CuHttpAuthManagerPrivate *d;

    CuData m_make_error_data(const QString& msg) const;
};

#endif // CUHTTPAUTHMANAGER_H
