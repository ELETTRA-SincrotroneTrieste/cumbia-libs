#include "formulas.h"
#include "ui/ui_formulas.h"

#include <cumbiapool.h>
#include <cumbiaepics.h>
#include <cumbiatango.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuepics-world.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>
#include <QPluginLoader>
#include <cupluginloader.h>
#include <cuformulaplugininterface.h>

Formulas::Formulas(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Testtest)
{
    cu_pool = cumbia_pool;
    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());

    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());

    // log
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

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
            CuControlsReaderFactoryI *formula_rf = fplu->getFormulaReaderFactory(cu_pool, m_ctrl_factory_pool);
            cu_pool->registerCumbiaImpl("formula", fplu->getCumbia());
            m_ctrl_factory_pool.registerImpl("formula", *formula_rf);
            m_ctrl_factory_pool.setSrcPatterns("formula", fplu->srcPatterns());
            cu_pool->setSrcPatterns("formula", fplu->srcPatterns());
        }
    }
    else {
        perr("failed to load plugin loader under path %s: %s", qstoc(plupath), qstoc(pluginLoader.errorString()));
    }
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);
}

Formulas::~Formulas()
{
    delete ui;
}
