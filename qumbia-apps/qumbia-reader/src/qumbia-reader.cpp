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

#include <cupollingservice.h>

// plugin
#include <cupluginloader.h>
#include <cuformulaplugininterface.h>
#include <cutthreadtokengen.h>

#ifdef HAS_CUHDB
#include <cuhistoricaldbplugin_i.h>
#else
class CuHdbPlugin_I;
#endif

#include <quapps.h>

QumbiaReader::QumbiaReader(CumbiaPool *cumbia_pool, QWidget *parent) :
    QObject(parent)
{
    QStringList engines;
    cu_pool = cumbia_pool;
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);

    // plugins, instantiate if possible. Initialize only after engines.
    CuPluginLoader pload;
    QObject *plugin_qob;
    CuFormulaPluginI *fplu = pload.get<CuFormulaPluginI>("cuformula-plugin.so", &plugin_qob);
    CuHdbPlugin_I *hdb_p = nullptr;

#ifdef HAS_CUHDB
    // historical database
    QObject *hdb_o;
    hdb_p = pload.get<CuHdbPlugin_I>("cuhdb-qt-plugin.so", &hdb_o);
    hdb_p = pload.get<CuHdbPlugin_I>("cuhdb-qt-plugin.so", &hdb_o);
#endif
    // parse configuration
    CmdLineOptions cmdo(fplu != nullptr, hdb_p != nullptr);
    m_conf = cmdo.parse(qApp->arguments());
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
//    CuTThreadTokenGen *tango_tk_gen = new CuTThreadTokenGen(1, "thread_");

