#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>

#include <cucontrolsfactorypool.h>
#include <cumbiapool_o.h>
#include <cumbiapool.h>
#include <cumbiawebsocket.h>
#include <cuwsreader.h> // for CuWSReaderFactory
#include <qmlappstatemanager.h>

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

int main(int argc, char *argv[])
{
    CumbiaPool *cu_pool = new CumbiaPool();
    CuControlsFactoryPool m_ctrl_factory_pool;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv); // before creating CumbiaWebSocket!

    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaWebSocket* cuws = new CumbiaWebSocket("ws://pwma-dev.elettra.eu:10080/updates",
                                                "https://pwma-dev.elettra.eu:10443/v1/cs/",
                                                new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    std::vector<std::string> tango_proto_patterns;
    tango_proto_patterns.push_back("tango://");
    cu_pool->registerCumbiaImpl("tango", cuws);
    m_ctrl_factory_pool.registerImpl("tango", CuWSReaderFactory());

    m_ctrl_factory_pool.setSrcPatterns("tango",tango_proto_patterns);
    cu_pool->setSrcPatterns("tango", tango_proto_patterns);

    CumbiaPool_O cumbia_fac_qobj;

    cumbia_fac_qobj.init(cu_pool, m_ctrl_factory_pool);

    QQuickView *view = new QQuickView;
    view->resize(800, 400);
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->engine()->rootContext()->setContextProperty("cumbia_poo_o", &cumbia_fac_qobj);
    view->setSource(QUrl("qrc:/main.qml"));
    view->show();

    // manage application state changes
    CuAppStateManager appstman(&app, view);
    // close websocket when the application is inactive, reopen it when it's active again
    appstman.addSuspendable(cuws->websocketClient());

    int ret = app.exec();
    delete view;
    delete cuws;
    delete cu_pool;
    return ret;
}
