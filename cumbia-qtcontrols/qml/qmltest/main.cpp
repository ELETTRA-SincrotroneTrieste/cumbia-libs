#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <cucontrolsfactorypool.h>
#include <cumbiapool_o.h>
#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutango-world.h>
#include <cumbiaepics.h>
#include <cuepics-world.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>  // for CuTWriterFactory
#include <cutcontrolsreader.h>  // for CuTReaderFactory
#include <cutcontrolswriter.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

#include <QtDebug>

int main(int argc, char *argv[])
{
    CumbiaPool *cu_pool = new CumbiaPool();
    CuControlsFactoryPool m_ctrl_factory_pool;

    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());

#ifdef QUMBIA_EPICS_CONTROLS
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());
#endif

    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());

    CumbiaPool_O cupo_qobj;
    cupo_qobj.init(cu_pool, m_ctrl_factory_pool);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    printf("setting context property %p cumbia pool %p\n", &cupo_qobj, cupo_qobj.getPool());
    engine.rootContext()->setContextProperty("cumbia_poo_o", &cupo_qobj);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));


    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