//        Cumbia *cu_t = cumbia_pool->get("tango");
//        if(cu_t) {
//            printf("cumbia-reader: installing tango thread token generator (hw concurrency) %d [for testing]\n", std::thread::hardware_concurrency());
//            cu_t->setThreadTokenGenerator(tango_tk_gen);
//            // always, timestamp, none
//            if(m_conf.unchanged_upd_mode == "timestamp")
//                static_cast<CumbiaTango *>(cu_t)->setReadUpdatePolicy(CuDataUpdatePolicy::OnPollUnchangedTimestampOnly);
//            else if(m_conf.unchanged_upd_mode == "none")
//                static_cast<CumbiaTango *>(cu_t)->setReadUpdatePolicy(CuDataUpdatePolicy::OnPollUnchangedNoUpdate);
//        }

    m_props_map[Low] = QStringList() << "min" << "min_alarm" << "min_warning" << "max_warning" <<
                                        "max_alarm" << "max" << "dfs" << "display_unit"
                                     << "label" << "description";
    m_props_map[Medium] = QStringList() << "activity" << "worker_activity" << "worker_thread";

    m_err = m_conf.property && m_conf.refresh_limit != 1;
    if(m_err)  {
        perr("-q (property) and -m (monitor) or -r X, X > 1 are not compatible\n");
    }
    else {
        if(m_conf.list_options)
            cmdo.list_options();
#ifdef HAS_CUHDB
        if(hdb_p && !m_conf.db_profile.isEmpty()) {
            hdb_p->setDbProfile(m_conf.db_profile);
        }
        else if(hdb_p && !m_conf.list_options){
            printf("\e[1;33m* \e[0;4mcumbia read\e[0m: using \e[1;33mdefault\e[0m historical database profile, if available\n");
        }
#endif

        if(!m_conf.usage && !m_conf.list_options && m_conf.sources.size() > 0)
            m_createReaders(m_conf.sources);
        else if(m_conf.sources.size() == 0 && !m_conf.list_options && !m_conf.help)
            cmdo.help(qApp->arguments().first(), "");
        else if(m_conf.help) {
            printf("\e[1;32mcumbia read specific options\e[0m\n%s\n\n", qstoc(cmdo.help()));
            printf("Type \e[1;32mcumbia read\e[0m with no parameters to read the manual\n\n");
        }

        if(m_conf.refresh_limit < 1 && m_conf.sources.size() > 0 && !m_conf.help && !m_conf.list_options) {
            // wait for keyboard input to stop
            printf("\e[1;32m * \e[0;4mmonitor\e[0m started: press \e[1;32many key\e[0m to \e[1;32mexit\e[0m\n");
            KbdInputWaitThread *kbdt = new KbdInputWaitThread(this);
            connect(kbdt, SIGNAL(finished()), qApp, SLOT(quit()));
            kbdt->start();
        }
    } // !m_err
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
    printf("[%s] [\e[1;34mstring\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), qstoc(val));
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
//    printf("\e[1;33m%s --> %s\e[0m\n", qstoc(src), datos(hdbd));
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

void QumbiaReader::onNewDoubleMatrix(const QString &src, double timestamp_us, const CuMatrix<double> &m, const CuData &da){
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mdouble\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewFloatMatrix(const QString &src, double timestamp_us, const CuMatrix<float> &m, const CuData &da) {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mdouble\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewBoolMatrix(const QString& src, double timestamp_us, const CuMatrix<bool>& m, const CuData& da) {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;35mbool\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewUCharMatrix(const QString& src, double timestamp_us, const CuMatrix<unsigned char>& m, const CuData& da)  {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;32muchar\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewCharMatrix(const QString& src, double timestamp_us, const CuMatrix<char>& m, const CuData& da)  {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mchar\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewShortMatrix(const QString& src, double timestamp_us, const CuMatrix<short>& m, const CuData& da)  {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;35mshort\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewUShortMatrix(const QString& src, double timestamp_us, const CuMatrix<unsigned short>& m, const CuData& da)  {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;35mushort\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewStringMatrix(const QString& src, double timestamp_us, const CuMatrix<std::string>& m, const CuData& da)  {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;33mstring\e[0m,matrix] \n{ %s ",
           qstoc(makeTimestamp(timestamp_us)), m.repr(m_conf.truncate).c_str());
    printf("}\n");
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewIntMatrix(const QString& src, double timestamp_us, const CuMatrix<int>& m, const CuData& da)  {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;36mint\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewUIntMatrix(const QString& src, double timestamp_us, const CuMatrix<unsigned>& m, const CuData& da) {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;36muint\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewULongMatrix(const QString& src, double timestamp_us, const CuMatrix<unsigned long>& m, const CuData& da)  {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[0;34mulong\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void QumbiaReader::onNewLongMatrix(const QString& src, double timestamp_us, const CuMatrix<long>& m, const CuData& da)  {
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mlong\e[0m,matrix(%ldx%ld)] { ", qstoc(makeTimestamp(timestamp_us)), m.nrows(), m.ncols());
    printf("---\n%s\n---\n", m.repr(m_conf.truncate).c_str());
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
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
    if(m_conf.verbosity > Low && pr.containsKey(CuDType::Thread))  // pr.containsKey("thread")
        printf("[\e[1;36m%s\e[0m] ", pr[CuDType::Thread].toString().c_str());  // pr["thread"]
    // src and timestamp
    printf("%s [%s] ", qstoc(src), qstoc(makeTimestamp(ts)));

    printf("\n - \e[1;32m%s\e[0m\n", vtoc2(pr, "device"));


    if(pr.containsKey(CuDType::Keys))  // pr.containsKey("keys")
        m_print_list_props(pr);
    else
        m_print_property(pr);

    m_checkRefreshCnt(sender());

    if(m_conf.verbosity == Debug)
        printf("\n\e[0;35m-l debug\e[0m: \e[1;35m{\e[0m %s \e[1;35m}\e[0m\n", pr.toString().c_str());
}

void QumbiaReader::onNewUnchanged(const QString &src, double ts) {
    printf("\e[0;35m * \e[0m [%s]: \"%s\" \e[0;35munchanged\e[0m\n", qstoc(makeTimestamp(ts)), qstoc(src));
}

void QumbiaReader::onError(const QString &src, double ts, const QString &msg, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    perr("[%s]: \"\e[1;31m%s\e[0m\"\n", qstoc(makeTimestamp(ts)), qstoc(msg));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
    printf("DATA: %s\n", datos(da));
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
        printf("\n\e[0;35m-l debug\e[0m: \e[1;35m{\e[0m %s \e[1;35m}\n", da.toString().c_str());
    }

    // label
    if(da.containsKey(CuDType::Label)) {  // da.containsKey("label")
        printf("\e[1;32;4m%s\e[0m: ", da[CuDType::Label].toString().c_str());  // da["label"]
    }
    else {
        printf("\e[1;32;4m%s\e[0m: ", da[CuDType::Src].toString().c_str());  // da["src"]
    }

    if(m_conf.verbosity >= High) {
        if(da.containsKey(CuDType::Thread))  // da.containsKey("thread")
            printf("[\e[1;36m%s\e[0m] ", da[CuDType::Thread].toString().c_str());  // da["thread"]
    }
    if(m_conf.verbosity >= Medium) {
        if(da.containsKey(CuDType::Min)) {  // da.containsKey("min")
            double min;
            da[CuDType::Min].to<double>(min);  // da["min"]
            printf("\e[1;31m%s\e[1;35m < \e[0m", qstoc(m_format<double>(min, "%.2f")));
        }
    }
}

void QumbiaReader::m_print_extra2(const CuData &da)
{
    if(da.containsKey(CuDType::Label) && m_conf.verbosity >= Medium) {  // da.containsKey("label")
        // print full source name
        printf(" [\e[1;36m%s\e[0m]", da[CuDType::Src].toString().c_str());  // da["src"]
    }
    if(m_conf.verbosity >= Medium) {
        if(da.containsKey(CuDType::Min)) {  // da.containsKey("min")
            double max;
            da[CuDType::Max].to<double>(max);  // da["max"]
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

void QumbiaReader::m_createReaders(const QStringList &srcs)  {
    for(int i = 0; i < srcs.size(); i++)
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
        int idx = a.indexOf("://");
        QString save_dom;
        if(idx > -1) {
            save_dom = a.section("://", 0, 0) + "://";
            a.remove(save_dom);
        }
        a.replace("//", "->");
        a = QString("%1%2").arg(save_dom).arg(a);
        QRegularExpressionMatch match = squarebrackets_src.match(a);
        if(match.hasMatch()) {
            a.replace("[[", "(").replace("]]", ")");
        }
        Qu_Reader *r = new Qu_Reader(this, cu_pool, m_ctrl_factory_pool);
        if(m_conf.verbosity > Low && !m_conf.no_properties)
            r->saveProperty();
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

        // matrix
        connect(r, SIGNAL(newDoubleMatrix(const QString&, double, const CuMatrix<double>&, const CuData& ) ), this,
                SLOT(onNewDoubleMatrix(const QString&,  double, const CuMatrix<double>&, const CuData& )));

        connect(r, SIGNAL(newIntMatrix(const QString&, double, const CuMatrix<int>&, const CuData& ) ), this,
                SLOT(onNewIntMatrix(const QString&,  double, const CuMatrix<int>&, const CuData& )));

        connect(r, SIGNAL(newCharMatrix(const QString&, double, const CuMatrix< char>&, const CuData& ) ), this,
                SLOT(onNewCharMatrix(const QString&,  double, const CuMatrix< char>&, const CuData& )));
        connect(r, SIGNAL(newUCharMatrix(const QString&, double, const CuMatrix<unsigned char>&, const CuData& ) ), this,
                SLOT(onNewUCharMatrix(const QString&,  double, const CuMatrix<unsigned char>&, const CuData& )));
        connect(r, SIGNAL(newFloatMatrix(const QString&, double, const CuMatrix<float>&, const CuData& ) ), this,
                SLOT(onNewFloatMatrix(const QString&,  double, const CuMatrix<float>&, const CuData& )));
        connect(r, SIGNAL(newUShortMatrix(const QString&, double, const CuMatrix<unsigned short>&, const CuData& ) ), this,
                SLOT(onNewUShortMatrix(const QString&,  double, const CuMatrix<unsigned short>&, const CuData& )));
        connect(r, SIGNAL(newShortMatrix(const QString&, double, const CuMatrix<short>&, const CuData& ) ),
                this, SLOT(onNewShortMatrix(const QString&,  double, const CuMatrix<short>&, const CuData& )));
        connect(r, SIGNAL(newLongMatrix(const QString&, double, const CuMatrix<long int>&, const CuData& ) ), this,
                SLOT(onNewLongMatrix(const QString&,  double, const CuMatrix<long int>&, const CuData& )));
        connect(r, SIGNAL(newStringMatrix(const QString&, double, const CuMatrix<std::string>&, const CuData& ) ), this,
                SLOT(onNewStringMatrix(const QString&,  double, const CuMatrix<std::string>&, const CuData& )));

        connect(r, SIGNAL(newUnchanged(QString, double)), this, SLOT(onNewUnchanged(QString, double)));
        connect(r, SIGNAL(newError(QString,double,QString, const CuData&)),
                this, SLOT(onError(QString,double,QString, const CuData&)));

        connect(r, SIGNAL(destroyed(QObject*)), this, SLOT(onReaderDestroyed(QObject *)));

        connect(r, SIGNAL(propertyReady(QString,double,CuData)), this, SLOT(onPropertyReady(QString,double,CuData)));

        if(m_conf.refresh_limit == 1)
            reader_ctx_options["single-shot"] = true;
        reader_ctx_options[CuDType::Period] = m_conf.period;  // reader_ctx_options["period"]
        if(m_conf.property)
            reader_ctx_options[CuDType::Property] = true;  // reader_ctx_options["property"]
        if(m_conf.no_properties)
            reader_ctx_options["no-properties"] = true;
        r->setContextOptions(reader_ctx_options);
        r->setSource(a);
        m_readers.append(r);
        m_refreshCntMap[a] = 0;
    }

}

void QumbiaReader::m_print_list_props(const CuData &pr)
{
    const CuVariant &plist = pr[CuDType::Keys];  // pr["keys"]
    const std::vector<std::string> vp = plist.toStringVector();

    foreach(QuString p, vp) {
        printf("   \e[1;32m%-25s\e[0m --> \e[1;3m%s\e[0m\n", qstoc(p),
               pr[p.toStdString()].toString().c_str());
    }
    printf("\e[1;36m-------------------------------------------------\e[0m\n");
}

void QumbiaReader::m_print_property(const CuData &pr)
{
    if(pr.containsKey(CuDType::Class)) printf(" - class: %s\n", vtoc2(pr, CuDType::Class));  // pr.containsKey("class")
    if(pr.containsKey(CuDType::Pattern)) printf(" - pattern: \"%s\"\n", vtoc2(pr, CuDType::Pattern));  // pr.containsKey("pattern")
    if(pr.containsKey(CuDType::Value)) printf(" - value: %s\n", vtoc2(pr, CuDType::Value));  // pr.containsKey("value")
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

bool QumbiaReader::config_error() const {
    return m_err;
}
