#include "qumbia-reader.h"
#include "cmdlineoptions.h"
#include "kbdinputwaitthread.h"

#include <cumbiapool.h>
#include <qustring.h>
#include <qustringlist.h>
#include <cucontextactionbridge.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>
#include <cutimerservice.h>
#include <qureader.h>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDateTime>
#include <QtDebug>

// plugin
#include <cupluginloader.h>
#include <cuformulaplugininterface.h>

#ifdef HAS_CUHDB
#include <cuhistoricaldbplugin_i.h>
#else
class CuHdbPlugin_I;
#endif


#ifdef QUMBIA_EPICS_CONTROLS_VERSION
#include <cumbiaepics.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cuepics-world.h>
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cumbiatango.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cutcontrols-utils.h>
#include <cutangoreplacewildcards.h>
#endif
#ifdef CUMBIA_RANDOM_VERSION
#include <cumbiarandom.h>
#include <curndreader.h>
#include <curndactionfactories.h>
#include <cumbiarndworld.h>
#endif

#ifdef CUMBIA_WEBSOCKET_VERSION
#include <cumbiawebsocket.h>
#include <cumbiawsworld.h>
#include <cuwscontrolsreader.h> // for CuWSReaderFactory
#include <cuwstangoreplacewildcards.h>
#endif

#ifdef CUMBIA_HTTP_VERSION
#include <cumbiahttp.h>
#include <cumbiahttpworld.h>
#include <cuhttpcontrolsreader.h> // for CuHttpReaderFactory
#include <cuhttpregisterengine.h>
#include <cuhttptangoreplacewildcards.h>
#endif

