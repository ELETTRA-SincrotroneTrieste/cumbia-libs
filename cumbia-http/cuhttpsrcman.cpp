#include "cuhttpsrcman.h"
#include "cuhttpactionfactoryi.h"
#include "cuhttp_source.h"
#include <QQueue>
#include <QTimer>
#include <QMultiMap>
#include <QtDebug>

#define TMR_DEQUEUE_INTERVAL 200

class SrcQueueManagerPrivate {
public:
    QTimer *timer;
    QQueue<SrcItem> srcq;
    QMultiMap<QString, SrcData> srcd;
    CuHttpSrcQueueManListener *lis;
};

SrcItem::SrcItem(const std::string &s, CuDataListener *li, const CuHTTPActionA::Type typ, const QString &chan) :
    src(s), l(li), type(typ), channel(typ == CuHTTPActionA::Reader || typ == CuHTTPActionA::Stop ? chan : "") {
}

SrcItem::SrcItem() : l(nullptr) { }

SrcData::SrcData(CuDataListener *l, const CuHTTPActionA::Type typ, const QString &chan) : lis(l), type(typ), channel(chan) { }

bool SrcData::isEmpty() const {
    return this->lis == nullptr;
}

SrcData::SrcData() : lis(nullptr) { }

CuHttpSrcMan::CuHttpSrcMan(CuHttpSrcQueueManListener* l, QObject *parent) : QObject(parent) {
    d = new SrcQueueManagerPrivate;
    d->lis = l;
    d->timer = new QTimer(this);
    d->timer->setInterval(TMR_DEQUEUE_INTERVAL);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(onDequeueTimeout()));
}

CuHttpSrcMan::~CuHttpSrcMan() {
    delete d;
}

void CuHttpSrcMan::setQueueManListener(CuHttpSrcQueueManListener *l) {
    d->lis = l;
}

void CuHttpSrcMan::enqueueSrc(const CuHTTPSrc &httpsrc, CuDataListener *l, const CuHTTPActionA::Type &type, const QString& chan) {
    if(d->timer->interval() > TMR_DEQUEUE_INTERVAL)
        d->timer->setInterval(TMR_DEQUEUE_INTERVAL); // restore quick timeout if new sources are on the way
    if(!d->timer->isActive()) d->timer->start();
    std::string s = httpsrc.prepare();
    d->srcq.enqueue(SrcItem(s, l, type, chan));
}

QList<SrcData> CuHttpSrcMan::takeSrcs(const QString &src) const {
    QList<SrcData> srcd = d->srcd.values(src);
    d->srcd.remove(src);
    return srcd;
}

void CuHttpSrcMan::onDequeueTimeout() {
    qDebug() << __PRETTY_FUNCTION__ << "dequeueing" << d->srcq.size() << "items -- timer interval " << d->timer->interval();
    QList<SrcItem> items;
    while(!d->srcq.isEmpty()) {
        const SrcItem& i = d->srcq.dequeue();
        d->srcd.insert(QString::fromStdString(i.src), SrcData(i.l, i.type, i.channel));
        items.append(i);
    }
    // slow down timer if no sources
    if(items.size() == 0) d->timer->setInterval(d->timer->interval() * 2);
    else d->lis->onSrcBundleReqReady(items);
}

