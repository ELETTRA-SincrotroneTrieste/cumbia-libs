#include "cutcwidgets.h"
#include "ui_cutcwidgets.h"

#include <cumbiatango.h>
#include <tango.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <qulabel.h>
#include <qubutton.h>
#include <qutrendplot.h>
#include <quspectrumplot.h>
#include <qutrealtimeplot.h>
#include <qulineedit.h>
#include <QGridLayout>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cuserviceprovider.h>
#include <qutplotupdatestrategy.h>
#include <qutplotcontextmenustrategy.h>
#include <qulogimpl.h>
#include <QtDebug>
#include <QMessageBox>
#include <QSpinBox>
#include <QComboBox>
#include <cutangoopt_builder.h>
#include <QTimer>


CuTCWidgets::CuTCWidgets(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CuTCWidgets),
    m_layoutColumnCount(10)
{
    // for valgrind test
    int *f= new int[1000];

    m_switchCnt = 0;
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);

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

CuTCWidgets::~CuTCWidgets()
{
    delete ui;
}

void CuTCWidgets::configure(const CuData &d)
{
    if(d["type"].toString() != "property") /* "property" for attributes and commands */
        return;

    const int plotRowCnt = 5;
    int layout_row = 2;
    int format = d["data_format"].toInt();

    QGridLayout *lo = qobject_cast<QGridLayout *>(ui->widget->layout());
    QStringList srcs = ui->leSrcs->text().split(",", QString::SkipEmptyParts);
    for(int i = 0; i < srcs.size() && d["writable"].toInt() == Tango::READ_WRITE; i++)
    {
        QuButton *b = new QuButton(this, cu_t, CuTWriterFactory());
        b->setText("Write");
        QString target = srcs.at(i) + "(";
        if(format == 1) /* Tango::SPECTRUM */
        {
            /* put m_layoutColumnCount - 1 line edits */
            for(int j = 0; j < m_layoutColumnCount - 1; j++)
            {
                QuLineEdit *le = new QuLineEdit(this);
                le->setObjectName(QString("leInputArgs_%1").arg(j + (i * m_layoutColumnCount)));
                lo->addWidget(le, layout_row, j, 1, 1);
                target += "&" + le->objectName() + ",";
            }
            lo->addWidget(b, layout_row, m_layoutColumnCount -1, 1, 1);
        }
        else
        {
            QuLineEdit *le = new QuLineEdit(this);
            le->setObjectName("leInputArgs");
            lo->addWidget(le, layout_row, 0, 1, m_layoutColumnCount - 1);
            target += "&" + le->objectName();
            lo->addWidget(b, layout_row, m_layoutColumnCount - 1, 1, 1);
        }
        target += ")";
        //    b->setTargets(target);
        b->setToolTip(b->targets());
        layout_row++;
    }


    int rtsrc = 0;
    foreach(QString s, srcs) /* a/b/c->GetThis(a,b,c) */
        if(s.contains(QRegExp("\\-\\>[A-Za-z_@\\-+\\.,]+\\(.*\\)")))
            rtsrc++;
    bool realtime = (rtsrc == srcs.size());

    printf("\e[1;34mCuTWidgets: configure: format %d realtime %d"
           " - %d %d\e[0m\n", format, realtime, rtsrc, srcs.size());

    QuTPlotContextMenuStrategy *ctx_menu_strat = new QuTPlotContextMenuStrategy(realtime);

    if(format == 0)
    {
        QuTrendPlot *plot = new QuTrendPlot(this, cu_t, CuTReaderFactory());
        lo->addWidget(plot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
        plot->setContextMenuStrategy(ctx_menu_strat);
        plot->setSources(srcs);
    }
    else if(format == 1 && !realtime)
    {
        QuSpectrumPlot *splot = new QuSpectrumPlot(this, cu_t, CuTReaderFactory());
        lo->addWidget(splot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
        splot->setContextMenuStrategy(ctx_menu_strat);
        splot->setSources(srcs);
    }
    else if(format == 1 && realtime)
    {
        QuTRealtimePlot *rtplot = new QuTRealtimePlot(this, cu_t, CuTReaderFactory());
        lo->addWidget(rtplot, layout_row, 0, plotRowCnt, m_layoutColumnCount);
        rtplot->setContextMenuStrategy(ctx_menu_strat);
        rtplot->setSources(srcs);
    }
}

void CuTCWidgets::changeRefresh()
{
    int period = ui->sbPeriod->value();
    int refmode = ui->cbRefMode->currentIndex();
    CuData options;
    options["period"] = period;
    options["refresh_mode"] = refmode;
    QuTrendPlot *tp = findChild<QuTrendPlot *>();
    if(tp)
        tp->sendData(options);
    QuSpectrumPlot *sp = findChild<QuSpectrumPlot *>();
    if(sp)
        sp->sendData(options);
}

void CuTCWidgets::sourcesChanged()
{
    /* clear widgets */
    QGridLayout *lo = NULL;
    if(!ui->widget->layout())
        lo = new QGridLayout(ui->widget);
    else
        lo = qobject_cast<QGridLayout *>(ui->widget->layout());

    QStringList srcs = ui->leSrcs->text().split(",", QString::SkipEmptyParts);
    if(srcs == m_oldSrcs)
        return;

//    QStringList newSrcs;
//    foreach(QString s, srcs)
//        if(!m_oldSrcs.contains(s))
//            newSrcs << s;

//    QStringList remSrcs;
//    foreach(QString s, m_oldSrcs)
//        if(!srcs.contains(s))
//            remSrcs << s;

    foreach(QuTrendPlot *plot, ui->widget->findChildren<QuTrendPlot *>())
        delete plot;
    foreach(QuSpectrumPlot *plot, ui->widget->findChildren<QuSpectrumPlot *>())
        delete plot;
    foreach(QuLabel *l, ui->widget->findChildren<QuLabel *>())
            delete l;

    const int srcCnt = srcs.size();
    const int colSpan = m_layoutColumnCount / srcCnt;
    ui->labelTitle->setText(ui->leSrcs->text());

    for(int i = 0; i < srcCnt; i++)
    {
        CuTReaderFactory rf;
        CuTangoOptBuilder obu;
        obu.setPeriod(1000).setRefreshMode(CuTReader::PolledRefresh);
        QuLabel *l = new QuLabel(this, cu_t, rf);
        l->setOptions(obu.options());
        l->setMaximumLength(30); /* truncate if text is too long */
        if(i == 0) /* configure only once! */
            connect(l, SIGNAL(newData(const CuData&)), this, SLOT(configure(const CuData&)));
        l->setSource(srcs.at(i));
        lo->addWidget(l, 0, (i) * colSpan, 1, colSpan);
    }
}

void CuTCWidgets::unsetSources()
{
    foreach(QuTrendPlot *plot, ui->widget->findChildren<QuTrendPlot *>())
         plot->unsetSources();
    foreach(QuSpectrumPlot *sp, ui->widget->findChildren<QuSpectrumPlot *>())
        sp->unsetSources();
    foreach(QuLabel *l, findChildren<QuLabel *>())
        l->unsetSource();
}

void CuTCWidgets::switchSources()
{
    m_switchCnt++;
    QString src = QString("test/device/%1/double_spectrum_ro").arg(m_switchCnt % 2 + 1);
    ui->leSrcs->setText(src);
    sourcesChanged();
}

