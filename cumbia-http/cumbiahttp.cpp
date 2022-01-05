#include "cumbiahttp.h"
#include "cuhttpchannelreceiver.h"
#include "cuhttpauthmanager.h"
#include "cuhttpsrchelper_i.h"
#include "cuhttpsrcman.h"
#include "cuhttpwritehelper.h"
#include "cuhttpbundledsrcreq.h"

#include <cumacros.h>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumbiahttpworld.h>
#include <cuhttpactionfactoryservice.h>
#include <cuhttpactionfactoryi.h>
#include <QtDebug>
#include <QUrl>

#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>
#include <qnetworkaccessmanager.h>
#include <qureplacewildcards_i.h>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QThread> // for QThread::currentThread()
#include <qustring.h>

class CuHttpSrcReqQueue {
public:
    void add(const QList<SrcItem>& rr, const QList<SrcItem>& wr, const QList<SrcItem> & ur) {
        rreqs += rr; // read, subscribe
        wreqs += wr; // write
        ureqs += ur; // unsubscribe
    }
    void empty_q() { wreqs.clear(); rreqs.clear(); ureqs.clear(); }

    QList<SrcItem> wreqs, rreqs, ureqs;
};

class CumbiaHttpPrivate {
public:
    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
    QString url;
    QList<QuReplaceWildcards_I *> m_repl_wildcards_i;
    QList<CuHttpSrcHelper_I *>src_helpers;
    QNetworkAccessManager *qnam;
    CuHttpChannelReceiver *chan_recv;
    CuHttpAuthManager *auth_man;
    CuHttpSrcMan *src_q_man;
    CuHttpWriteHelper *w_helper;
    int chan_ttl;
    unsigned long long client_id;
    CuHttpSrcReqQueue request_q;
    CuHttpCliIdMan *id_man;
};

/*!
 * \brief CumbiaHttp
 * \param http_url the url to be used for http incoming data
 * \param http_url the http (https) url to be used to send requests to the server (output)
 * \param tfi thread factory implementation
 * \param teb thread events bridge factory
 */
CumbiaHttp::CumbiaHttp(const QString &url,
                       const QString& channel,
                       CuThreadFactoryImplI *tfi,
                       CuThreadsEventBridgeFactory_I *teb)
{
    d = new CumbiaHttpPrivate;
    d->m_threadsEventBridgeFactory = teb;
    d->m_threadFactoryImplI = tfi;
    d->url = url;
    d->chan_ttl = 5;
    d->qnam = new QNetworkAccessManager(nullptr);
    d->chan_recv = new CuHttpChannelReceiver(d->url, channel, d->qnam);
    d->chan_recv->setDataExpireSecs(static_cast<time_t>(d->chan_ttl));
    d->auth_man = new CuHttpAuthManager(d->qnam, this);
    d->chan_recv->start();
    d->src_q_man = new CuHttpSrcMan(this);
    d->w_helper = nullptr;
    d->client_id = 0;
    d->id_man = new CuHttpCliIdMan(d->url + "/tok", d->url + "/u/tok", d->qnam, this);
    cuprintf("CumbiaHttp: instantiated with url %s\n", qstoc(url));
    m_init();
}

CumbiaHttp::~CumbiaHttp()
{
    pdelete("~CumbiaHttp %p", this);
    d->chan_recv->stop();
    // deleted CuHttpControlsReaders will have enqueued their unsubscribe requests
    QList<SrcItem> ri, wi, ui;
    d->src_q_man->dequeueItems(ri, wi, ui);
    printf("~CumbiaHttp: calling onSrcBundleReqReady *blocking* with sizes %d write %d\n", ri.size(), wi.size());
    onSrcBundleReqReady(ri, wi, ui);
    printf("~CumbiaHttp: calling d->id_man->unsubscribe *blocking* ");
    d->id_man->unsubscribe(true); // true: block
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(d->m_threadsEventBridgeFactory)
        delete d->m_threadsEventBridgeFactory;
    if(d->m_threadFactoryImplI)
        delete d->m_threadFactoryImplI;
    delete d->w_helper;
    delete d->qnam;
    foreach(QuReplaceWildcards_I *i, d->m_repl_wildcards_i)
        delete i;
    foreach(CuHttpSrcHelper_I *sh, d->src_helpers)
        delete sh;
    d->src_helpers.clear();
    d->m_repl_wildcards_i.clear();
    delete d->id_man;
    delete d;
}

