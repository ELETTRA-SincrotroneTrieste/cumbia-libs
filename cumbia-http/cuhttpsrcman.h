#ifndef SRCQUEUEMANAGER_H
#define SRCQUEUEMANAGER_H

#include <QObject>
#include <cudata.h>
#include "cuhttp_source.h"

class SrcQueueManagerPrivate;
class CuDataListener;

class SrcData {
public:
    SrcData();
    SrcData(CuDataListener *l,
            const std::string& method,
            const QString& chan, const CuData& opts,
            const CuVariant& w_val = CuVariant());
    CuDataListener *lis;
    std::string method;
    bool isEmpty() const;
    QString channel;
    CuVariant wr_val;
    CuData options;
};

class SrcItem {
public:
    SrcItem();
    SrcItem(const std::string& s,
            CuDataListener*li,
            const std::string& method,
            const QString& chan,
            const CuVariant& w_val,
            const CuData& opts);

    std::string src;
    CuDataListener *l;
    std::string method;
    QString channel;
    CuVariant wr_val;
    CuData options;
};

class CuHttpSrcQueueManListener {
public:
    virtual void onSrcBundleReqReady(const QList<SrcItem>& rsrcs, const QList<SrcItem>& wsrcs,
                                     const QList<SrcItem>& unsubsrcs) = 0;
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
    void enqueueSrc(const CuHTTPSrc & httpsrc, CuDataListener *l, const std::string& method, const QString &chan, const CuVariant &w_val, const CuData &options);
    void cancelSrc(const CuHTTPSrc &httpsrc, const std::string& method, CuDataListener *l, const QString&);
    bool queue_contains(const std::string& src, CuDataListener* l) const;
    QList<SrcData> takeSrcs(const QString& src) const;
    int dequeueItems(QList<SrcItem> &read_i, QList<SrcItem> &write_i, QList<SrcItem> &unsu_i);
    QMap<QString, SrcData> takeTgts() const;
    QMap<QString, SrcData> takeSrcs() const;
    const QMap<QString, SrcData> &targetMap() const;
    void process_queue();

private slots:
    void onDequeueTimeout();

private:

    SrcQueueManagerPrivate *d;
    bool m_queue_remove(const std::string& src, const string &method, CuDataListener* l);
    bool m_wait_map_remove(const std::string& src, const string &method, CuDataListener* l);
    void m_do_enqueue(const CuHTTPSrc & httpsrc, CuDataListener *l, const std::string& method, const QString &chan, const CuVariant &w_val, const CuData &options);
};

#endif // SRCQUEUEMANAGER_H
