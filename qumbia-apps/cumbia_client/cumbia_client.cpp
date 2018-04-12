#include "cumbia_client.h"
#include "ui_cumbia_client.h"
#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cumacros.h>
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
#include <QSpinBox>
#include <QComboBox>
#include <cutango-world.h>
#include <cucontext.h>
#include <cucontextactionbridge.h>
#include <QTimer>

#ifdef QUMBIA_EPICS_CONTROLS
    #include <cumbiaepics.h>
    #include <cuepcontrolsreader.h>
    #include <cuepcontrolswriter.h>
    #include <cuepics-world.h>
    #include <cuepreadoptions.h>
    #include <queplotupdatestrategy.h>
#endif

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>

CumbiaClient::CumbiaClient(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CumbiaClient),
    m_layoutColumnCount(10)
{
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
#endif

    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());

    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());

   // m_log = new CuLog(&m_log_impl);
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

    ui->setupUi(this);
    connect(ui->pbSetSources, SIGNAL(clicked()), this, SLOT(sourcesChanged()));
    connect(ui->pbApplyRefresh, SIGNAL(clicked()), this, SLOT(changeRefresh()));
    connect(ui->pbUnsetSrc, SIGNAL(clicked()), this, SLOT(unsetSources()));

    if(qApp->arguments().count() >= 2)
    {
        for(int i = 1; i < qApp->arguments().count(); i++)
        {
            if(i > 1 && i < qApp->arguments().count() - 1)
                ui->leSrcs->setText(ui->leSrcs->text() + " ");
            ui->leSrcs->setText(ui->leSrcs->text() + qApp->arguments().at(i));
        }
        sourcesChanged();
    }

    resize(1000, 600);

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(switchSources()));
    t->setInterval(1000);
    // t->start();


    new CuContextActionBridge(this, cumbia_pool, m_ctrl_factory_pool);
}

CumbiaClient::~CumbiaClient()
{
    delete ui;
}

