#include "randomwi.h"
#include "ui_randomwi.h"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>
#include <cumbiarandom.h>
#include <cumbiarndworld.h>
#include <curndreader.h>

Randomwi::Randomwi(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Randomwi)
{
    cu_pool = cumbia_pool;
    // setup Cumbia pool and register cumbia implementations for random
    CumbiaRandom *cura = new CumbiaRandom(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaRNDWorld rndw;
    cu_pool->registerCumbiaImpl("random", cura);
    m_ctrl_factory_pool.registerImpl("random", CuRNDReaderFactory());
    m_ctrl_factory_pool.setSrcPatterns("random", rndw.srcPatterns());
    cu_pool->setSrcPatterns("random", rndw.srcPatterns());
    // log
    cura->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

//    ui->quTrendPlot->setSource("random://th1/scalar/1/-10.0/10.2/r1");
    ui->quLabel->setSource("random://th1/scalar/1/-10.0/10.2/r1");
    ui->quCircularGauge->setSource("random://th1/scalar/1/-10.0/10.2/r1");
//    ui->quLed->setSource("random://th1/scalar/bool/1/0/1/b1");
//    ui->quTrendPlot->setSource("random://th1/scalar/1/-100.0/100.0/r2");
//    ui->quSpectrumPlot->setSource("random://th1/spectrum/100/-100.0/100.0/r2");
}

Randomwi::~Randomwi()
{
    delete ui;
}
