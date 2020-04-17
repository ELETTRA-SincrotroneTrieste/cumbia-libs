#include "$HFILE$"
#include "ui_$HFILE$"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>

#include <QtDebug>
#include <QCommandLineParser>

#ifdef QUMBIA_EPICS_CONTROLS_VERSION
#include <cuepregisterengine.h>
#endif
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cutangoregisterengine.h>
#endif
#ifdef CUMBIA_RANDOM_VERSION
#include <curndregisterengine.h>
#endif
#ifdef CUMBIA_WEBSOCKET_VERSION
#include <cuwsregisterengine.h>
#endif

$MAINCLASS$::$MAINCLASS$(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::$UIFORMCLASS$)
{
    QStringList engines;
    cu_pool = cumbia_pool;
    Cumbia *cuws = nullptr, *cuta = nullptr;
    Cumbia *cuep = nullptr, *cura = nullptr;

    // websocket engine
#ifdef CUMBIA_WEBSOCKET_VERSION
    QCommandLineParser parser;
    CuWsRegisterEngine wsre;
    if(wsre.hasCmdOption(&parser, qApp->arguments())) {
        cuws = wsre.registerWithDefaults(cumbia_pool, m_ctrl_factory_pool);
        static_cast<CumbiaWebSocket *>(cuws)->openSocket();
        cuws->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
        engines << "websocket";
    }
#endif
    // other engines, if websocket is not in use
    if(!cuws) {
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
        CuTangoRegisterEngine tare;
        cuta = tare.registerWithDefaults(cu_pool, m_ctrl_factory_pool);
        cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
        engines << "tango";
#endif
#ifdef CUMBIA_RANDOM_VERSION
        CuRndRegisterEngine rndre;
        cura = rndre.registerWithDefaults(cu_pool, m_ctrl_factory_pool);
        cura->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
        engines << "random";
#endif
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
        CuEpRegisterEngine epre;
        cuep = epre.registerWithDefaults(cu_pool, m_ctrl_factory_pool);
        cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
        engines << "epics";
#endif
    }

    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    // needs DEFINES -= QT_NO_DEBUG_OUTPUT in .pro
    qDebug() << __PRETTY_FUNCTION__ << "available engines" << engines;
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