void CumbiaHttp::m_init()
{
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuHTTPActionFactoryService::CuHTTPActionFactoryServiceType),
                                          new CuHTTPActionFactoryService());
}

// if there is a src item among rsrcs that needs the client id and we don't have it yet, request
// the client id first, and then process the list of SrcItem. "s" and "u" are requests submitted
// to the async client and need client ID
//
void CumbiaHttp::onSrcBundleReqReady(const QList<SrcItem> &rsrcs,
                                     const QList<SrcItem> &wsrcs,
                                     const QList<SrcItem>& usrcs) {
    if(d->client_id > 0 || !m_need_client_id(rsrcs))
        m_start_bundled_src_req(rsrcs, wsrcs, usrcs);
    else {
        d->request_q.add(rsrcs, wsrcs, usrcs);
        m_start_client_id_req();
    }
}

void CumbiaHttp::m_start_bundled_src_req(const QList<SrcItem> &rsrcs,
                                         const QList<SrcItem> &wsrcs,
                                         const QList<SrcItem> &unsubsrcs)
{
    if(rsrcs.size() > 0) {
        CuHttpBundledSrcReq * r = new CuHttpBundledSrcReq(rsrcs, this, d->client_id);
        r->setBlocking(d->chan_recv->exiting()); // destruction in progress
        r->start(d->url + "/bu/src-bundle", d->qnam);
    }
    if(unsubsrcs.size() > 0) {
        CuHttpBundledSrcReq * r = new CuHttpBundledSrcReq(unsubsrcs, this, d->client_id);
        r->setBlocking(d->chan_recv->exiting()); // destruction in progress
        r->start(d->url + "/u/bu/src-bundle", d->qnam);
    }
    if(wsrcs.size() > 0) {
        CuHttpBundledSrcReq * r = new CuHttpBundledSrcReq(wsrcs, this);
        r->start(d->url + "/bu/xec-bundle", d->qnam);
    }
}

void CumbiaHttp::onSrcBundleReplyReady(const QByteArray &json) {
    CumbiaHTTPWorld w;
    std::list<CuData> dali;
    bool ok = w.json_decode(json, dali);
    for(std::list<CuData>::iterator it = dali.begin(); ok && it != dali.end(); ++it)
        m_data_is_auth_req(*it) ? m_auth_request(*it) :  m_lis_update(*it);
}

/*!
 * \brief notify every listener of a http server/gateway general error
 *
 * \param errd data provided by CuHttpBundledSrcReq::onError after getting an error from an http request
 *
 * For each listener of a given source (or target), an error message is built and delivered.
 * It will be made up of
 * - the error from the http server
 * - the "data" (web page) received by the web server
 * - the portion of the original request for the listener, extracted from the whole request payload,
 *   which in principle contains a JSon array of requests bundled together.
 */
void CumbiaHttp::onSrcBundleReplyError(const CuData &errd) {
    QMap<QString, QString> revmap;
    QString chan;
    CumbiaHTTPWorld().request_reverse_eng(errd["payload"].toString().c_str(), revmap, chan);
    const QMap<QString, SrcData> &ma = d->src_q_man->takeSrcs(), &tma = d->src_q_man->takeTgts();
    CuData dat("err",true);
    std::string msg = errd["msg"].toString();
    msg += "\ndata:\"" + errd["data"].toString() + "\"";
    dat["msg"] = msg;
    dat.putTimestamp();
    foreach(const QString& s, ma.keys())  { // sources
        if(ma[s].lis) ma[s].lis->onUpdate(m_make_server_err(revmap, s, dat));
    }
    foreach(const QString& s, tma.keys())  { // targets
        if(ma[s].lis) ma[s].lis->onUpdate(m_make_server_err(revmap, s, dat));
    }
}

void CumbiaHttp::readEnqueue(const CuHTTPSrc &source, CuDataListener *l, const CuHTTPActionFactoryI& f) {
    d->src_q_man->enqueueSrc(source, l, f.getMethod(), d->chan_recv->channel(), CuVariant(), f.options());
}

void CumbiaHttp::unsubscribeEnqueue(const CuHTTPSrc &httpsrc, CuDataListener *l) {
        d->src_q_man->enqueueSrc(httpsrc, l, "u", d->chan_recv->channel(), CuVariant(), CuData());
}