void CumbiaClient::configure(const CuData &d)
{
    if(d.containsKey("data_format_str"))
        sender()->disconnect(this, SLOT(configure(CuData)));

    qDebug() << __FUNCTION__ << "labels" << findChildren<QuLabel *>();



    printf("+++++++++++---------------- configure %s ----------------+++++++++++++++++\n", d.toString().c_str());

    const int plotRowCnt = 5;
    int layout_row = 2;
    std::string format = d["data_format_str"].toString();

    QGridLayout *lo = qobject_cast<QGridLayout *>(ui->widget->layout());
//    foreach(QuLabel *l, findChildren<QuLabel *>())
//       delete l;


//    for(int i = 0; i < 10; i++)
//    {
//        QuLabel *l = new QuLabel(this, cu_pool, m_ctrl_factory_pool);
//        l->setSource(d["src"].toString().c_str());
//        lo->addWidget(l, ++layout_row, 0, 1, m_layoutColumnCount);
//    }

//       foreach(QuLabel *l, findChildren<QuLabel *>())
//         delete l;
//    return;

    //    for(int i = 0; i < srcs.size() && d["writable"].toInt() == Tango::READ_WRITE; i++)
    //    {
    //        QuButton *b = new QuButton(this, cu_ep, CuTWriterFactory());
    //        b->setText("Write");
    //        QString target = srcs.at(i) + "(";
    //        if(format == 1) /* Tango::SPECTRUM */
    //        {
    //            /* put m_layoutColumnCount - 1 line edits */
    //            for(int j = 0; j < m_layoutColumnCount - 1; j++)
    //            {
    //                QuLineEdit *le = new QuLineEdit(this);
    //                le->setObjectName(QString("leInputArgs_%1").arg(j + (i * m_layoutColumnCount)));
    //                lo->addWidget(le, layout_row, j, 1, 1);
    //                target += "&" + le->objectName() + ",";
    //            }
    //            lo->addWidget(b, layout_row, m_layoutColumnCount -1, 1, 1);
    //        }
    //        else
    //        {
    //            QuLineEdit *le = new QuLineEdit(this);
    //            le->setObjectName("leInputArgs");
    //            lo->addWidget(le, layout_row, 0, 1, m_layoutColumnCount - 1);
    //            target += "&" + le->objectName();
    //            lo->addWidget(b, layout_row, m_layoutColumnCount - 1, 1, 1);
    //        }
    //        target += ")";
    //        //    b->setTargets(target);
    //        b->setToolTip(b->targets());
    //        layout_row++;
    //    }

    if(format == "scalar")
    {
        QuTrendPlot *plot = findChild<QuTrendPlot *>();
        if(plot)
            ; //lo->removeWidget(plot);
        else
        {
            plot = new QuTrendPlot(this, cu_pool, m_ctrl_factory_pool);
            if(findChild<QuSpectrumPlot *>()) /* there's a spectrum plot already */
                layout_row += plotRowCnt;
            lo->addWidget(plot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
        }
        //  plot->setContextMenuStrategy(ctx_menu_strat);
        printf("addSource to plto scalar\n");
        plot->addSource(d["src"].toString().c_str());
    }
    else if(format == "vector")
    {
        QuSpectrumPlot *splot = findChild<QuSpectrumPlot *>();
        if(splot)
            ; //lo->removeWidget(splot);
        else
        {
            splot = new QuSpectrumPlot(this, cu_pool, m_ctrl_factory_pool);
            if(findChild<QuTrendPlot *>()) /* there's already a trend plot */
                layout_row += plotRowCnt;
            lo->addWidget(splot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
        }
        //splot->setContextMenuStrategy(ctx_menu_strat);
        splot->addSource(d["src"].toString().c_str());
    }
//    foreach(QuLabel *l, findChildren<QuLabel *>())
//       delete l;

    CuContextActionBridge *cab = findChild<CuContextActionBridge *>();
    if(cab) delete cab;
    cab = new CuContextActionBridge(this, cu_pool, m_ctrl_factory_pool);
}

void CumbiaClient::changeRefresh()
{
    int period = ui->sbPeriod->value();
    int refmode = ui->cbRefMode->currentIndex();
    CuData options;
    options["period"] = period;
    options["refresh_mode"] = refmode;
    QuTrendPlot *tp = findChild<QuTrendPlot *>();
    if(tp)
        tp->getContext()->sendData(options);
    QuSpectrumPlot *sp = findChild<QuSpectrumPlot *>();
    if(sp)
        sp->getContext()->sendData(options);
}

void CumbiaClient::sourcesChanged()
{
    /* clear widgets */
    QGridLayout *lo = NULL;
    if(!ui->widget->layout())
        lo = new QGridLayout(ui->widget);
    else
        lo = qobject_cast<QGridLayout *>(ui->widget->layout());

    //  unsetSources();

    QStringList srcs = ui->leSrcs->text().split(QRegExp("\\s+"), QString::SkipEmptyParts);
    const int srcCnt = srcs.size();
    const int colSpan = m_layoutColumnCount / srcCnt;

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
        printf("spectrum plot found!\n");
        QStringList psrcs = sp_plot->sources();
        printf("spectrum plot got sources %d!\n", psrcs.size());
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
        l->setMaximumLength(30); /* truncate if text is too long */
        connect(l, SIGNAL(newData(const CuData&)), this, SLOT(configure(const CuData&)));
        l->setSource(newSrcs.at(i));
        lo->addWidget(l, 0, (i + col) * colSpan, 1, colSpan);
    }

    m_oldSrcs = srcs;
}

void CumbiaClient::unsetSources()
{
    foreach(QuTrendPlot *plot, ui->widget->findChildren<QuTrendPlot *>())
        plot->unsetSources();
    foreach(QuSpectrumPlot *sp, ui->widget->findChildren<QuSpectrumPlot *>())
        sp->unsetSources();
    foreach(QuLabel *l, findChildren<QuLabel *>())
        l->unsetSource();
    m_oldSrcs.clear();
}

void CumbiaClient::switchSources()
{
    m_switchCnt++;
    QString src = QString("test/device/%1/double_spectrum_ro").arg(m_switchCnt % 2 + 1);
    ui->leSrcs->setText(src);
    sourcesChanged();
}

