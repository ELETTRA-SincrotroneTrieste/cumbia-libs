#ifndef CUMBIAHTTP_H
#define CUMBIAHTTP_H

#include <cumbia.h>
#include <string>
#include <cuhttpactionreader.h>
#include <cuhttpactionfactoryi.h>
#include <QString>

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
 */
class CumbiaHttp : public Cumbia, public CuHTTPActionListener
{

public:
    enum Type { CumbiaHTTPType = Cumbia::CumbiaUserType + 18 };

    CumbiaHttp(const QString& url, const QString &channel, CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaHttp();

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    QString url() const;

    virtual int getType() const;

    void addAction(const std::string &source, CuDataListener *l, const CuHTTPActionFactoryI &f);
    void unlinkListener(const string &source, CuHTTPActionA::Type t, CuDataListener *l);
    CuHTTPActionA *findAction(const std::string &source, CuHTTPActionA::Type t) const;

    void addReplaceWildcardI(QuReplaceWildcards_I *rwi);
    void addSrcHelper(CuHttpSrcHelper_I *srch);

    QList<QuReplaceWildcards_I *> getReplaceWildcard_Ifaces() const;
    QList<CuHttpSrcHelper_I *>getSrcHelpers() const;

private:

    void m_init();

    CumbiaHttpPrivate *d;


    // CuHTTPActionListener interface
public:
    void onActionStarted(const string &source, CuHTTPActionA::Type t);
    void onActionFinished(const string &source, CuHTTPActionA::Type t);
};

#endif // CUMBIAHTTP_H
