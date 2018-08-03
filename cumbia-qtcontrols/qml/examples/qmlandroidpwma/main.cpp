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

    // since we use Qt Charts, we must select QApplication, not QGuiApplication
    QApplication app(argc, argv); // before creating CumbiaWebSocket!

    // setup Cumbia web socket with the web socket address and the host name to prepend to the sources
    // for the HTTP requests
    CumbiaWebSocket* cuws = new CumbiaWebSocket("ws://pwma-dev.elettra.eu:10080/updates",
                                                "https://pwma-dev.elettra.eu:10443/v1/cs/",
                                                new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());

    // source names will start with "tango://", use it as pattern to register the CumbiaWebSocket
    // implementation for such sources
    std::vector<std::string> tango_proto_patterns;
    tango_proto_patterns.push_back("tango://");
    cu_pool->registerCumbiaImpl("tango", cuws);
    m_ctrl_factory_pool.registerImpl("tango", CuWSReaderFactory());
    m_ctrl_factory_pool.setSrcPatterns("tango",tango_proto_patterns);
    cu_pool->setSrcPatterns("tango", tango_proto_patterns);

    // wrap up CumbiaPool and CuControlsFactoryPool into a QObject so that it can be
    // set as a QML engine context property below
    CumbiaPool_O cumbia_fac_qobj;
    cumbia_fac_qobj.init(cu_pool, m_ctrl_factory_pool);

    // the view
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

    // tidy up
    delete view;
    delete cuws;
    delete cu_pool;
    return ret;
}
