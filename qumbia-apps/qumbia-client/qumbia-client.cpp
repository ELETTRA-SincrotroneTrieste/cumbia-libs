#include "qumbia-client.h"
#include "ui_qumbia-client.h"
#include "writer.h"
#include <cumbiapool.h>
#include <cumacros.h>

#ifdef CUMBIA_WEBSOCKET_VERSION
#include <cuwsregisterengine.h>
#endif

#ifdef CUMBIA_HTTP_VERSION
#include <cuhttpregisterengine.h>
#endif

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cutangoregisterengine.h>
#include <cutreader.h>
#endif

#ifdef QUMBIA_EPICS_CONTROLS_VERSION
#include <cuepregisterengine.h>
#endif

#ifdef CUMBIA_RANDOM_VERSION
#include <curndregisterengine.h>
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
#include <QCommandLineParser>
#include <QComboBox>
#include <cucontext.h>
#include <QTimer>

#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <quapps.h>
#include <cuengineaccessor.h>

QumbiaClient::QumbiaClient(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QumbiaClient),
    m_layoutColumnCount(10)
{
    QStringList engines;
    cu_pool = cumbia_pool;
    m_switchCnt = 0;

    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    ui->setupUi(this);
    new CuEngineAccessor(this, &cu_pool, &m_ctrl_factory_pool);
    connect(ui->pbSetSources, SIGNAL(clicked()), this, SLOT(sourcesChanged()));
    connect(ui->pbApplyRefresh, SIGNAL(clicked()), this, SLOT(changeRefresh()));
    connect(ui->pbUnsetSrc, SIGNAL(clicked()), this, SLOT(unsetSources()));

    ui->leSrcs->setPlaceholderText("Type a space separated list of sources");

    /// TEST
    ui->leSrcs->setText("test/device/1/double_scalar");

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
    if(d[TTT::Type].toString() == "property") {  // d["type"]
        sender()->disconnect(this, SLOT(configure(CuData)));
        const int plotRowCnt = 5;
        int layout_row = 2;
        std::string format = d[TTT::DataFormatStr].toString();  // d["dfs"]

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
            plot->addSource(d[TTT::Src].toString().c_str());  // d["src"]
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
            splot->addSource(d[TTT::Src].toString().c_str());  // d["src"]

            data_dim = d[TTT::DimX].toLongInt();  // d["dim_x"]
            if(data_dim <= 0)
                data_dim = d[TTT::Value].getSize();  // d["value"]
            if(data_dim <= 0 && d.containsKey(TTT::MaxDimX))  // d.containsKey("max_dim_x")
                data_dim = d[TTT::MaxDimX].toULongInt();  // d["max_dim_x"]
        }

        if(d["writable"].toInt() > 0)
        {
            QWidget *wi = ui->gbWriters->findChild<QScrollArea *>()->widget();
            ui->pbWrite->setChecked(true);
            Writer *w = new Writer(wi, cu_pool, m_ctrl_factory_pool, data_dim, QString::fromStdString(d[TTT::Src].toString()));  // d["src"]
            qobject_cast<QVBoxLayout *>(wi->layout())->addWidget(w);
        }
    }
}

void QumbiaClient::changeRefresh()
{
    int period = ui->sbPeriod->value();
    int refmode = 0;
#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    switch(ui->cbRefMode->currentIndex()) {
    case 0:
        refmode = CuTReader::PolledRefresh;
        break;
    case 1:
    default:
        refmode = CuTReader::ChangeEventRefresh;
        break;
    }
#else
    printf("QumbiaClient.changeRefresh: warning: Tango module is not supported: this method may fail\n");
#endif

    CuData options;
    options[TTT::Period] = period;  // options["period"]
    options[TTT::RefreshMode] = refmode;  // options["refresh_mode"]
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
    printf("\e[1;33m%s entering\e[0m\n", __PRETTY_FUNCTION__);
    /* clear widgets */
    QGridLayout *lo = NULL;
    if(!ui->widget->layout())
        lo = new QGridLayout(ui->widget);
    else
        lo = qobject_cast<QGridLayout *>(ui->widget->layout());

    int period = ui->sbPeriod->value();
    int refmode = ui->cbRefMode->currentIndex() == 0 ? CuTReader::PolledRefresh : CuTReader::ChangeEventRefresh;
    CuData options;
    options[TTT::Period] = period;  // options["period"]
    options[TTT::RefreshMode] = refmode;  // options["refresh_mode"]

    QStringList srcs = ui->leSrcs->text().split(QRegularExpression("\\s+"));
    const int srcCnt = srcs.size() > 0 ? srcs.size() : 1;
    const int colSpan = m_layoutColumnCount / srcCnt;

    if(m_oldSrcs.size() == 0) {
        foreach(QuLabel *l, ui->widget->findChildren<QuLabel *>()) {
            printf("\e[1;31m%s deleting qulabel (1) %p\e[0m\n", __PRETTY_FUNCTION__, l);
            delete l;
        }
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
        if(!tr_plot->sources().count())  { {
                printf("\e[1;31m%s deleting plot %p\e[0m\n", __PRETTY_FUNCTION__, tr_plot);
                delete tr_plot;
            }
        }
    }

    QuSpectrumPlot *sp_plot = ui->widget->findChild<QuSpectrumPlot *>();
    if(sp_plot)
    {
        QStringList psrcs = sp_plot->sources();
        foreach(QString s, remSrcs) {
            if(psrcs.contains(s))
                sp_plot->unsetSource(s);
        }
        if(!sp_plot->sources().count()) {
            printf("\e[1;31m%s deleting plot %p\e[0m\n", __PRETTY_FUNCTION__, sp_plot);
            delete sp_plot;
        }
    }

    foreach(QuLabel *l, ui->widget->findChildren<QuLabel *>())
        if(remSrcs.contains(l->source())) {
            printf("%s deleting qulabel (2) %p\n", __PRETTY_FUNCTION__, l);
            delete l;
        }
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
        printf("QumbiaClient.sourcesChanged: \e[1;32mset source %s\n", qstoc(newSrcs.at(i)));
        l->setSource(newSrcs.at(i));
        lo->addWidget(l, 0, (i + col) * colSpan, 1, colSpan);
    }

    m_oldSrcs = srcs;
}

void QumbiaClient::unsetSources()
{
    printf("%s \n", __PRETTY_FUNCTION__);
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

