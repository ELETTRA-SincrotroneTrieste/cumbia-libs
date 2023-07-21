#include "cuhttpsrcman.h"
#include "cuhttp_source.h"
#include <QQueue>
#include <QTimer>
#include <QMultiMap>
#include <QtDebug>

#define TMR_DEQUEUE_INTERVAL 400

class SrcQueueManagerPrivate {
public:
    QTimer *timer;
    QQueue<SrcItem> srcq; // srcq_add contains srcs with methods != "u"
    QMultiMap<QString, SrcData> srcd;
    QMap<QString, SrcData> tgtd;
    QList<SrcItem> r_items, w_items;
    CuHttpSrcQueueManListener *lis;
    CuData options;
    int dequ_chunk_siz;
};

SrcItem::SrcItem(const std::string &s, CuDataListener *li, const std::string &metho,
                 const QString &chan, const CuVariant &w_val, const CuData &opts) :
    src(s), l(li), method(metho),
    channel(metho == "s" || metho == "u" ? chan : ""), wr_val(w_val), options(opts) {
}

SrcItem::SrcItem() : l(nullptr) { }

SrcData::SrcData(CuDataListener *l, const string &me, const QString &chan, const CuData &opts, const CuVariant &w_val)
    : lis(l), method(me), channel(chan), wr_val(w_val), options(opts) { }

bool SrcData::isEmpty() const {
    return this->lis == nullptr;
}

SrcData::SrcData() : lis(nullptr) { }

CuHttpSrcMan::CuHttpSrcMan(CuHttpSrcQueueManListener* l, int deq_chunksiz, QObject *parent) : QObject(parent) {
    d = new SrcQueueManagerPrivate;
    d->lis = l;
    d->timer = new QTimer(this);
    d->timer->setInterval(TMR_DEQUEUE_INTERVAL);
    d->dequ_chunk_siz = deq_chunksiz; // dequeue deq_chunksiz items then sleep for TMR_DEQUEUE_INTERVAL
    connect(d->timer, SIGNAL(timeout()), this, SLOT(onDequeueTimeout()));
}

CuHttpSrcMan::~CuHttpSrcMan() {
    d->timer->stop();
    delete d;
}

void CuHttpSrcMan::setQueueManListener(CuHttpSrcQueueManListener *l) {
    d->lis = l;
}

void CuHttpSrcMan::enqueueSrc(const CuHTTPSrc &httpsrc,
                              CuDataListener *l,
                              const std::string& method,
                              const QString& chan,
                              const CuVariant &w_val,
                              const CuData& options) {
    if(d->timer->interval() > TMR_DEQUEUE_INTERVAL)
        d->timer->setInterval(TMR_DEQUEUE_INTERVAL); // restore quick timeout if new sources are on the way
    if(!d->timer->isActive()) d->timer->start();
    std::string s = httpsrc.prepare();
    const SrcItem si(s, l, method, chan, w_val, options);
    d->srcq.enqueue(si);
}

void CuHttpSrcMan::unlinkListener(CuDataListener *l) {
    // set listener to nullptr src queue
    bool rem = m_queue_remove(l);
    if(!rem) { // if rem, src was still in queue, no request sent
        rem = m_wait_map_remove(l);
    }
}

// remove from all queues where listener equals l
bool CuHttpSrcMan::m_queue_remove(CuDataListener *l) {
    const int siz = d->srcq.size();
    pretty_pri("finding listener %p srcq size %d", l, d->srcq.size());
    QMutableListIterator<SrcItem> mi(d->srcq);
    while(mi.hasNext()) {
        mi.next();
        if(mi.value().l == l) {
            mi.value().l = nullptr;
        }
    }
    return siz != d->srcq.size();
}

bool CuHttpSrcMan::m_wait_map_remove(CuDataListener* l) {
    bool r = false;
    QMutableMapIterator<QString, SrcData> mi(d->srcd);
    while(mi.hasNext()) {
        mi.next();
        if(mi.value().lis == l) {
            mi.value().lis = nullptr;
            r = true;
        }
    }
    QMutableMapIterator<QString, SrcData> tgtmi(d->tgtd);
    while(tgtmi.hasNext()) {
        tgtmi.next();
        if(tgtmi.value().lis == l  || tgtmi.value().lis == nullptr) {
            tgtmi.value().lis = nullptr;
            r = true;
        }
    }
    return r;
}

QList<SrcData> CuHttpSrcMan::takeSrcs(const QString &src) const {
    QList<SrcData> srcd = d->srcd.values(src);
    d->srcd.remove(src);
    return srcd;
}

int CuHttpSrcMan::dequeueItems(QList<SrcItem> &read_i, QList<SrcItem> &write_i) {
    read_i.clear(); write_i.clear();
    while(!d->srcq.isEmpty()) {
        const SrcItem& i = d->srcq.dequeue();
        i.method != "write" ?  read_i.append(i) : write_i.append(i);
    }
    return read_i.size() + write_i.size();
}

const QMap<QString, SrcData>& CuHttpSrcMan::targetMap() const {
    return d->tgtd;
}

void CuHttpSrcMan::process_queue() {
    onDequeueTimeout();
}

QMap<QString, SrcData> CuHttpSrcMan::takeTgts() const {
    QMap<QString, SrcData> tgtd = std::move(d->tgtd);
    d->tgtd.clear();
    return tgtd;
}

QMap<QString, SrcData> CuHttpSrcMan::takeSrcs() const {
    QMap<QString, SrcData> srcd = std::move(d->srcd);
    d->srcd.clear();
    return srcd;
}

void CuHttpSrcMan::onDequeueTimeout() {
    int x = 0;
    bool empty = d->srcq.isEmpty();
    while(!d->srcq.isEmpty() && x++ < d->dequ_chunk_siz) {
        const SrcItem& i = d->srcq.dequeue();
        i.method != "write" ?  d->srcd.insert(QString::fromStdString(i.src), SrcData(i.l, i.method, i.channel, i.options))
                             : d->tgtd.insert(QString::fromStdString(i.src), SrcData(i.l, i.method, i.channel, i.options, i.wr_val));
        i.method != "write" ?  d->r_items.append(i) : d->w_items.append(i);
    }
    if(d->r_items.size() || d->w_items.size()) {
        d->lis->onSrcBundleReqReady(d->r_items, d->w_items);
        d->r_items.clear();
        d->w_items.clear();
    }
    // slow down timer if no sources
    if(empty) {
        d->timer->stop();
    }
}

