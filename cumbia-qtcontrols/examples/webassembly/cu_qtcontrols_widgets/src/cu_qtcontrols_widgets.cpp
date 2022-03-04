#include "cu_qtcontrols_widgets.h"

#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>

#ifdef CUMBIA_RANDOM_VERSION
#include <cumbiarandom.h>
#include <curndreader.h>
#include <curndactionfactories.h>
#include <cumbiarndworld.h>
#include <quspectrumplot.h>
#endif


#include <QtDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <QGridLayout>
#include <qulabel.h>
#include <qutrendplot.h>
#include <qucirculargauge.h>
#include <QTimer>

Cu_qtcontrols_widgets::Cu_qtcontrols_widgets(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    bool use_curandom = true;
    m_cnt = 0;
    cu_pool = new CumbiaPool();
    // cumbia-random
#ifdef CUMBIA_RANDOM_VERSION
    CumbiaRandom *cura = new CumbiaRandom(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaRNDWorld rndw;
    cu_pool->registerCumbiaImpl("random", cura);
    m_ctrl_factory_pool.registerImpl("random", CuRNDReaderFactory());
    m_ctrl_factory_pool.setSrcPatterns("random", rndw.srcPatterns());
    cu_pool->setSrcPatterns("random", rndw.srcPatterns());
    cura->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

    QGridLayout *lo = new QGridLayout(this);
    QuLabel *la = new QuLabel(this, cu_pool, m_ctrl_factory_pool);
    lo->addWidget(la, 0, 0, 2, 2);
    la->setText("waiting");

    QuTrendPlot *plot = new QuTrendPlot(this, cu_pool, m_ctrl_factory_pool);
    plot->setSources(QStringList() << "random://one/random/double/1/-250/250/plot_r");
    QuSpectrumPlot *splot = new QuSpectrumPlot(this, cu_pool, m_ctrl_factory_pool);
    splot->setSources(QStringList() << "random://one/random/double/100/-250/250/plot_r");
    QuCircularGauge *gauge = new QuCircularGauge(this, cu_pool, m_ctrl_factory_pool);
    gauge->setSource("random://one/random/double/1/-100/100/plot_r");

    lo->addWidget(gauge, 0, 1, 2, 2);

    lo->addWidget(plot, 2, 0, 4, 4);
    lo->addWidget(splot, 6, 0, 4, 4);

    if(use_curandom) {
        la->setSource("random://one/random/number/1/0/100/one_r");
    } else {
        QTimer *t = new QTimer(this);
        t->setSingleShot(false);
        t->setInterval(1000);
        connect(t, SIGNAL(timeout()), this, SLOT(onTimeo()));
        t->start();
    }

#endif
}

Cu_qtcontrols_widgets::~Cu_qtcontrols_widgets()
{

}

void Cu_qtcontrols_widgets::onTimeo()
{
    m_cnt++;
    findChild<QuLabel *>()->setText(QString::number(m_cnt));
}
