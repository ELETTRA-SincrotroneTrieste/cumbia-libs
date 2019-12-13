#include "qumbia-client.h"
#include "ui_qumbia-client.h"
#include "writer.h"
#include <cumbiapool.h>
#include <cumacros.h>

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutreader.h> // PolledRefresh / EventRefresh
#include <cutango-world.h>
#endif

#ifdef CUMBIA_RANDOM_VERSION
#include <cumbiarandom.h>
#include <curndreader.h>
#include <curndactionfactories.h>
#include <cumbiarndworld.h>
#endif

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <qulabel.h>
#include <qubutton.h>
#include <qutrendplot.h>
#include <quspectrumplot.h>
#include <qulineedit.h>
#include <QGridLayout>
#include <cuserviceprovider.h>
#include <qulogimpl.h>
#include <QtDebug>
#include <QMessageBox>
#include <QScrollArea>
#include <QSpinBox>
#include <QComboBox>
#include <cucontext.h>
#include <QTimer>

#ifdef QUMBIA_EPICS_CONTROLS
#include <cumbiaepics.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cuepics-world.h>
#include <cuepreadoptions.h>
#endif

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

QumbiaClient::QumbiaClient(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QumbiaClient),
    m_layoutColumnCount(10)
{
    QStringList engines;
    // for valgrind test
    // int *f= new int[1000];
    cu_pool = cumbia_pool;
    m_switchCnt = 0;
    // setup Cumbia pool and register cumbia implementations for tango and epics
#ifdef QUMBIA_EPICS_CONTROLS
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());
    cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    engines << "EPICS";
#endif

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    engines << "tango";
#endif

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

    ui->setupUi(this);
    connect(ui->pbSetSources, SIGNAL(clicked()), this, SLOT(sourcesChanged()));
    connect(ui->pbApplyRefresh, SIGNAL(clicked()), this, SLOT(changeRefresh()));
    connect(ui->pbUnsetSrc, SIGNAL(clicked()), this, SLOT(unsetSources()));

    if(qApp->arguments().count() >= 2)
    {
        for(int i = 1; i < qApp->arguments().count(); i++)
        {
            ui->leSrcs->setText(ui->leSrcs->text() + " " + qApp->arguments().at(i));
        }
        sourcesChanged();
    }

    resize(1000, 600);

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(switchSources()));
    t->setInterval(1000);
    // t->start();


    QVBoxLayout *wblo = new QVBoxLayout(ui->gbWriters);
    QScrollArea *scrollA = new QScrollArea(0);
    QWidget *writersContentW = new QWidget(this);
    QVBoxLayout *writersLo = new QVBoxLayout(writersContentW);
    scrollA->setWidget(writersContentW);
    scrollA->setWidgetResizable(true);
    wblo->addWidget(scrollA);

    connect(ui->pbWrite, SIGNAL(toggled(bool)), ui->gbWriters, SLOT(setVisible(bool)));
    ui->gbWriters->setVisible(false);

    // engines information
    ui->lengines->setText("Engines: " + engines.join(", "));
}

QumbiaClient::~QumbiaClient()
{
    delete ui;
}

