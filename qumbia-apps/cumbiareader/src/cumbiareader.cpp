#include "cumbiareader.h"
#include "cmdlineoptions.h"

#include <cumbiapool.h>
#include <qustring.h>
#include <qustringlist.h>
#include <cucontextactionbridge.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>
#include <cutimerservice.h>
#include <reader.h>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDateTime>

// plugin
#include <cupluginloader.h>
#include <cuformulaplugininterface.h>

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
#endif
#ifdef CUMBIA_RANDOM_VERSION
#include <cumbiarandom.h>
#include <curndreader.h>
#include <curndactionfactories.h>
#include <cumbiarndworld.h>
#endif

Cumbiareader::Cumbiareader(CumbiaPool *cumbia_pool, QWidget *parent) :
    QObject(parent)
{
    QStringList engines;
    cu_pool = cumbia_pool;

    // setup Cumbia pool and register cumbia implementations for tango and epics
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
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
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
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
    CuPluginLoader pload;
    QObject *plugin_qob;
    CuFormulaPluginI *fplu = pload.get<CuFormulaPluginI>("cuformula-plugin.so", &plugin_qob);
    if(fplu) {
        fplu->initialize(cu_pool, m_ctrl_factory_pool);
        engines << "formula plugin";
    }
    m_props_map[Low] = QStringList() << "min" << "min_alarm" << "min_warning" << "max_warning" <<
                                        "max_alarm" << "max" << "data_format_str" << "display_unit"
                                     << "label" << "description";
    m_props_map[Medium] = QStringList() << "activity" << "worker_activity" << "worker_thread";

    CmdLineOptions cmdo;
    m_conf = cmdo.parse(qApp->arguments());

    if(m_conf.max_timers > 0) {
        CuTimerService *ts = static_cast<CuTimerService *>(cuta->getServiceProvider()->get(CuServices::Timer));
        ts->setTimerMaxCount(m_conf.max_timers);
    }
    if(m_conf.usage) {
        cmdo.usage(qApp->arguments().first());
        printf("\nAvailable engines: %s\n\n", qstoc(engines.join(", ")));
    }
    else if(m_conf.list_options)
        cmdo.list_options();

    if(!m_conf.usage && !m_conf.list_options)
        m_createReaders(m_conf.sources);
}

Cumbiareader::~Cumbiareader() {

}

QString Cumbiareader::makeTimestamp(const double d) const {
    QString ts = QDateTime::fromSecsSinceEpoch(static_cast<int>(d)).toString("HH:mm:ss");
    ts += QString("+%1").arg(d - static_cast<int>(d));
    return ts;
}

bool Cumbiareader::usage_only() const {
    return m_conf.usage || m_conf.list_options;
}

