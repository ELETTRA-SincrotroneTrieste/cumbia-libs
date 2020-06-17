#ifndef SRCQUEUEMANAGER_H
#define SRCQUEUEMANAGER_H

#include <QObject>
#include "cuhttpactionfactoryi.h"

class SrcQueueManagerPrivate;
class CuDataListener;
class CuHTTPActionFactoryI;

class SrcData {
public:
    SrcData();
    SrcData(CuDataListener *l, const CuHTTPActionA::Type typ);
    CuDataListener *lis;
    CuHTTPActionA::Type type;
};

class SrcItem {
public:
    SrcItem();
    SrcItem(const std::string& s, CuDataListener*li, const CuHTTPActionFactoryI& fa);

    std::string src;
    CuDataListener *l;
    CuHTTPActionFactoryI *factory;
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
    void enqueueSrc(const std::string &source, CuDataListener *l, const CuHTTPActionFactoryI &f);

private slots:
    void onDequeueTimeout();

private:

    SrcQueueManagerPrivate *d;
};

#endif // SRCQUEUEMANAGER_H
