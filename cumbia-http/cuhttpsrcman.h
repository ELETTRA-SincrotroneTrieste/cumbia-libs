#ifndef SRCQUEUEMANAGER_H
#define SRCQUEUEMANAGER_H

#include <QObject>
#include "cuhttpactiona.h"

class SrcQueueManagerPrivate;
class CuDataListener;

class SrcData {
public:
    SrcData();
    SrcData(CuDataListener *l, const CuHTTPActionA::Type typ, const QString& chan);
    CuDataListener *lis;
    CuHTTPActionA::Type type;
    bool isEmpty() const;
    QString channel;
};

class SrcItem {
public:
    SrcItem();
    SrcItem(const std::string& s, CuDataListener*li, const CuHTTPActionA::Type type, const QString& chan);

    std::string src;
    CuDataListener *l;
    CuHTTPActionA::Type type;
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
    void enqueueSrc(const CuHTTPSrc & httpsrc, CuDataListener *l, const CuHTTPActionA::Type &type, const QString &chan);

    QList<SrcData> takeSrcs(const QString& src) const;

private slots:
    void onDequeueTimeout();

private:

    SrcQueueManagerPrivate *d;
};

#endif // SRCQUEUEMANAGER_H
