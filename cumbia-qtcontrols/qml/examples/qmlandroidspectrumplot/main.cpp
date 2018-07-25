#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QQuickItem>

#include <cucontrolsfactorypool.h>
#include <cumbiapool_o.h>
#include <cumbiapool.h>
#include <cumbiawebsocket.h>
#include <cuwsreader.h>
#include <cumbiawsworld.h>

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{
    CumbiaPool *cu_pool = new CumbiaPool();
    CuControlsFactoryPool m_ctrl_factory_pool;

    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaWebSocket* cuws = new CumbiaWebSocket(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("websocket", cuws);
    m_ctrl_factory_pool.registerImpl("websocket", CuWSReaderFactory());

    CumbiaWSWorld wsw;
    m_ctrl_factory_pool.setSrcPatterns("websocket", wsw.srcPatterns());
    cu_pool->setSrcPatterns("websocket", wsw.srcPatterns());

    CumbiaPool_O cumbia_fac_qobj;

    qDebug() << "CONCHA DE LA LORAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    qDebug() << "init on cumbia factory object!\n";
    cumbia_fac_qobj.init(cu_pool, m_ctrl_factory_pool);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

//    QQmlApplicationEngine engine;
//    engine.rootContext()->setContextProperty("cumbia_poo_o", &cumbia_fac_qobj);

//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

//    qDebug() << "root objects size  " << engine.rootObjects().size();
//    QObject *root = engine.rootObjects().at(0);
//    qDebug() << "root object" << root;
//    const QMetaObject *super = root->metaObject()->superClass();
//    while(super) {
//        qDebug() << "root super" << super->className();
//        super = super->superClass();
//    }

//    QQuickItem* root_i = window->contentItem();
//    qDebug() << "content itesm" << window->contentItem();

//    foreach(QQuickItem  *it, root_i->childItems()) {
//          qDebug() << "Child " << it << "NAME" << it->objectName();
//          const QMetaObject *super = it->metaObject()->superClass();
//          qDebug() << "souce " << it->property("source").toString();
//          while(super) {
//              qDebug() << "      item  super" << super->className() ;
//              super = super->superClass();
//          }
//    }


    QQuickView view;
    view.resize(800, 400);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.engine()->rootContext()->setContextProperty("cumbia_poo_o", &cumbia_fac_qobj);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

//    if (rootObjects().isEmpty())
//        return -1;

    return app.exec();
}
