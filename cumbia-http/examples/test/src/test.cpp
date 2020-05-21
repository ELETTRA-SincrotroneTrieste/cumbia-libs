#include "test.h"
#include "ui_test.h"

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
#ifdef CUMBIA_HTTP_VERSION
#include <cuhttpregisterengine.h>
#endif

Test::Test(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Test)
{
    QStringList engines;
    cu_pool = cumbia_pool;
    Cumbia *cuws = nullptr, *cuta = nullptr, *cuhttp = nullptr;
    Cumbia *cuep = nullptr, *cura = nullptr;
    m_log = new CuLog(&m_log_impl);

#if defined(CUMBIA_WEBSOCKET_VERSION) || defined (CUMBIA_HTTP_VERSION)
    QCommandLineParser parser;
#endif

    // websocket engine
#ifdef CUMBIA_WEBSOCKET_VERSION
    CuWsRegisterEngine wsre;
    if(wsre.hasCmdOption(&parser, qApp->arguments())) {
        cuws = wsre.registerWithDefaults(cumbia_pool, m_ctrl_factory_pool);
        static_cast<CumbiaWebSocket *>(cuws)->openSocket();
        cuws->getServiceProvider()->registerSharedService(CuServices::Log, m_log);
        engines << "websocket";
    }
#endif
#ifdef CUMBIA_HTTP_VERSION
    CuHttpRegisterEngine httpre;
    if(httpre.hasCmdOption(&parser, qApp->arguments())) {
        cuhttp = httpre.registerWithDefaults(cumbia_pool, m_ctrl_factory_pool);
        cuhttp->getServiceProvider()->registerSharedService(CuServices::Log, m_log);
        engines << "http";
    }
#endif
    // other engines, if websocket is not in use
    if(!cuws && !cuhttp) {
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
        CuTangoRegisterEngine tare;
        cuta = tare.registerWithDefaults(cu_pool, m_ctrl_factory_pool);
        cuta->getServiceProvider()->registerSharedService(CuServices::Log, m_log);
        engines << "tango";
#endif
#ifdef CUMBIA_RANDOM_VERSION
        CuRndRegisterEngine rndre;
        cura = rndre.registerWithDefaults(cu_pool, m_ctrl_factory_pool);
        cura->getServiceProvider()->registerSharedService(CuServices::Log, m_log);
        engines << "random";
#endif
#ifdef QUMBIA_EPICS_CONTROLS_VERSION
        CuEpRegisterEngine epre;
        cuep = epre.registerWithDefaults(cu_pool, m_ctrl_factory_pool);
        cuep->getServiceProvider()->registerSharedService(CuServices::Log, m_log);
        engines << "epics";
#endif
    }

    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    // needs DEFINES -= QT_NO_DEBUG_OUTPUT in .pro
    qDebug() << __PRETTY_FUNCTION__ << "available engines" << engines;
}

Test::~Test()
{
    delete ui;
}