void Cumbiareader::onNewDouble(const QString& src, double ts, double val, const CuData &da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s]  [\e[1;36mdouble\e[0m] \e[1;32m%s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<double>(val, "%.2f")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void Cumbiareader::onNewDoubleVector(const QString &src, double ts, const QVector<double> &v, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;36mdouble\e[0m,%d] { ",  qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<double>(v[i], "%.2f")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    printf("%s \e[0m}", qstoc(m_format<double>(v[v.size() - 1], "%.2f")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void Cumbiareader::onNewShort(const QString &src, double ts, short val, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mshort\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<short>(val, "%d")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void Cumbiareader::onNewLong(const QString &src, double ts, long val, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mlong\e[0m] %s\e[0m", qstoc(makeTimestamp(ts)), qstoc(m_format<long>(val, "%ld")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void Cumbiareader::onNewLongVector(const QString &src, double ts, const QVector<long> &v, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;35mlong\e[0m,%d] { ", qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<long>(v[i], "%ld")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    printf("%s \e[0m}", qstoc(m_format<long>(v[v.size() - 1], "%ld")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void Cumbiareader::onPropertyReady(const QString &src, double ts,const CuData &pr)
{
    m_refreshCntMap[src]++;
    printf("\n \e[1;36m*\e[0m ");
    if(m_conf.verbosity > Low && pr.containsKey("thread"))
        printf("[\e[1;36m%s\e[0m] ", pr["thread"].toString().c_str());
    // src and timestamp
    printf("%s [%s] ", qstoc(src), qstoc(makeTimestamp(ts)));

    if(pr.containsKey("list")) {
        m_print_list_props(pr);
        qApp->quit();
    }
    else {
        m_print_property(pr);
        m_checkRefreshCnt(sender());
    }

    if(m_conf.verbosity == Debug)
        printf("\n\e[0;35m--l=debug\e[0m: \e[1;35m{\e[0m %s \e[1;35m}\n", pr.toString().c_str());
}

void Cumbiareader::onNewShortVector(const QString &src, double ts, const QVector<short> &v, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    printf("[%s] [\e[1;34mshort\e[0m,%d] { ", qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_conf.truncate < 0 || i < m_conf.truncate - 1); i++)
        printf("%s,", qstoc(m_format<short>(v[i], "%d")));
    if(m_conf.truncate > -1 && m_conf.truncate < v.size())
        printf(" ..., ");
    printf("%s \e[0m}", qstoc(m_format<short>(v[v.size() - 1], "%d")));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void Cumbiareader::onError(const QString &src, double ts, const QString &msg, const CuData& da)
{
    m_refreshCntMap[src]++;
    m_print_extra1(da);
    perr("[%s]: \"\e[1;31m%s\e[0m\"\n", qstoc(makeTimestamp(ts)), qstoc(msg));
    m_print_extra2(da);
    m_checkRefreshCnt(sender());
}

void Cumbiareader::onReaderDestroyed(QObject *o)
{
    m_readers.removeAll(o);
    if(m_readers.isEmpty())
        qApp->quit();
}

void Cumbiareader::m_print_extra1(const CuData &da)
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

void Cumbiareader::m_print_extra2(const CuData &da)
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

void Cumbiareader::m_checkRefreshCnt(QObject *o)
{
    Reader * r = qobject_cast<Reader *>(o);
    if(r && m_refreshCntMap[r->source()] == m_conf.refresh_limit) {
        r->stop();
        o->deleteLater();
    }
}

void Cumbiareader::m_createReaders(const QStringList &srcs) {
    if(m_conf.tango_property) {
        Reader *r = new Reader(this, cu_pool, m_ctrl_factory_pool);
        r->setTgPropertyList(srcs);
        connect(r, SIGNAL(newError(QString,double,QString, const CuData&)),
                this, SLOT(onError(QString,double,QString, const CuData&)));
        connect(r, SIGNAL(propertyReady(QString,double,CuData)), this, SLOT(onPropertyReady(QString,double,CuData)));
        r->getTgProps();
    }
    else {
        for(int i = 1; i < srcs.size(); i++)
        {
            // square brackets in bash shell are OK
            // if present, replace them with ()
            // [A-Za-z0-9\-_\.,/]+->[A-Za-z0-9\-_\.,]+(?:\[.*\]){0,1}
            // match for example aa/bb/cc:command[1,2] and replace with
            // aa/bb/cc->command(1,2)
            //
            QRegularExpression squarebrackets_cmd("[A-Za-z0-9\\-_\\.,/]+->[A-Za-z0-9\\-_\\.,]+(?:\\[.*\\]){0,1}");
            CuData reader_ctx_options;
            QString a = m_conf.sources.at(i);
            if(a.count('/') == 4 && a.count("//") == 1)
                a.replace("//", "->");
            QRegularExpressionMatch match = squarebrackets_cmd.match(a);
            if(match.hasMatch()) {
                a.replace('[', '(').replace(']', ')');
            }
            Reader *r = new Reader(this, cu_pool, m_ctrl_factory_pool);
            if(m_conf.verbosity > Low)
                r->saveProperty();
            if(m_conf.property) { // property only
                r->propertyOnly();
                reader_ctx_options["properties-only"] = true;
            }

            connect(r, SIGNAL(newDouble(QString,double,double, const CuData&)), this,
                    SLOT(onNewDouble(QString,double,double, const CuData&)));
            connect(r, SIGNAL(newDoubleVector(QString,double,QVector<double>, const CuData&)), this,
                    SLOT(onNewDoubleVector(QString,double,QVector<double>, const CuData&)));
            connect(r, SIGNAL(newShort(QString,double,short, const CuData&)), this,
                    SLOT(onNewShort(QString,double,short, const CuData&)));
            connect(r, SIGNAL(newShortVector(QString,double,QVector<short>, const CuData&)), this,
                    SLOT(onNewShortVector(QString,double,QVector<short>, const CuData&)));
            connect(r, SIGNAL(newLong(QString,double,long, const CuData&)), this,
                    SLOT(onNewLong(QString,double,long, const CuData&)));
            connect(r, SIGNAL(newLongVector(QString,double,QVector<long>, const CuData&)), this,
                    SLOT(onNewLongVector(QString,double,QVector<long>, const CuData&)));

            connect(r, SIGNAL(destroyed(QObject*)), this, SLOT(onReaderDestroyed(QObject *)));
            reader_ctx_options["period"] = m_conf.period;
            r->setContextOptions(reader_ctx_options);
            r->setSource(a);
            m_readers.append(r);
            m_refreshCntMap[a] = 0;
        }
    }
}

void Cumbiareader::m_print_list_props(const CuData &pr)
{
    //    printf("\e[1;33m%s\e[0m\n", pr.toString().c_str());
    const CuVariant &plist = pr["list"];
    // get the list of device (or class or device/attribute) + property name
    // e.g.  ["list" -> test/device/1/double_scalar:abs_change,test/device/1/double_scalar:archive_abs_change,...]
    const std::vector<std::string> vp = plist.toStringVector();
    // group properties by device
    QMap<QString, QStringList> dev_pr_map;
    foreach(QuString p, vp) {
        // get the property values
        if(pr.containsKey(p.toStdString()) && p.count(":") == 1)  // group prints by dev
            dev_pr_map[p.section(':', 0, 0)].append(p.section(':', 1, 1));
    }
    foreach(QString dev, dev_pr_map.keys()) {
        printf("\n - \e[1;32m%s\e[0m\n", qstoc(dev));
        foreach(QString prop, dev_pr_map[dev]) {
            printf("   \e[1;32m%-25s\e[0m --> \e[1;3m%s\e[0m\n", qstoc(prop),
                   pr[QString("%1:%2").arg(dev).arg(prop).toStdString()].toString().c_str());
        }
        printf("\e[1;36m-------------------------------------------------\e[0m\n");
    }
}

void Cumbiareader::m_print_property(const CuData &pr)
{
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
QString Cumbiareader::m_format(const T &v, const char *fmt) const
{
    char str[64];
    !m_conf.format.isEmpty() ? snprintf(str, 64, qstoc(m_conf.format), v) : snprintf(str, 64, fmt, v);
    return QString(str);
}