QumbiaReader::QumbiaReader(CumbiaPool *cumbia_pool, QWidget *parent) :
    QObject(parent)
{
    QStringList engines;
    cu_pool = cumbia_pool;

    // plugins, instantiate if possible. Initialize only after engines.
    CuPluginLoader pload;
    QObject *plugin_qob;
    CuFormulaPluginI *fplu = pload.get<CuFormulaPluginI>("cuformula-plugin.so", &plugin_qob);
    CuHdbPlugin_I *hdb_p;

#ifdef HAS_CUHDB
    // historical database
    QObject *hdb_o;
    hdb_p = pload.get<CuHdbPlugin_I>("cuhdb-qt-plugin.so", &hdb_o);
    hdb_p = pload.get<CuHdbPlugin_I>("cuhdb-qt-plugin.so", &hdb_o);
#endif
    // parse configuration
    CmdLineOptions cmdo(fplu != nullptr, hdb_p != nullptr);
    m_conf = cmdo.parse(qApp->arguments());

#ifdef QUMBIA_EPICS_CONTROLS_VERSION
    CumbiaEpics* cuep = nullptr;
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    CumbiaTango* cuta = nullptr;
#endif

    if(m_conf.url.startsWith("wss://") || m_conf.url.startsWith("ws://")) {
#ifdef CUMBIA_WEBSOCKET_VERSION
        printf("activating cumbia websocket...ws url %s\n", qstoc(m_conf.url));
        // setup Cumbia web socket with the web socket address and the host name to prepend to the sources
        // for the HTTP requests
        CumbiaWSWorld wsw;
        QUrl u(m_conf.url);
        CumbiaWebSocket* cuws = new CumbiaWebSocket(u.toString(), "", new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        cu_pool->registerCumbiaImpl("ws", cuws);
        cu_pool->setSrcPatterns("ws", wsw.srcPatterns());
        m_ctrl_factory_pool.setSrcPatterns("ws", wsw.srcPatterns());
        m_ctrl_factory_pool.registerImpl("ws", CuWSReaderFactory());
        cuws->openSocket();
        CuWsTangoReplaceWildcards *tgrwi = new CuWsTangoReplaceWildcards;
        cuws->addReplaceWildcardI(tgrwi);
        engines << "websocket";
#else
        perr("QumbiaReader: module cumbia-websocket is not available");
#endif
    }
    else if(m_conf.url.startsWith("https://") || m_conf.url.startsWith("http://")) {
#ifdef CUMBIA_HTTP_VERSION
        CuHttpRegisterEngine httpre;
        QString url, chan = m_conf.url.section(QRegularExpression("[^:^/]/"), -1); // match last token after a / but skip http[s]://
        chan != m_conf.url ? url = m_conf.url.remove(m_conf.url.lastIndexOf('/'), chan.length() + 1) : chan.remove(0, chan.length());
        if(url.contains(QRegularExpression("http[s]{0,1}://.*")) && chan.isEmpty())
            printf("\e[1;33m*\e[0m cumbia read: channel not detected in URL: required form: \"%s/\e[1;33mchannel_name\e[0m\"\n",
                   qstoc(url));
        httpre.setUrl(url);
        httpre.setChannel(chan);
        CumbiaHttp *cuhttp = httpre.registerWithDefaults(cu_pool, m_ctrl_factory_pool);
        printf("activating cumbia http... url %s channel %s\n", qstoc(url), qstoc(chan));
#else
        perr("QumbiaReader: module cumbia-http is not available");
#endif
    }

    // setup Cumbia pool and register cumbia implementations for tango and epics
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
    cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
    cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());
    engines << "epics";
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
    engines << "tango";
#endif


#ifdef CUMBIA_RANDOM_VERSION
    CumbiaRandom *cura = new CumbiaRandom(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaRNDWorld rndw;
    cu_pool->registerCumbiaImpl("random", cura);
    m_ctrl_factory_pool.registerImpl("random", CuRNDReaderFactory());
    m_ctrl_factory_pool.setSrcPatterns("random", rndw.srcPatterns());
    cu_pool->setSrcPatterns("random", rndw.srcPatterns());
    cura->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    engines << "random";
#endif

    // formulas. load after engines

    if(fplu) {
        fplu->initialize(cu_pool, m_ctrl_factory_pool);
        engines << "formula plugin";
    }


#ifdef HAS_CUHDB
    if(hdb_p) {
        cu_pool->registerCumbiaImpl("hdb", hdb_p->getCumbia());
        cu_pool->setSrcPatterns("hdb", hdb_p->getSrcPatterns());
        m_ctrl_factory_pool.registerImpl("hdb", *hdb_p->getReaderFactory());
        engines << "historical database";
    }
#endif

    m_props_map[Low] = QStringList() << "min" << "min_alarm" << "min_warning" << "max_warning" <<
                                        "max_alarm" << "max" << "data_format_str" << "display_unit"
                                     << "label" << "description";
    m_props_map[Medium] = QStringList() << "activity" << "worker_activity" << "worker_thread";


#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    if(cuta && m_conf.max_timers > 0) {
        CuTimerService *ts = static_cast<CuTimerService *>(cuta->getServiceProvider()->get(CuServices::Timer));
        ts->setTimerMaxCount(m_conf.max_timers);
    }
#endif
    if(m_conf.usage) {
        cmdo.usage(qApp->arguments().first());
        printf("\nAvailable engines: %s\n\n", qstoc(engines.join(", ")));
    }
    else if(m_conf.list_options)
        cmdo.list_options();
#ifdef HAS_CUHDB
    if(hdb_p && !m_conf.db_profile.isEmpty()) {
        hdb_p->setDbProfile(m_conf.db_profile);
    }
    else if(hdb_p && !m_conf.list_options){
        printf("\e[1;33m* \e[0;4mcumbia read\e[0m: using \e[1;33mdefault\e[0m historical database profile, if available\n");
    }
#endif

    if(!m_conf.usage && !m_conf.list_options)
        m_createReaders(m_conf.sources);

    if(m_conf.refresh_limit < 1) {
        // wait for keyboard input to stop
        printf("\e[1;32m * \e[0;4mmonitor\e[0m started: press \e[1;32many key\e[0m to \e[1;32mexit\e[0m\n");
        KbdInputWaitThread *kbdt = new KbdInputWaitThread(this);
        connect(kbdt, SIGNAL(finished()), qApp, SLOT(quit()));
        kbdt->start();
    }
}

QumbiaReader::~QumbiaReader() {
}

QString QumbiaReader::makeTimestamp(const double d) const {
    QString ts = QDateTime::fromSecsSinceEpoch(static_cast<int>(d)).toString("HH:mm:ss");
    ts += QString("+%1").arg(d - static_cast<int>(d));
    return ts;
}

bool QumbiaReader::usage_only() const {
    return m_conf.usage || m_conf.list_options;
}

