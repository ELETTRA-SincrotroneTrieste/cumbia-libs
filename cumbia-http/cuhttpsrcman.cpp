#include "cuhttpsrcman.h"
#include "cuhttpactionfactoryi.h"
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

SrcItem::SrcItem(const std::string &s, CuDataListener *li, const CuHTTPActionFactoryI &fa) :
    src(s), l(li), factory(fa.clone()) { }

SrcItem::SrcItem() : l(nullptr) { }

SrcData::SrcData(CuDataListener *l, const CuHTTPActionA::Type typ) : lis(l), type(typ) { }

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

void CuHttpSrcMan::enqueueSrc(const std::string &source, CuDataListener *l, const CuHTTPActionFactoryI &f) {
    if(d->timer->interval() > TMR_DEQUEUE_INTERVAL)
        d->timer->setInterval(TMR_DEQUEUE_INTERVAL); // restore quick timeout if new sources are on the way
    if(!d->timer->isActive()) d->timer->start();
    d->srcq.enqueue(SrcItem(source, l, f));
}

void CuHttpSrcMan::onDequeueTimeout() {
    qDebug() << __PRETTY_FUNCTION__ << "dequeueing" << d->srcq.size() << "items -- timer interval " << d->timer->interval();
    QList<SrcItem> items;
    while(!d->srcq.isEmpty()) {
        const SrcItem& i = d->srcq.dequeue();
        d->srcd.insert(QString::fromStdString(i.src), SrcData(i.l, i.factory->getType()));
        items.append(i);
    }
    // slow down timer if no sources
    if(items.size() == 0) d->timer->setInterval(d->timer->interval() * 2);
    else d->lis->onSrcBundleReqReady(items);
}