void CumbiaHttp::executeWrite(const CuHTTPSrc &source, CuDataListener *l, const CuHTTPActionFactoryI &f) {
    d->src_q_man->enqueueSrc(source, l, f.getMethod(), "", f.options().value("write_val"), f.options());
}

/*!
 * \brief disconnect the listener l of the given source
 * \param source the full name of the source, as from CuHTTPSrc.prepare
 * \param l the CuDataListener to disconnect
 *
 * \note
 * Disconnect the listener both from a pending sync reply (onSrcBundleReplyReady) and
 * the channel receiver
 *
 * \note
 * called by CuHttpControlsR
 */
void CumbiaHttp::unlinkListener(const CuHTTPSrc &source, const std::string& method, CuDataListener *l) {
    if(CumbiaHTTPWorld().source_valid(source.getName())) {
        // cancelSrc never sends "unsubscribe"
        d->src_q_man->cancelSrc(source, method, l, d->chan_recv->channel());
        d->chan_recv->removeDataListener(l);
    }
}

/*!
 * \brief add a QuReplaceWildcards_I instance
 * \par Note
 * Ownership is handed to CumbiaHttp and objects are deleted in ~CumbiaHttp
 */
void CumbiaHttp::addReplaceWildcardI(QuReplaceWildcards_I *rwi) {
    d->m_repl_wildcards_i << rwi;
}

/*!
 * \brief Add an implementation of CuHttpSrcHelper_I to help processing and preparing the sources
 *        before sending them through the http module.
 * \param srch a pointer to a valid CuHttpSrcHelper_I instance.
 * \par Ownership
 * Ownership is handed to CumbiaHttp and CuHttpSrcHelper_I is deleted within the CumbiaHttp destructor.
 */
void CumbiaHttp::addSrcHelper(CuHttpSrcHelper_I *srch) {
    d->src_helpers.append(srch);
}

QList<QuReplaceWildcards_I *> CumbiaHttp::getReplaceWildcard_Ifaces() const{
    return d->m_repl_wildcards_i;
}

QList<CuHttpSrcHelper_I *> CumbiaHttp::getSrcHelpers() const {
    return d->src_helpers;
}

/*!
 * \brief Change the message time to live on the channel
 * \param secs number of seconds
 * @see chanMsgTtl
 */
void CumbiaHttp::setChanMsgTtl(int secs) {
    return d->chan_recv->setDataExpireSecs(static_cast<time_t>(secs));
}

/*!
 * \brief Messages received from the channel that are older than this value in seconds are discarded
 *
 * \par Default value
 * DEFAULT_CHAN_MSG_TTL value configured in cumbia-http.pro (seconds)
 * \par Command line argument
 * --ttl=X or --chan-msgs-ttl=X are recognized by CuHttpRegisterEngine and allow tuning the value
 * at startup.
 */
int CumbiaHttp::chanMsgTtl() const {
    return static_cast<time_t>(d->chan_recv->dataExpiresSecs());
}

CuThreadFactoryImplI *CumbiaHttp::getThreadFactoryImpl() const
{
    return d->m_threadFactoryImplI;
}

CuThreadsEventBridgeFactory_I *CumbiaHttp::getThreadEventsBridgeFactory() const {
    return d->m_threadsEventBridgeFactory;
}

QString CumbiaHttp::url() const {
    return d->url;
}

int CumbiaHttp::getType() const {
    return CumbiaHTTPType;
}

bool CumbiaHttp::m_data_is_auth_req(const CuData &da) const {
    return da.containsKey("auth_url") && da.has("method", "write");
}

void CumbiaHttp::onCredsReady(const QString &user, const QString &passwd) {
    qDebug() << __PRETTY_FUNCTION__ << user << passwd;
    if(user.isEmpty()) {
        // unrecoverable, notify listeners and remove targets from src_q_man
        CuData err = d->w_helper->makeErrData("invalid user name").set("is_result", true);
        const QMap<QString, SrcData> &tm = d->src_q_man->takeTgts();
        foreach(const QString& src, tm.keys()) {
            qDebug() << __PRETTY_FUNCTION__ << "tgt" << src << "msg" << "invalid user nam";
            if(tm[src].lis) tm[src].lis->onUpdate(err.set("src", src.toStdString()));
        }
    }
    else
        d->auth_man->tryAuthorize(user, passwd);
}

