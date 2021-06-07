#include "cuepwidgets.h"
#include "ui_cutcwidgets.h"

#include <cumbia.h>
#include <cumacros.h>
#include <cuthreadfactoryimpl.h>
#include <cumbiaepics.h>
#include <qthreadseventbridgefactory.h>
#include <qulabel.h>
#include <qubutton.h>
#include <qutrendplot.h>
#include <quspectrumplot.h>
#include <qulineedit.h>
#include <cuepcontrolsreader.h>
#include <QGridLayout>
#include <cuserviceprovider.h>
#include <qulogimpl.h>
#include <QtDebug>
#include <QMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include <cuepreadoptions.h>
#include <queplotupdatestrategy.h>
#include <QTimer>


CuEpWidgets::CuEpWidgets(CumbiaEpics *cuep, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CuTCWidgets),
    m_layoutColumnCount(10)
{
    // for valgrind test
    // int *f= new int[1000];

    m_switchCnt = 0;
    cu_ep = cuep;
    m_log = new CuLog(&m_log_impl);
    cu_ep->getServiceProvider()->registerService(CuServices::Log, m_log);

    ui->setupUi(this);
    connect(ui->pbSetSources, SIGNAL(clicked()), this, SLOT(sourcesChanged()));
    connect(ui->pbApplyRefresh, SIGNAL(clicked()), this, SLOT(changeRefresh()));
    connect(ui->pbUnsetSrc, SIGNAL(clicked()), this, SLOT(unsetSources()));

    if(qApp->arguments().count() >= 2)
    {
        for(int i = 1; i < qApp->arguments().count(); i++)
        {
            if(i > 1 && i < qApp->arguments().count() - 1)
                ui->leSrcs->setText(ui->leSrcs->text() + ",");
            ui->leSrcs->setText(ui->leSrcs->text() + qApp->arguments().at(i));
        }
       sourcesChanged();
    }

    resize(1000, 600);

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(switchSources()));
    t->setInterval(1000);
   // t->start();
}

CuEpWidgets::~CuEpWidgets()
{
    delete ui;
}

void CuEpWidgets::configure(const CuData &d)
{
    printf("\e[1;32mCuEpWidgets::configure enter %s\e[0m\n", d.toString().c_str());
    if(d["type"].toString() != "connection")
        return;

    const int plotRowCnt = 5;
    int layout_row = 2;
    int format = d["df"].toInt();

    QGridLayout *lo = qobject_cast<QGridLayout *>(ui->widget->layout());
    QStringList srcs = ui->leSrcs->text().split(",", QString::SkipEmptyParts);
    QuEpicsPlotUpdateStrategy *plot_upd_strategy = new QuEpicsPlotUpdateStrategy(true);
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

//    if(format == 0)
//    {
        QuTrendPlot *plot = new QuTrendPlot(this, cu_ep, CuEpReaderFactory());
        lo->addWidget(plot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
        plot->setUpdateStrategy(plot_upd_strategy);
      //  plot->setContextMenuStrategy(ctx_menu_strat);
        plot->setSources(srcs);
//    }
//    else if(format == 1 && !realtime)
//    {
//        QuSpectrumPlot *splot = new QuSpectrumPlot(this, cu_ep, CuEpReaderFactory());
//        lo->addWidget(splot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
//        splot->setUpdateStrategy(plot_upd_strategy);
//        splot->setContextMenuStrategy(ctx_menu_strat);
//        splot->setSources(srcs);
//    }
}

void CuEpWidgets::changeRefresh()
{
    int period = ui->sbPeriod->value();
    int refmode = ui->cbRefMode->currentIndex();
    CuData options;
    options["period"] = period;
    options["refresh_mode"] = refmode;
   // QuTrendPlot *tp = findChild<QuTrendPlot *>();
   // if(tp)
    //    tp->sendData(options);
    //QuSpectrumPlot *sp = findChild<QuSpectrumPlot *>();
    //if(sp)
     ///   sp->sendData(options);
}

void CuEpWidgets::sourcesChanged()
{
    /* clear widgets */
    QGridLayout *lo = NULL;
    if(!ui->widget->layout())
        lo = new QGridLayout(ui->widget);
    else
        lo = qobject_cast<QGridLayout *>(ui->widget->layout());

  //  unsetSources();

    foreach(QuTrendPlot *plot, ui->widget->findChildren<QuTrendPlot *>())
        delete plot;
    foreach(QuSpectrumPlot *plot, ui->widget->findChildren<QuSpectrumPlot *>())
        delete plot;
    foreach(QuLabel *l, ui->widget->findChildren<QuLabel *>())
        delete l;

    QStringList srcs = ui->leSrcs->text().split(",", QString::SkipEmptyParts);
    const int srcCnt = srcs.size();
    const int colSpan = m_layoutColumnCount / srcCnt;
    ui->labelTitle->setText(ui->leSrcs->text());

    for(int i = 0; i < srcCnt; i++)
    {
        CuEpReaderFactory rf;

        QuLabel *l = new QuLabel(this, cu_ep, rf);
        l->setMaximumLength(30); /* truncate if text is too long */
        if(i == 0) /* configure only once! */
            connect(l, SIGNAL(newData(const CuData&)), this, SLOT(configure(const CuData&)));

        EpSource eps(srcs.at(i).toStdString());
        printf("setting source \"%s\"\n", eps.toString().c_str());

        l->setSource(srcs.at(i));
        lo->addWidget(l, 0, (i) * colSpan, 1, colSpan);
    }
}

void CuEpWidgets::unsetSources()
{
    foreach(QuTrendPlot *plot, ui->widget->findChildren<QuTrendPlot *>())
         plot->unsetSources();
    foreach(QuSpectrumPlot *sp, ui->widget->findChildren<QuSpectrumPlot *>())
        sp->unsetSources();
    foreach(QuLabel *l, findChildren<QuLabel *>())
        l->unsetSource();
}

void CuEpWidgets::switchSources()
{
    m_switchCnt++;
    QString src = QString("test/device/%1/double_spectrum_ro").arg(m_switchCnt % 2 + 1);
    ui->leSrcs->setText(src);
    sourcesChanged();
}