void QumbiaReader::onNewDouble(const QString& src, double ts, double val, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s]  [\e[1;36mdouble\e[0m] \e[1;32m%s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<double>(val, "%.2f")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewFloat(const QString &src, double ts, float val, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s]  [\e[1;36mfloat\e[0m] \e[1;32m%s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<float>(val, "%.2f")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewDoubleVector(const QString &src, double ts, const QVector<double> &v, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;36mdouble\e[0m,%d] { ",  qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<double>(v[i], "%.2f")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    printf("%s \e[0m}", qstoc(m_format<double>(v[v.size() - 1], "%.2f")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewFloatVector(const QString &src, double ts, const QVector<float> &v, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;36mfloat[0m,%d] { ",  qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<float>(v[i], "%.2f")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    printf("%s \e[0m}", qstoc(m_format<float>(v[v.size() - 1], "%.2f")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewBoolVector(const QString &src, double ts, const QVector<bool> &v, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;35mbool[0m,%d] { ",  qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", v[i] ? "TRUE" : "FALSE");
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    printf("%s \e[0m}", v[v.size() - 1] ? "TRUE" : "FALSE");
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewShort(const QString &src, double ts, short val, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mshort\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<short>(val, "%d")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewBool(const QString &src, double ts, bool val, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;35mbool\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), val ? "TRUE" : "FALSE");
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewUShort(const QString &src, double ts, unsigned short val, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;34munsigned short\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<unsigned short>(val, "%d")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewLong(const QString &src, double ts, long val, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mlong\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<long>(val, "%ld")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewULong(const QString &src, double ts, unsigned long val, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;34munsigned long\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<unsigned long>(val, "%lu")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewString(const QString &src, double ts, const QString &val, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;36mstring\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), qstoc(val));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onStringConversion(const QString &src, const QString &fromType, double timestamp_us, const QString &v, const CuData &da) {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;31m%s-->string\e[0m] [\e[1;36mstring\e[0m] %s\e[0m",
           qstoc(makeTimestamp(timestamp_us)), qstoc(fromType), qstoc(v));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewLongVector(const QString &src, double ts, const QVector<long> &v, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;35mlong\e[0m,%d] { ", qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<long>(v[i], "%ld")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    if(v.size() > 0)
        printf("%s \e[0m}", qstoc(m_format<long>(v[v.size() - 1], "%ld")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewULongVector(const QString &src, double ts, const QVector<unsigned long> &v, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;35munsigned long\e[0m,%d] { ", qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<unsigned long>(v[i], "%lu")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    if(v.size() > 0)
        printf("%s \e[0m}", qstoc(m_format<unsigned long>(v[v.size() - 1], "%lu")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewStringList(const QString &src, double ts, const QStringList &val, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mstring\e[0m,%d] { ", qstoc(makeTimestamp(ts)), val.size());
    for(int i = 0; i < val.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(val[i]));
    if(m_conf.truncate > -1 && m_conf.truncate < val.size())
        printf(" ..., ");
    if(val.size() > 0)
        printf("%s \e[0m}", qstoc(val[val.size() - 1]));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onStringListConversion(const QString &src, const QString &fromType,
                                          double timestamp_us, const QStringList &v, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;31m%s-->string list\e[0m,%d] [\e[1;36mstring\e[0m]\e[0m",
           qstoc(makeTimestamp(timestamp_us)), qstoc(fromType), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(v[i]));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    if(v.size() > 0)
        printf("%s \e[0m}", qstoc(v[v.size() - 1]));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewHdbData(const QString &src, const CuData &hdbd)
{
    m_hdbHelper.mergeResult(src, hdbd);
    if(m_hdbHelper.allComplete()) {
        m_refreshCntMap[qobject_cast<Qu_Reader *>(sender())->source()]++;
        m_hdbHelper.print_all(m_hdbHelper.takeAll(), m_conf.db_output_file);
        // all complete, all printed, delete all
        foreach(Qu_Reader *r, findChildren<Qu_Reader *>())
            r->deleteLater();
        if(m_conf.db_output_file.isEmpty())
            printf("\e[1;33m*\e[0m \e[0;4mcumbia read\e[0m: \e[1;33mhint\e[0m:  use \e[3m--db-output-file=out.csv\e[0m "
                   "command line option to save data on a \e[3mCSV\e[0m file.\n");
    }
}

void QumbiaReader::onNewShortVector(const QString &src, double ts, const QVector<short> &v, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mshort\e[0m,%d] { ", qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<short>(v[i], "%u")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    if(v.size() > 0)
        printf("%s \e[0m}", qstoc(m_format<short>(v[v.size() - 1], "%u")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewUShortVector(const QString &src, double ts, const QVector<unsigned short> &v, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;34munsigned short\e[0m,%d] { ", qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<unsigned short>(v[i], "%u")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    if(v.size() > 0)
        printf("%s \e[0m}", qstoc(m_format<unsigned short>(v[v.size() - 1], "%u")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onPropertyReady(const QString &src, double ts,const CuData &pr)
{
    m_refreshCntMap[src]++;
    printf("\n \e[1;36m*\e[0m ");
    if(m_conf.verbosity > Low && pr.containsKey("thread"))
        printf("[\e[1;36m%s\e[0m] ", pr["thread"].toString().c_str());
    // src and timestamp
    printf("%s [%s] ", qstoc(src), qstoc(makeTimestamp(ts)));

    printf("\n - \e[1;32m%s\e[0m\n", vtoc2(pr, "device"));


    if(pr.containsKey("keys"))
        m_print_list_props(pr);
    else
        m_print_property(pr);

    m_checkRefreshCnt(sender());

    if(m_conf.verbosity == Debug)
        printf("\n\e[0;35m--l=debug\e[0m: \e[1;35m{\e[0m %s \e[1;35m}\e[0m\n", pr.toString().c_str());
}

void QumbiaReader::onError(const QString &src, double ts, const QString &msg, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    perr("[%s]: \"\e[1;31m%s\e[0m\"\n", qstoc(makeTimestamp(ts)), qstoc(msg));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onReaderDestroyed(QObject *o)
{
    m_readers.removeAll(o);
    if(m_readers.isEmpty())
        qApp->quit();
}

void QumbiaReader::m_print_extra1(const CuData &da)
{
    //    printf("\e[1;35m%s\e[0m\n", da.toString().c_str());
    printf(" \e[1;36m*\e[0m ");

    if(m_conf.verbosity == Debug) {
        printf("\n\e[0;35m--l=debug\e[0m: \e[1;35m{\e[0m %s \e[1;35m}\n", da.toString().c_str());
    }

    // label
    if(da.containsKey("label")) {
        printf("\e[1;32;4m%s\e[0m: ", da["label"].toString().c_str());
    }
    else {
        printf("\e[1;32;4m%s\e[0m: ", da["src"].toString().c_str());
    }

    if(m_conf.verbosity >= High) {
        if(da.containsKey("thread"))
            printf("[\e[1;36m%s\e[0m] ", da["thread"].toString().c_str());
    }
    if(m_conf.verbosity >= Medium) {
        if(da.containsKey("min")) {
            double min;
            da["min"].to<double>(min);
            printf("\e[1;31m%s\e[1;35m < \e[0m", qstoc(m_format<double>(min, "%.2f")));
        }
    }
}

void QumbiaReader::m_print_extra2(const CuData &da)
{
    if(da.containsKey("label") && m_conf.verbosity >= Medium) {
        // print full source name
        printf(" [\e[1;36m%s\e[0m]", da["src"].toString().c_str());
    }
    if(m_conf.verbosity >= Medium) {
        if(da.containsKey("min")) {
            double max;
            da["max"].to<double>(max);
            printf("\e[1;35m < \e[1;31m%s \e[0m", qstoc(m_format<double>(max, "%.2f")));
        }
        if(da["display_unit"].toString().size() > 0)
            printf("[%s] ", da["display_unit"].toString().c_str());
        if(da.containsKey("elapsed_us"))
            printf("[in %ldus] ", da["elapsed_us"].toLongInt());

    }
    printf("\n");
}

void QumbiaReader::m_checkRefreshCnt(QObject *o)
{
    Qu_Reader * r = qobject_cast<Qu_Reader *>(o);
    if(r && m_conf.refresh_limit > 0 && m_refreshCntMap[r->source()] >= m_conf.refresh_limit) {
        r->stop();
        o->deleteLater();
    }
}

void QumbiaReader::m_createReaders(const QStringList &srcs) {
    if(m_conf.tango_property) {
        Qu_Reader *r = new Qu_Reader(this, cu_pool, m_ctrl_factory_pool);
        r->setTgPropertyList(srcs);
        connect(r, SIGNAL(newError(QString,double,QString, const CuData&)),
                this, SLOT(onError(QString,double,QString, const CuData&)));
        r->getTgProps();
    }
    else {
        for(int i = 1; i < srcs.size(); i++)
        {
            // square brackets in bash shell are OK
            // if present, replace them with ()
            // [A-Za-z0-9\-_\.,/]+(?:->|/)[A-Za-z0-9\-_\.,]+(?:\[.*\]){0,1}
            // match for example aa/bb/cc:command[1,2] and replace with
            // aa/bb/cc->command(1,2)
            // t/de/1[values] --> t/de/1(description,values) <-- this is a device property read
            QRegularExpression squarebrackets_src("[A-Za-z0-9\\-_\\.,/]+(?:->|/)[A-Za-z0-9\\-_\\.,]+(?:\\[.*\\]){0,1}");
            CuData reader_ctx_options;
            QString a = m_conf.sources.at(i);
            // do not replace domain:// with domain-> !
            std::string save_dom;
            std::vector<std::string> doms = cu_pool->names();
            for(size_t i = 0; i < doms.size() && save_dom.length() == 0; i++) {
                const std::string& dom = doms[i];
                if(a.startsWith(QString::fromStdString(dom) + "://"))
                    save_dom = dom + "://";
            }
            if(save_dom.length() > 0)
                a.remove(QString::fromStdString(save_dom));
            a.replace("//", "->");
            a = QString("%1%2").arg(save_dom.c_str()).arg(a);
            QRegularExpressionMatch match = squarebrackets_src.match(a);
            if(match.hasMatch()) {
                a.replace('[', '(').replace(']', ')');
            }
            Qu_Reader *r = new Qu_Reader(this, cu_pool, m_ctrl_factory_pool);
            if(m_conf.verbosity > Low)
                r->saveProperty();
            if(m_conf.property) { // property only
                r->propertyOnly();
                reader_ctx_options["properties-only"] = true;
                connect(r, SIGNAL(propertyReady(QString,double,CuData)), this, SLOT(onPropertyReady(QString,double,CuData)));
            }
            // scalar
            connect(r, SIGNAL(newDouble(QString,double,double, const CuData&)), this,
                    SLOT(onNewDouble(QString,double,double, const CuData&)));
            connect(r, SIGNAL(newFloat(QString,double,float, const CuData&)), this,
                    SLOT(onNewFloat(QString,double,float, const CuData&)));
            connect(r, SIGNAL(newBool(QString,double,bool, const CuData&)), this,
                    SLOT(onNewBool(QString,double,bool, const CuData&)));
            connect(r, SIGNAL(newLong(QString,double,long, const CuData&)), this,
                    SLOT(onNewLong(QString,double,long, const CuData&)));
            connect(r, SIGNAL(newShort(QString,double,short, const CuData&)), this,
                    SLOT(onNewShort(QString,double,short, const CuData&)));
            connect(r, SIGNAL(newUShort(QString,double,unsigned short, const CuData&)), this,
                    SLOT(onNewUShort(QString,double,unsigned short, const CuData&)));
            connect(r, SIGNAL(newString(QString,double,QString, const CuData&)), this,
                    SLOT(onNewString(QString,double,QString, const CuData&)));
            connect(r, SIGNAL(toString(QString,QString,double,QString, const CuData&)), this,
                    SLOT(onStringConversion(QString,QString,double,QString, const CuData&)));
            // arrays
            connect(r, SIGNAL(newDoubleVector(QString,double,QVector<double>, const CuData&)), this,
                    SLOT(onNewDoubleVector(QString,double,QVector<double>, const CuData&)));
            connect(r, SIGNAL(newFloatVector(QString,double,QVector<float>, const CuData&)), this,
                    SLOT(onNewFloatVector(QString,double,QVector<float>, const CuData&)));
            connect(r, SIGNAL(newBoolVector(QString,double,QVector<bool>, const CuData&)), this,
                    SLOT(onNewBoolVector(QString,double,QVector<bool>, const CuData&)));
            connect(r, SIGNAL(newShortVector(QString,double,QVector<short>, const CuData&)), this,
                    SLOT(onNewShortVector(QString,double,QVector<short>, const CuData&)));
            connect(r, SIGNAL(newUShortVector(QString,double,QVector<unsigned short>, const CuData&)), this,
                    SLOT(onNewUShortVector(QString,double,QVector<unsigned short>, const CuData&)));
            connect(r, SIGNAL(newLongVector(QString,double,QVector<long>, const CuData&)), this,
                    SLOT(onNewLongVector(QString,double,QVector<long>, const CuData&)));
            connect(r, SIGNAL(newULongVector(QString,double,QVector<unsigned long>, const CuData&)), this,
                    SLOT(onNewULongVector(QString,double,QVector<unsigned long>, const CuData&)));
            connect(r, SIGNAL(newStringList(QString,double,QStringList,CuData)), this,
                    SLOT(onNewStringList(QString,double,QStringList,CuData)));
            connect(r, SIGNAL(toStringList(QString,QString,double,QStringList,CuData)), this,
                    SLOT(onStringListConversion(QString,QString,double,QStringList,CuData)));
            connect(r, SIGNAL(newHdbData(QString,CuData)), this, SLOT(onNewHdbData(QString, CuData)));

            connect(r, SIGNAL(newError(QString,double,QString, const CuData&)),
                    this, SLOT(onError(QString,double,QString, const CuData&)));

            connect(r, SIGNAL(destroyed(QObject*)), this, SLOT(onReaderDestroyed(QObject *)));

            connect(r, SIGNAL(propertyReady(QString,double,CuData)), this, SLOT(onPropertyReady(QString,double,CuData)));

            reader_ctx_options["period"] = m_conf.period;
            r->setContextOptions(reader_ctx_options);
            r->setSource(a);
            m_readers.append(r);
            m_refreshCntMap[a] = 0;
        }
    }
}

void QumbiaReader::m_print_list_props(const CuData &pr)
{
    const CuVariant &plist = pr["keys"];
    const std::vector<std::string> vp = plist.toStringVector();

    foreach(QuString p, vp) {
        printf("   \e[1;32m%-25s\e[0m --> \e[1;3m%s\e[0m\n", qstoc(p),
               pr[p.toStdString()].toString().c_str());
    }
    printf("\e[1;36m-------------------------------------------------\e[0m\n");
}

void QumbiaReader::m_print_property(const CuData &pr)
{
    if(pr.containsKey("class")) printf(" - class: %s\n", vtoc2(pr, "class"));
    if(pr.containsKey("pattern")) printf(" - pattern: \"%s\"\n", vtoc2(pr, "pattern"));
    if(pr.containsKey("value")) printf(" - value: %s\n", vtoc2(pr, "value"));
    if(pr.containsKey("tango_host")) printf(" - tango host: %s\n", vtoc2(pr, "tango_host"));

    QStringList outputted_props;
    for(int i = Low; i <= static_cast<int>(Medium); i++) {
        printf("\e[1;36m-------------------------------------------------\e[0m\n");
        const QStringList& props = m_props_map[i];
        foreach(QString p, props) {
            const CuVariant &pv = pr[qstoc(p)];
            if(!pv.isNull())
                printf(" \e[1;32m%-15s\e[0m --> \e[1;3m%s\e[0m\n", qstoc(p), pr[qstoc(p)].toString().c_str());
            outputted_props << p;
        }
    }

    if(m_conf.verbosity > Medium) {
        printf("\e[1;36m-------------------------------------------------\e[0m\n");
        for(size_t i = 0; i < pr.keys().size(); i++) {
            QuString pn(pr.keys()[i]);
            if(!outputted_props.contains(pn)) {
                const CuVariant &pv = pr[pr.keys()[i]];
                if(!pv.isNull())
                    printf(" \e[1;32m%-25s\e[0m --> \e[1;3m%s\e[0m\n", qstoc(pn), pr[pr.keys()[i]].toString().c_str());

            }
        }
    }
    printf("\e[1;36m-------------------------------------------------\e[0m\n\n");
}

template<typename T>
QString QumbiaReader::m_format(const T &v, const char *fmt) const
{
    char str[64];
    !m_conf.format.isEmpty() ? snprintf(str, 64, qstoc(m_conf.format), v) : snprintf(str, 64, fmt, v);
    return QString(str);
}
