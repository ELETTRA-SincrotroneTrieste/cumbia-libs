#ifndef CUHTTPBUNDLEDSRCREQ_H
#define CUHTTPBUNDLEDSRCREQ_H

#include <QObject>
#include <QSslError>
#include <QNetworkReply>
#include "cuhttpsrcman.h"

class CuHttpBundledSrcReqPrivate;

class CuHttpBundledSrcReqListener {
public:
    virtual void onSrcBundleReplyReady(const QByteArray& json) = 0;
};

class CuHttpBundledSrcReq : public QObject
{
    Q_OBJECT
public:
    explicit CuHttpBundledSrcReq(const QList<SrcItem>& srcs,
                                 CuHttpBundledSrcReqListener *l,
                                 QObject *parent = nullptr);
    explicit CuHttpBundledSrcReq(const QMap<QString, SrcData>& targetmap,
                                 CuHttpBundledSrcReqListener *l,
                                 const QByteArray& cookie = QByteArray(),
                                 QObject *parent = nullptr);

    virtual ~CuHttpBundledSrcReq();

    void start(const QUrl &url, QNetworkAccessManager *nam);

signals:

protected slots:
    virtual void onNewData();
    virtual void onReplyFinished();
    virtual void onReplyDestroyed(QObject *);
    virtual void onSslErrors(const QList<QSslError> &errors);
    virtual void onError(QNetworkReply::NetworkError code);

private:
    CuHttpBundledSrcReqPrivate* d;

    void m_on_buf_complete();
    bool m_likely_valid(const QByteArray &ba) const;
};

#endif // CUHTTPBUNDLEDSRCREQ_H
