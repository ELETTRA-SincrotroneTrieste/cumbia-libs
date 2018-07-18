#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QQuickItem>

#include <cucontrolsfactorypool.h>
#include <cumbiapool_o.h>
#include <cutreader.h>
#include <cutwriter.h>
#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutango-world.h>
#ifdef QUMBIA_EPICS_CONTROLS
#include <cumbiaepics.h>
#include <cuepics-world.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#endif

#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

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

    CumbiaPool_O cumbia_fac_qobj;
    cumbia_fac_qobj.init(cu_pool, m_ctrl_factory_pool);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("cumbia_poo_o", &cumbia_fac_qobj);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    qDebug() << "root objects size  " << engine.rootObjects().size();
    QObject *root = engine.rootObjects().at(0);
    qDebug() << "root object" << root;
    const QMetaObject *super = root->metaObject()->superClass();
    while(super) {
        qDebug() << "root super" << super->className();
        super = super->superClass();
    }

    QQuickWindow *window = qobject_cast<QQuickWindow *>(root);
    QQuickItem* root_i = window->contentItem();
    qDebug() << "content itesm" << window->contentItem();

    foreach(QQuickItem  *it, root_i->childItems()) {
          qDebug() << "Child " << it << "NAME" << it->objectName();
          const QMetaObject *super = it->metaObject()->superClass();
          qDebug() << "souce " << it->property("source").toString();
          while(super) {
              qDebug() << "      item  super" << super->className() ;
              super = super->superClass();
          }
    }
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
