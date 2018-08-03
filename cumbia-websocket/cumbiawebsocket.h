#ifndef CUMBIAWEBSOCKET_H
#define CUMBIAWEBSOCKET_H

#include <cumbia.h>
#include <string>
#include <cuwsactionreader.h>
#include <cuwsactionfactoryi.h>
#include "cumbia-websocket_global.h"
#include <cuwsclient.h>
#include <QString>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuDataListener;

class CumbiaWebSocketPrivate;
/*!
 * \mainpage Cumbia <em>websocket</em> module
 *
 * The *cumbia web socket* module provides an interface to send and receive data through *http* and receive
 * data from a *web socket*. The library relies on the *canoned* server that has been developed for the
 * *PWMA mobile and web interface*:
 *
 * - The <a href="https://gitlab.com/PWMA/">PWMA</a> project
 *
 * The *cumbia web socket* module has been developed in order to build applications for <strong>mobile devices</strong>
 * able to interact with the Tango and EPICS control systems through the aforementioned *canoned server*
 * (<a href="https://gitlab.com/PWMA/">PWMA</a>).
 *
 * \section write_app_android Write an QML application for android.
 *
 * This example shows how to write and build a simple Qt QML application for Android that reads from the Tango control
 * system through *http and websockets* using *cumbia-websocket*.
 *
 * \subsection new_project Start a new project
 * - Open Qt creator and create a new project.
 * - In the "New file or project" dialog, choose *Qt Quick Application - Empty*
 * - Choose the location and make sure to select *Android for armeabi-v7a* in the *Kits* page of the wizard, named *Kit Selection*
 *
 * \subsection files Files
 * The *new project wizard* should have created the following files:
 * - main.cpp
 * - main.qml
 * - qml.qrc
 * - myqmlproject.pro
 * - myqmlproject.pro.user
 *
 * \subsection pro_file Add the support for *cumbia-qtcontrols* and *cumbia-websocket* in the *myqmlproject.pro* project file
 *
 * \code
 include(/usr/local/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
 include(/usr/local/include/cumbia-websocket/cumbia-websocket.pri)

 QT += quick
 CONFIG += c++11

 # ... pro file continues...
 #
 * \endcode
 *
 *
 * \subsection main_cpp The main.cpp file
 *
 * The main.cpp file employs the following objects in order to use
 * *cumbia-qtcontrols* and *cumbia-websocket*:
 *
 * - CumbiaPool: selects the appropriate *cumbia* implementation for the engine at runtime, according
 *               to the names of the sources provided;
 * - CumbiaWebSocket: provides access to the *cumbia websocket* module;
 * - CumbiaPool_O: a QObject that wraps CumbiaPool so that it can be accessed from QtQuick/QML (needs to be a QObject)
 * - QQuickView: the view for the application
 * - CuAppStateManager: an utility class that deals with application state changes (active, inactive, hidden, suspended)
 *
 * An example of a main.cpp is shown below
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

    // if we use Qt Charts, we must select QApplication, not QGuiApplication
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

    // tidy up everything
    delete view;
    delete cuws;
    delete cu_pool;
    return ret;
}

 * \endcode
 *
 * \subsection qmlfiles The qml files
 * The main.qml uses a *SwipeView* to swipe between three pages, each one defined in three
 * separate qml files: Page1.qml, Page2.qml and Page3.qml
 *
 * \subsubsection main_qml main.qml
 *
 * \code
import QtQuick 2.9
import QtQuick.Controls 2.4
import eu.elettra.cumbiaqmlcontrols 1.0

SwipeView {
    id:swipeView

    currentIndex: 0
    anchors.fill: parent

    Page1 {}
    Page2 {}
    Page3 {}

    PageIndicator {
        id: indicator
        count: swipeView.count
        currentIndex: swipeView.currentIndex
        anchors.bottom: parent.bottom
//            anchors.horizontalCenter: swipeView.horizontalCenter
    }
}
 * \endcode
 *
 * \subsubsection page1 Page1.qml
 * \code
import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0

CumbiaSpectrumChart {
    sources:  "tango://ken.elettra.trieste.it:20000/test/device/1/float_spectrum_ro;tango://ken.elettra.trieste.it:20000/test/device/1/double_spectrum_ro"
    id: spectrumChart
    title: "float, double spectrum"
}
 * \endcode
 *
 * \subsubsection page2 Page2.qml
 * \code
import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0

CumbiaTrendChart {
    sources:  "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar;tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar;tango://ken.elettra.trieste.it:20000/test/device/1/short_scalar"
    id: scalarChart
    title: "double, long, short"
}

 * \endcode
 *
 * \subsubsection page3 Page3.qml
 *
 * \code
import QtQuick 2.0
import eu.elettra.cumbiaqmlcontrols 1.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

ScrollView {
    id: secondPage
    width:300
    height:300
    clip: true

    ColumnLayout {
        id: grid
        spacing: 6

        Text {
            id: text1
            text: qsTr("long_scalar")
            font.pixelSize: 12
            width:parent.width
            Layout.margins: 10
        }
        CumbiaLabel {
            id: cumbiaReadLabel
            styleColor: "#0d99f2"
            source:"tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar"
            Layout.fillWidth: true
            Layout.margins: 10
        }
        CumbiaCircularGauge {
            id:longCircularGauge
            source:"tango://ken.elettra.trieste.it:20000/test/device/1/long_scalar"
            Layout.fillWidth: true
            Layout.margins: 10
        }
        Text {
            id: text2
            text: qsTr("double_scalar")
            font.pixelSize: 12
            Layout.fillWidth: true
            Layout.margins: 10
        }
        CumbiaLabel {
            id: cumbiaReadLabel1
            styleColor: "#0d99f2"
            source: "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
            Layout.fillWidth: true
            Layout.margins: 10
        }
        CumbiaCircularGauge {
            id:doubleCircularGauge
            source: "tango://ken.elettra.trieste.it:20000/test/device/1/double_scalar"
            Layout.fillWidth: true
            Layout.margins: 10
        }
    }
}
 *
 * \endcode
 *
 * The following library dependencies must be added to the project.
 * In Qt creator go to the *Projects* section, select *Android for armeabi-v7a* and  then *Build*.
 * In the *Build Android APK* section, *Additional Libraries*, add
 * - libcumbia.so
 * - libcumbia-qtcontrols-qt5.so
 * - libcumbia-websocket-qt5.so
 * - libcumbia-qtcontrols-qml-qt5plugin.so
 *
 * and the *ssl* libraries that must have been compiled for android as described in the file
 * *cumbia-websocket/android/README.openssl.txt*
 *
 * - libssl.so
 * - libcrypto.so
 *
 * The inclusion of the *so* libraries above listed is reflected by a section like this
 * in the *myqmlproject.pro* project file:
 *
 * \code

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../../../../../../../../libs/armeabi-v7a/libcumbia.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../../../../../../../../libs/armeabi-v7a/libcumbia-qtcontrols-qt5.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../cumbia-websocket/android/openssl-lib/libcrypto.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../cumbia-websocket/android/openssl-lib/libssl.so \
        /archivi/devel/utils/git/cppqtclients/cumbia-libs/cumbia-qtcontrols/qml/examples/qmlandroidpwma/../../../../../../../../../../../libs/armeabi-v7a/libcumbia-websocket-qt5.so \
        $$PWD/../../../../../../../../../../../usr/local/qt-5.11.1/5.11.1/android_armv7/qml/eu/elettra/cumbiaqmlcontrols/libcumbia-qtcontrols-qml-qt5plugin.so
}
 * \endcode
 *
 *
 * The three pages in the resulting application look like this:
 *
 * \image html spectrum_android.jpg [Tango spectrum]
 * \image latex spectrum_android.jpg [Tango spectrum]
 *
 * \image html scalar_android.jpg [Tango scalar]
 * \image latex scalar_android.jpg [Tango scalar]
 *
 * \image html elements_qml.jpg [Tango scalar, QML components]
 * \image latex elements_qml.jpg [Tango scalar, QML component]
 *
 */

class CUMBIAWEBSOCKETSHARED_EXPORT CumbiaWebSocket : public Cumbia, public CuWSClientListener
{

public:
    enum Type { CumbiaWSType = Cumbia::CumbiaUserType + 12 };

    CumbiaWebSocket(const QString& websocket_url,
                    const QString& http_url,
                    CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaWebSocket();

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    QString httpUrl() const;

    QString websocketUrl() const;

    virtual int getType() const;

    CuWSClient *websocketClient() const;

    void addAction(const std::string &source, CuDataListener *l, const CuWSActionFactoryI &f);
    void unlinkListener(const string &source, CuWSActionI::Type t, CuDataListener *l);
    CuWSActionI *findAction(const std::string &source, CuWSActionI::Type t) const;

    // CuWSClientListener interface
public:
    virtual void onUpdate(const QString& message);

private:

    void m_init();

    CumbiaWebSocketPrivate *d;

};

#endif // CUMBIAWEBSOCKET_H
