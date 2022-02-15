#ifndef CUMBIAHTTP_H
#define CUMBIAHTTP_H

#include <cumbia.h>
#include <string>
#include <cuhttpactionfactoryi.h>
#include <QString>

#include "cuhttpsrcman.h"
#include "cuhttpbundledsrcreq.h"
#include "cuhttpauthmanager.h"
#include "cuhttpcliidman.h"

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuDataListener;
class QuReplaceWildcards_I;
class CuHttpSrcHelper_I;

class CumbiaHttpPrivate;
/*!
 * \mainpage Cumbia <em>http</em> module
 *
 * The *cumbia http* module provides an interface to send and receive data through *http[s]*
 * and *Server Sent Events* (SSE).
 * The library relies on the *canone3* server (caserver):
 *
 * - The <a href="https://gitlab.com/PWMA/">PWMA</a> project
 *
 * The *cumbia http* module has been developed in order to build applications for <strong>desktop, mobile devices
 * and web applications</strong>, enabled to interact with the Tango and EPICS control systems through the
 * aforementioned service.
 *
 * Desktop applications written with cumbia will run transparently either relying on a native engine or
 * the *http* module. They do not require engine specific code, nor they require rebuilding. Additionally,
 * the sources specified as command line arguments need not be changed across different engines.
 * Engine independent apps are built using the CuModuleLoader class available since version 1.2.0:
 *
 * qmake project.pro file:
 * \code
  isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT=/usr/local/cumbia-libs
  }
  include($${CUMBIA_ROOT}/include/quapps/quapps.pri)
  \endcode

 * cpp:
 * \code
    #include <quapps.h>

    MyQtApp::MyQtApp(CumbiaPool *cumbia_pool, QWidget *parent) :
        QWidget(parent),
        cu_pool(cumbia_pool),
        ui(new Ui::HttpTest) {
        // load available module
        CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
        ui->setupUi(this, cumbia_pool, m_ctrl_factory_pool);
    }
 *\endcode
 *
 * Sources specified on the command line need not be changed, but the http URL and the channel where
 * the *http* application receives server sent events must be given, unless the utility
 *
 * \code
 * cumbia  apps module-default [set]
 * \endcode
 *
 * is used to set up an environment where *http* module is used by default. This is useful on workstations
 * where *native* engines are unavailable.
 *
 * CumbiaHttp is a CuHTTPActionListener in order to be notified when an action is finished, so that it can
 * be unregistered and deleted.
 *
 * \section tech_notes Technical notes
 * Either read or write operation will call CumbiaHttp::addAction with the complete name of the source, a pointer to a
 * CuDataListener and a const reference to either a CuHTTPActionReaderFactory or a CuHTTPActionWriterFactory, respectively.
 * See CuHttpControlsReader. The factories may contain options. A relevant one for a reader is the *single-shot*,
 * that reads a quantity once.
 *
 * CumbiaHttp::addAction enqueues
 * the request so that multiple requests are gathered together and sent in bundles, saving the number of http requests
 * to the server. CuHttpSrcMan is where source subscriptions are enqueued and a timer quickly dequeues them so that they
 * are packed in a small number of http requests. When a bundle is ready, CumbiaHttp::onSrcBundleReqReady starts an http
 * request to the server, with the multiple subscription requests represented in Json format.
 *
 * Whatever the operation, single read, monitor, write... *a synchronous reply is always expected*. In the *monitor* case,
 * subsequent updates are received through an SSE channel. The server will recollect results for each request and pack them
 * in a reply which will contain either a value (a reading, configuration properties) or an error.
 * In the *monitor* example, a request will contain a *subscribe* option. The *synchronous* reply will provide the first
 * read value (or configuration) *or* an error condition. This implies that if one or more amongst the *n* sources takes
 * time before being available, all sources in the bundle will be delivered late. Updates are distributed across the SSE channel.
 */
class CumbiaHttp : public Cumbia,
        public CuHttpBundledSrcReqListener,
        public CuHttpCliIdManListener,
        public CuHttpSrcQueueManListener,
        public CuHttpAuthManListener
{

public:
    enum Type { CumbiaHTTPType = Cumbia::CumbiaUserType + 18 };

    CumbiaHttp(const QString& url, const QString &channel, CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);
    ~CumbiaHttp();

    void setTag(const QString& tag);

    CuThreadFactoryImplI* getThreadFactoryImpl() const;
    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;
    QString url() const;
    virtual int getType() const;

    void readEnqueue(const CuHTTPSrc &source, CuDataListener *l, const CuHTTPActionFactoryI &f);
    void unsubscribeEnqueue(const CuHTTPSrc &httpsrc, CuDataListener *l);
    void executeWrite(const CuHTTPSrc &source, CuDataListener *l, const CuHTTPActionFactoryI &f);

    void unlinkListener(const CuHTTPSrc &source, const std::string& method, CuDataListener *l);

    void addReplaceWildcardI(QuReplaceWildcards_I *rwi);
    void addSrcHelper(CuHttpSrcHelper_I *srch);

    QList<QuReplaceWildcards_I *> getReplaceWildcard_Ifaces() const;
    QList<CuHttpSrcHelper_I *>getSrcHelpers() const;

    void setChanMsgTtl(int secs);
    int chanMsgTtl() const;

private:

    void m_init();
    bool m_data_is_auth_req(const CuData& da) const;
    void m_auth_request(const CuData& da);
    void m_lis_update(const CuData& da);
    CuData m_make_server_err(const QMap<QString, QString> &revmap, const QString& r, const CuData& in) const;
    void m_start_bundled_src_req(const QList<SrcItem> &rsrcs, const QList<SrcItem> &wsrcs);
    void m_dequeue_src_reqs();
    void m_start_client_id_req();
    bool m_need_client_id(const QList<SrcItem> &rsrcs) const;

    CumbiaHttpPrivate *d;

    // CuHttpSrcQueueManListener interface
public:
    void onSrcBundleReqReady(const QList<SrcItem>& rsrcs, const QList<SrcItem>& wsrcs);

    // CuHttpBundledSrcReqListener interface
public:
    void onSrcBundleReplyReady(const QByteArray &json);
    void onSrcBundleReplyError(const CuData &errd);

    // CuHttpAuthManListener interface
public:
    void onCredsReady(const QString &user, const QString &passwd);
    void onAuthReply(bool authorised, const QString &user, const QString &message, bool encrypted);
    void onAuthError(const QString &errm);

    // CuHttpCliIdManListener interface
private:
    void onIdReady(const unsigned long long &client_id, const time_t ttl);
    void onIdManError(const QString& err);
};

#endif // CUMBIAHTTP_H