void CumbiaHttp::onAuthReply(bool authorised, const QString &user, const QString &message, bool encrypted) {
    qDebug () << __PRETTY_FUNCTION__ << authorised << user << message << encrypted;
    if(authorised) {
        // restart writes
        QByteArray cookie = d->auth_man->getCookie();
        CuHttpBundledSrcReq * r = new CuHttpBundledSrcReq(d->src_q_man->targetMap(), this, cookie);
        r->start(d->url + "/bu/xec-bundle", d->qnam);
    }
    else {
        CuData err = d->w_helper->makeErrData(message).set("is_result", true).set("user", user.toStdString());
        const QMap<QString, SrcData>& tamap = d->src_q_man->takeTgts();
        foreach(const QString& src, tamap.keys()) {
            qDebug() << __PRETTY_FUNCTION__ << "tgt" << src << "msg" << message;
            if(tamap[src].lis) tamap[src].lis->onUpdate(err.set("src", src.toStdString()));
        }
    }
}

void CumbiaHttp::onAuthError(const QString &errm) {
    qDebug() << __PRETTY_FUNCTION__ << errm;
    CuData err = d->w_helper->makeErrData(errm).set("is_result", true);
    const QMap<QString, SrcData> &tm = d->src_q_man->takeTgts();
    foreach(const QString& src, tm.keys()) {
        if(tm[src].lis) tm[src].lis->onUpdate(err.set("src", src.toStdString()));
    }
}

void CumbiaHttp::m_auth_request(const CuData &da) {
    if(!d->w_helper)
        d->w_helper = new CuHttpWriteHelper();
    d->auth_man->authPrompt(QuString(da, "auth_url"), false); // false: use dialog
}

void CumbiaHttp::m_lis_update(const CuData &da) {
    const std::string &src = da.value("src").toString();
    QList<SrcData> tgtli;
    const QMap<QString, SrcData> &mp = d->src_q_man->takeTgts();
    foreach(const QString& tgt, mp.keys()) {
        tgtli.push_back(mp.value(tgt));
    }
    const QList<SrcData> &dali = d->src_q_man->takeSrcs(QString::fromStdString(src)) + tgtli;

    foreach(const SrcData& srcd, dali) {
        // update listener but not if method is "u": at this time it will have been deleted
        if(srcd.lis && srcd.method != "u") srcd.lis->onUpdate(da);
        if(srcd.method == "s")
            d->chan_recv->addDataListener(QString::fromStdString(src), srcd.lis);
        else if(srcd.method == "u") {
            d->chan_recv->removeDataListener(srcd.lis);
        }
    }
}

CuData CumbiaHttp::m_make_server_err(const QMap<QString, QString>& revmap, const QString &src, const CuData &in) const
{
    CuData out(in);
    const std::string& req = revmap.contains(src) ? "\nrequest:\"" + revmap[src].toStdString() + "\" part of a bundle of "
            + std::to_string(revmap.size()) + " requests." : "\nrequest unavailable";
    out["msg"] = in["msg"].toString() + req;
    return out;
}
void CumbiaHttp::m_dequeue_src_reqs() {
    m_start_bundled_src_req(d->request_q.rreqs, d->request_q.wreqs, d->request_q.ureqs);
    d->request_q.empty_q();
}

void CumbiaHttp::m_start_client_id_req() {
    d->id_man->start();
}

bool CumbiaHttp::m_need_client_id(const QList<SrcItem> &rsrcs) const
{
    bool needs_id = false;
    QListIterator<SrcItem> it(rsrcs);
    while(!needs_id && it.hasNext()) {
        const SrcItem &i = it.next();
        needs_id = i.method == "s" || i.method == "u";
    }
    return needs_id;
}

void CumbiaHttp::onIdReady(const unsigned long long &client_id, const time_t ttl) {
    d->client_id = client_id;
    printf("CumbiaHttp.onIdReady: got client id \e[1;32m%llu\e[0m, ttl %ld calling m_dequeue_src_reqs\n", client_id, ttl);
    m_dequeue_src_reqs(); // process requests waiting for client_id
}

void CumbiaHttp::onIdManError(const QString &err) {
    perr("CumbiaHttp.onIdManError: client id manager error: %s", qstoc(err));
}
