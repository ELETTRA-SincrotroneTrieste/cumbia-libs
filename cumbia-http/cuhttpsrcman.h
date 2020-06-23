#ifndef SRCQUEUEMANAGER_H
#define SRCQUEUEMANAGER_H

#include <QObject>
#include "cuhttp_source.h"

class SrcQueueManagerPrivate;
class CuDataListener;

class SrcData {
public:
    SrcData();
    SrcData(CuDataListener *l, const std::string& method, const QString& chan);
    CuDataListener *lis;
    std::string method;
    bool isEmpty() const;
    QString channel;
};

class SrcItem {
public:
    SrcItem();
    SrcItem(const std::string& s, CuDataListener*li, const std::string& method, const QString& chan);

    std::string src;
    CuDataListener *l;
    std::string method;
    QString channel;
};

class CuHttpSrcQueueManListener {
public:
    virtual void onSrcBundleReqReady(const QList<SrcItem>& srcs) = 0;
};

/*!
 * \brief Http source queue and data manager
 */
class CuHttpSrcMan : public QObject
{
    Q_OBJECT
public:
    explicit CuHttpSrcMan(CuHttpSrcQueueManListener* l, QObject *parent = nullptr);
    virtual ~CuHttpSrcMan();
    void setQueueManListener(CuHttpSrcQueueManListener* l);
    void enqueueSrc(const CuHTTPSrc & httpsrc, CuDataListener *l, const std::string& method, const QString &chan);

    QList<SrcData> takeSrcs(const QString& src) const;

private slots:
    void onDequeueTimeout();

private:

    SrcQueueManagerPrivate *d;
};

#endif // SRCQUEUEMANAGER_H