void QumbiaClient::configure(const CuData &d)
{
    if(d["type"].toString() == "property") {
        sender()->disconnect(this, SLOT(configure(CuData)));
        const int plotRowCnt = 5;
        int layout_row = 2;
        std::string format = d["data_format_str"].toString();

        QGridLayout *lo = qobject_cast<QGridLayout *>(ui->widget->layout());
        int data_dim = 1;
        if(format == "scalar")
        {
            QuTrendPlot *plot = findChild<QuTrendPlot *>();
            if(!plot)
            {
                plot = new QuTrendPlot(this, cu_pool, m_ctrl_factory_pool);
                if(findChild<QuSpectrumPlot *>()) /* there's a spectrum plot already */
                    layout_row += plotRowCnt;
                lo->addWidget(plot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
            }
            plot->configure(d);
            plot->addSource(d["src"].toString().c_str());
        }
        else if(format == "vector")
        {
            QuSpectrumPlot *splot = findChild<QuSpectrumPlot *>();
            if(!splot)
            {
                splot = new QuSpectrumPlot(this, cu_pool, m_ctrl_factory_pool);
                if(findChild<QuTrendPlot *>()) /* there's already a trend plot */
                    layout_row += plotRowCnt;
                lo->addWidget(splot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
            }
            splot->configure(d);
            splot->addSource(d["src"].toString().c_str());

            data_dim = d["dim_x"].toLongInt();
            if(data_dim <= 0)
                data_dim = d["value"].getSize();
            if(data_dim <= 0 && d.containsKey("max_dim_x"))
                data_dim = d["max_dim_x"].toULongInt();
        }

        if(d["writable"].toInt() > 0)
        {
            QWidget *wi = ui->gbWriters->findChild<QScrollArea *>()->widget();
            ui->pbWrite->setChecked(true);
            Writer *w = new Writer(wi, cu_pool, m_ctrl_factory_pool, data_dim, QString::fromStdString(d["src"].toString()));
            qobject_cast<QVBoxLayout *>(wi->layout())->addWidget(w);
        }
    }
}

void QumbiaClient::changeRefresh()
{
    int period = ui->sbPeriod->value();
    int refmode;
    switch(ui->cbRefMode->currentIndex()) {
    case 0:
        refmode = CuTReader::PolledRefresh;
        break;
    case 1:
    default:
        refmode = CuTReader::ChangeEventRefresh;
        break;
    }

    CuData options;
    options["period"] = period;
    options["refresh_mode"] = refmode;

    QuTrendPlot *tp = findChild<QuTrendPlot *>();
    if(tp) {
        printf("sending data %s to plot \n", options.toString().c_str());
        tp->getContext()->sendData(options);
    }
    QuSpectrumPlot *sp = findChild<QuSpectrumPlot *>();
    if(sp) {
        sp->getContext()->sendData(options);
    }
}

void QumbiaClient::sourcesChanged()
{
    /* clear widgets */
    QGridLayout *lo = NULL;
    if(!ui->widget->layout())
        lo = new QGridLayout(ui->widget);
    else
        lo = qobject_cast<QGridLayout *>(ui->widget->layout());

    int period = ui->sbPeriod->value();
    int refmode = ui->cbRefMode->currentIndex();
    CuData options;
    options["period"] = period;
    options["refresh_mode"] = refmode;

    QStringList srcs = ui->leSrcs->text().split(QRegExp("\\s+"), QString::SkipEmptyParts);
    const int srcCnt = srcs.size();
    const int colSpan = m_layoutColumnCount / srcCnt;

    if(m_oldSrcs.size() == 0) {
        foreach(QuLabel *l, ui->widget->findChildren<QuLabel *>())
            delete l;
    }

    QStringList newSrcs;
    foreach(QString s, srcs)
        if(!m_oldSrcs.contains(s))
            newSrcs << s;

    QStringList remSrcs;
    foreach(QString s, m_oldSrcs)
        if(!srcs.contains(s))
            remSrcs << s;

    qDebug() << __FUNCTION__ << "m_oldSrcs" << m_oldSrcs;
    qDebug() << __FUNCTION__ << "NEW SRCS" << newSrcs << " TO REMOVE " << remSrcs;

    QuTrendPlot *tr_plot = ui->widget->findChild<QuTrendPlot *>();
    if(tr_plot)
    {
        QStringList psrcs = tr_plot->sources();
        foreach(QString s, remSrcs)
            if(psrcs.contains(s))
                tr_plot->unsetSource(s);
        if(!tr_plot->sources().count())
        {
            printf("\e[1;31mDELETING SCALAR PLOTTTT\e[0m\n");

            delete tr_plot;
        }
    }

    QuSpectrumPlot *sp_plot = ui->widget->findChild<QuSpectrumPlot *>();
    if(sp_plot)
    {
        QStringList psrcs = sp_plot->sources();
        foreach(QString s, remSrcs)
        {
            printf("see if has src %s\n", qstoc(s));
            if(psrcs.contains(s))
            {
                printf("unsetting source on spectrum plot src %s\n", qstoc(s));
                sp_plot->unsetSource(s);
            }
        }
        if(!sp_plot->sources().count())
        {
            printf("\n\n\n\n\e[1;31mDELETING SPECTRUM PLOTTTT\e[0m\n\n\n\n\n");
            delete sp_plot;
        }
    }

    foreach(QuLabel *l, ui->widget->findChildren<QuLabel *>())
        if(remSrcs.contains(l->source()))
            delete l;
    foreach(QuLabel *l, ui->widget->findChildren<QuLabel *>())
        lo->removeWidget(l);

    /* redistribute remaining labels */
    int col = 0;
    foreach(QuLabel *l, ui->widget->findChildren<QuLabel *>())
        lo->addWidget(l, 0, (col++) * colSpan, 1, colSpan);

    m_do_conf = true; /* repopulate after sources change */

    ui->labelTitle->setText(ui->leSrcs->text());

    for(int i = 0; i < newSrcs.size(); i++)
    {
        QuLabel *l = new QuLabel(this, cu_pool, m_ctrl_factory_pool);
        l->getContext()->setOptions(options);
        l->setWordWrap(true);
        l->setMaximumLength(30); /* truncate if text is too long */
        connect(l, SIGNAL(newData(const CuData&)), this, SLOT(configure(const CuData&)));
        l->setSource(newSrcs.at(i));
        lo->addWidget(l, 0, (i + col) * colSpan, 1, colSpan);
    }

    m_oldSrcs = srcs;
}

void QumbiaClient::unsetSources()
{
    foreach(QuTrendPlot *plot, ui->widget->findChildren<QuTrendPlot *>())
        plot->unsetSources();
    foreach(QuSpectrumPlot *sp, ui->widget->findChildren<QuSpectrumPlot *>())
        sp->unsetSources();
    foreach(QuLabel *l, findChildren<QuLabel *>())
        l->unsetSource();
    m_oldSrcs.clear();
}

void QumbiaClient::switchSources()
{
    m_switchCnt++;
    QString src = QString("test/device/%1/double_spectrum_ro").arg(m_switchCnt % 2 + 1);
    ui->leSrcs->setText(src);
    sourcesChanged();
}

