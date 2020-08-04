#include "cuhttpsrcman.h"
#include "cuhttpactionfactoryi.h"
#include "cuhttp_source.h"
#include <QQueue>
#include <QTimer>
#include <QMultiMap>
#include <QtDebug>

#define TMR_DEQUEUE_INTERVAL 400

class SrcQueueManagerPrivate {
public:
    QTimer *timer;
    QQueue<SrcItem> srcq;
    QMultiMap<QString, SrcData> srcd;
    QMap<QString, SrcData> tgtd;
    QList<SrcItem> r_items, w_items;
    CuHttpSrcQueueManListener *lis;
    CuData options;
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
    d->srcq.enqueue(SrcItem(s, l, method, chan, w_val, options));
}

/*!
 * \brief cancel a previously set source
 *
 * There are two possibile situations:
 * \li src enqueued for set source. Remove src from the queue so that the operation is canceled
 * \li src waiting for the http sync reply: remove src from the wait map so that the reply will
 *     not update l
 */
void CuHttpSrcMan::cancelSrc(const CuHTTPSrc &httpsrc, const std::string& method, CuDataListener *l, const QString& chan) {
    bool rem = m_queue_remove(httpsrc.prepare(), method, l);
    printf("CuHttpSrcMan::cancelSrc searched src %s method %s found in queue? %d\n",
           httpsrc.prepare().c_str(), method.c_str(), rem);

    if(!rem) { // if rem, src was still in queue, no request sent
        rem = m_wait_map_remove(httpsrc.prepare(), method, l);
        // if not in queue, request could have been sent: send "u" unsubscribe req
        if(method == "s")
            enqueueSrc(httpsrc, l, "u", chan, CuVariant(), CuData());
    }
}

bool CuHttpSrcMan::m_queue_remove(const string &src, const std::string& method, CuDataListener *l) {
    const int siz = d->srcq.size();
    QMutableListIterator<SrcItem> mi(d->srcq);
    while(mi.hasNext()) {
        mi.next();
//        printf("CuHttpSrcMan.m_queue_remove comparing %s wit %s %p wit %p\e[0m\n", mi.value().src.c_str(), src.c_str(), mi.value().l, l);
        if((mi.value().src == src && mi.value().l == l && mi.value().method == method) || (mi.value().l == nullptr)) {
            mi.remove();
        }
    }
    return siz != d->srcq.size();
}

bool CuHttpSrcMan::m_wait_map_remove(const string &src, const string &method, CuDataListener *l) {
    bool r = false;
    QMutableMapIterator<QString, SrcData> mi(d->srcd);
    while(mi.hasNext()) {
        mi.next();
        if((mi.key() == QString::fromStdString(src) && mi.value().lis == l && mi.value().method == method) || mi.value().lis == nullptr) {
            printf("\e[1;35mCuHttpSrcMan::unlinkSrc deactivating listener %p for src %s meth %s\n",
                   l, src.c_str(), mi.value().method.c_str());
            mi.remove();
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

const QMap<QString, SrcData>& CuHttpSrcMan::targetMap() const {
    return d->tgtd;
}

QMap<QString, SrcData> CuHttpSrcMan::takeTgts() const {
    QMap<QString, SrcData> tgtd = std::move(d->tgtd);
    d->tgtd.clear();
    return tgtd;
}

void CuHttpSrcMan::onDequeueTimeout() {
    bool empty = d->srcq.isEmpty();
    while(!d->srcq.isEmpty()) {
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

