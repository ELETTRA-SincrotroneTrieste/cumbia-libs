#include "webass-webso.h"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>

#ifdef QUMBIA_EPICS_CONTROLS_VERSION
#include <cumbiaepics.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cuepics-world.h>
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cumbiatango.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cutcontrols-utils.h>
#endif
#ifdef CUMBIA_RANDOM_VERSION
#include <cumbiarandom.h>
#include <curndreader.h>
#include <curndactionfactories.h>
#include <cumbiarndworld.h>
#include <cuwstangohelper.h>
#endif

#ifdef CUMBIA_WEBSOCKET_VERSION
#include <cumbiawebsocket.h>
#include <cumbiawsworld.h>
#include <cuwscontrolsreader.h> // for CuWSReaderFactory
#include <cuwscontrolswriter.h>
#include <cuwstangoreplacewildcards.h>
#endif

#include <QtDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <qulabel.h>
#include <qutrendplot.h>
#include <quspectrumplot.h>
#include <qubutton.h>
#include <quapplynumeric.h>
#include <QGridLayout>

WebassWebso::WebassWebso(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    cu_pool = cumbia_pool;

#ifdef QUMBIA_EPICS_CONTROLS_VERSION
    CumbiaEpics* cuep = nullptr;
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    CumbiaTango* cuta = nullptr;
#endif

    QStringList engines;
#ifdef CUMBIA_WEBSOCKET_VERSION
    QCommandLineParser parser;
    QCommandLineOption ws_url_o(QStringList() << "u" << "websocket-url", "URL to websocket server", "url", "ws://localhost:12702");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(ws_url_o);
    parser.process(*qApp);
    QString ws_url;
    if(parser.isSet(ws_url_o))
        ws_url = parser.value(ws_url_o);
    //
    // WebAssembly if command line arguments are not working
    // ws_url = "ws://localhost:12702";
    //
    if(!ws_url.isEmpty()) {
        // setup Cumbia web socket with the web socket address and the host name to prepend to the sources
        // for the HTTP requests
        CumbiaWSWorld wsw;
        CumbiaWebSocket* cuws = new CumbiaWebSocket(ws_url, "", new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        cu_pool->registerCumbiaImpl("ws", cuws);
        m_ctrl_factory_pool.registerImpl("ws", CuWSReaderFactory());
        m_ctrl_factory_pool.registerImpl("ws", CuWsControlsWriterFactory());
        // example source: "ws://tango://hokuto:20000/test/device/1/double_scalar"
        // ws:// domain prefix will be discarded
        //
        // case 1: websocket specific app
        //
        // m_ctrl_factory_pool.setSrcPatterns("ws", wsw.srcPatterns());
        // cu_pool->setSrcPatterns("ws", wsw.srcPatterns());
        //
        // case 2: access Tango through a websocket server
        //
        // Share the same source syntax (e.g. $1/attribute, my/tango/dev/attribute)
        // across native tango engine and websocket proxy server.
        // This allows to leave the application code unchanged. See (*) below
        //
        CuWsTangoReplaceWildcards *tgrwi = new CuWsTangoReplaceWildcards(/*QStringList() << "test/device/1"*/);
        cuws->addReplaceWildcardI(tgrwi);
        CuWsTangoHelper th;
        m_ctrl_factory_pool.setSrcPatterns("ws", th.srcPatterns());
        cu_pool->setSrcPatterns("ws", th.srcPatterns());

        // log
        cuws->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
        //
        // open the websocket
        //
        cuws->openSocket();
        //
        engines << "websocket";
    } // parser.isSet(ws_url_o)
#endif // #ifdef CUMBIA_WEBSOCKET_VERSION

    //
    // (*) additional engines are registered if websocket is not in use so that applications
    // can rely either on websocket or other engines without changing the definition of
    // their sources (i.e. without code changes and rebuild)
    //
    if(!cu_pool->get("ws")) {
        // cumbia-random
#ifdef CUMBIA_RANDOM_VERSION
        CumbiaRandom *cura = new CumbiaRandom(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        CumbiaRNDWorld rndw;
        cu_pool->registerCumbiaImpl("random", cura);
        m_ctrl_factory_pool.registerImpl("random", CuRNDReaderFactory());
        m_ctrl_factory_pool.setSrcPatterns("random", rndw.srcPatterns());
        cu_pool->setSrcPatterns("random", rndw.srcPatterns());
        cura->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
        engines << "random";
#endif

        // EPICS
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
        cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        cu_pool->registerCumbiaImpl("epics", cuep);
        m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
        m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
        cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
        CuEpicsWorld ew;
        m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
        cu_pool->setSrcPatterns("epics", ew.srcPatterns());
        engines << "epics";
#endif

        // Tango
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
        cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        cu_pool->registerCumbiaImpl("tango", cuta);
        m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
        m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
        cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
        CuTangoWorld tw;
        m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
        cu_pool->setSrcPatterns("tango", tw.srcPatterns());
        engines << "tango";
#endif
    }


    // needs DEFINES -= QT_NO_DEBUG_OUTPUT in .pro
    qDebug() << __PRETTY_FUNCTION__ << "available engines" << engines;

    QGridLayout *lo = new QGridLayout(this);
    QuLabel *la = new QuLabel(this, cu_pool, m_ctrl_factory_pool);
    la->setSource("$1/double_scalar");
    lo->addWidget(la, 0, 0, 1, 10);
    QuTrendPlot *tp = new QuTrendPlot(this, cu_pool, m_ctrl_factory_pool);
    tp->setSources(QStringList() << "$1/double_scalar" /*<< "$1/long_scalar"*/);
    lo->addWidget(tp, 1, 0, 5, 10);
    QLabel *swsl = new QLabel("Switch States", this);
    QuLabel *stl = new QuLabel(this, cu_pool, m_ctrl_factory_pool);
    stl->setSource("$1->State");
    QuButton *bu = new QuButton(this, cu_pool, m_ctrl_factory_pool);
    bu->setTarget("$1->SwitchStates");
    bu->setText("State");
    QLabel *dsl = new QLabel("double_scalar", this);
    QuApplyNumeric *an = new QuApplyNumeric(this, cu_pool, m_ctrl_factory_pool);
    an->setTarget("$1/double_scalar");

    lo->addWidget(swsl, 6, 0, 1, 1);
    lo->addWidget(stl, 6, 1, 1, 1);
    lo->addWidget(bu, 6, 2, 1, 1);
    lo->addWidget(dsl, 6, 3, 1, 1);
    lo->addWidget(an, 6, 4, 1, 1);

    QuSpectrumPlot *sp = new QuSpectrumPlot(this, cu_pool, m_ctrl_factory_pool);
    sp->setSources(QStringList() << "$1/double_spectrum_ro" << "$1/long_spectrum_ro");
    lo->addWidget(sp, 6, 0, 5, 10);

}

WebassWebso::~WebassWebso()
{
}
