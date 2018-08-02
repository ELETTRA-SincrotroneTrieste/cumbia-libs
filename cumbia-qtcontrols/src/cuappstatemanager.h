#ifndef QMLAPPSTATEMANAGER_H
#define QMLAPPSTATEMANAGER_H

#include <QObject>
#include <QList>

class QGuiApplication;

/*! \brief This class calls suspend and start on all children of a given root QObject
 *         according to the QGuiApplication state changes.
 *
 * This utility can be used when readers must be suspended when the application becomes
 * inactive. This approach is fundamental when developing applications on mobile
 * devices.
 *
 * \par Example: using CuAppStateManager in a qml based application for android
 *
 * This is how main.cpp should look like in order to build an application which properly
 * reacts to state changes
 *
 * \code
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

 * \endcode
 *
 * When an application changes its state, all children of the specified root object
 * (QQuickView in the example) are asked to suspend or start, according to the new
 * state. It is necessary that the children are QObject and implement the <em>start and
 * suspend</em> methods.
 *
 * Additional objects that are not children of the given root can be added through the
 * addSuspendable method. They must be QObject and implement the <em>start and suspend</em>
 * methods.
 *
 * \par Note
 * If CuAppStateManager is used as shown in the example (passing the root of the view), it is
 * not necessary to write additional code in the <em>qml</em> file to suspend and restart the
 * (readers) backends every time the application state changes.
 *
 */
class CuAppStateManager : public QObject
{
    Q_OBJECT
public:
    explicit CuAppStateManager(QGuiApplication *app, QObject* root);

    void addSuspendable(QObject *suspendable);

    void removeSuspendable(QObject *suspendable);

signals:

public slots:

private slots:
    void onApplicationStateChanged(Qt::ApplicationState st);

private:
    QList<QObject*> m_suspendables;
    QObject* m_root;
};

#endif // QMLAPPSTATEMANAGER_H
