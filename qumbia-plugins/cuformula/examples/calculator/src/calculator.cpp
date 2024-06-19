#include "calculator.h"

#include <cumbiapool.h>
#include <cumbiatango.h>
#ifdef QUMBIA_EPICS_CONTROLS
#include <cuepcontrolsreader.h>
#include <cumbiaepics.h>
#include <cuepics-world.h>
#include <cuepcontrolswriter.h>
#endif
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>
#include <QPluginLoader>
#include <cupluginloader.h>
#include <cucontext.h>
#include <cuformulaplugininterface.h>
#include <QDateTime>

#include <quwatcher.h>

Calculator::Calculator(CumbiaPool *cumbia_pool, QObject *parent, bool verbose) :
    QObject(parent)
{
    cu_pool = cumbia_pool;
    // setup Cumbia pool and register cumbia implementations for tango and epics
#ifdef QUMBIA_EPICS_CONTROLS
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());

    cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

#endif
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());

    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
    // log
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

    // formula

    CuPluginLoader plulo;
    QString plupath = plulo.getPluginAbsoluteFilePath(CUMBIA_QTCONTROLS_PLUGIN_DIR, "cuformula-plugin.so");
    QPluginLoader pluginLoader(plupath);
    QObject *plugin = pluginLoader.instance();
    if (plugin){
        CuFormulaPluginI *fplu = qobject_cast<CuFormulaPluginI *>(plugin);
        if(!fplu)
            perr("Failed to load formula plugin");
        else {
            printf("\e[1;32msetting up pools for formula...\e[0m\n");
            fplu->initialize(cu_pool, m_ctrl_factory_pool);
        }
    }
    else {
        perr("failed to load plugin loader under path %s: %s", qstoc(plupath), qstoc(pluginLoader.errorString()));
    }

    m_verbose = verbose;
    m_finished = false;
    m_code = 0; // success
}

Calculator::~Calculator()
{
    delete m_ctx;
}

void Calculator::read(const QString &expr)
{
    m_ctx = new CuContext(cu_pool, m_ctrl_factory_pool);
    CuData options;
    options["properties-only"] = true;
    m_ctx->setOptions(options);
    CuControlsReaderA* reader = m_ctx->replace_reader(expr.toStdString(), this);
    reader->setSource(expr);
}

bool Calculator::hasFinished() const
{
    return m_finished;
}

int Calculator::code() const
{
    return m_code;
}

void Calculator::onUpdate(const CuData &data)
{
    bool err = data[TTT::Err].toBool();  // data["err"]
    err ? printf("\e[1;31m") : printf("\e[0;m");
    if(err)
        printf("%s", data[TTT::Message].toString().c_str());  // data["msg"]

    if(m_verbose)
        printf("[\e[1;34mfull data\e[0m]: %s\n", data.toString().c_str());

    if(!err)
        printf("\n[\e[0;36m%s\e[0m  -->  \e[1;32m%s\e[0m ]\n\n",
               qstoc(QDateTime::fromMSecsSinceEpoch(data[TTT::Time_ms].toLongInt()).toString("yyyy.MM.dd hh.mm.ss")),  // data["timestamp_ms"]
               data[TTT::Value].toString().c_str());  // data["value"]

    emit finished();
    m_finished = true;
    err ? m_code = 1 : m_code = 0;
}
