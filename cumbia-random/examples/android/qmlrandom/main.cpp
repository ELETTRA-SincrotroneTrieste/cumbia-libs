#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QQuickItem>

#include <cucontrolsfactorypool.h>
#include <cumbiapool_o.h>
#include <cumbiapool.h>
#include <cumbiarandom.h>
#include <curndreader.h>
#include <cumbiarndworld.h>

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{
    CumbiaPool *cu_pool = new CumbiaPool();
    CuControlsFactoryPool m_ctrl_factory_pool;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv); // before creating Cumbiarandom!

    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaRandom* curnd = new CumbiaRandom(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    std::vector<std::string> tango_proto_patterns;
    tango_proto_patterns.push_back("random");
    cu_pool->registerCumbiaImpl("random", curnd);
    m_ctrl_factory_pool.registerImpl("random", CurndReaderFactory());

    m_ctrl_factory_pool.setSrcPatterns("random",tango_proto_patterns);
    cu_pool->setSrcPatterns("random", tango_proto_patterns);

    CumbiaPool_O cumbia_fac_qobj;

    cumbia_fac_qobj.init(cu_pool, m_ctrl_factory_pool);

    QQuickView *view = new QQuickView;
    view->resize(800, 400);
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->engine()->rootContext()->setContextProperty("cumbia_poo_o", &cumbia_fac_qobj);
    view->setSource(QUrl("qrc:/main.qml"));
    view->show();

    foreach(QObject * o, view->findChildren<QObject *>())
        qDebug() << __FUNCTION__ << "child" << o << o->objectName();

//    if (rootObjects().isEmpty())
//        return -1;

    int ret = app.exec();
    qDebug() << __FUNCTION__ << "app exited code" << ret;
    delete view;
    delete curnd;
    delete cu_pool;
    return ret;
}
