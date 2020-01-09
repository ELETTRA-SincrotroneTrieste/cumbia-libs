#include "cumbiaplots.h"
#include "ui_cumbiaplots.h"

// cumbia-tango
#include <cuserviceprovider.h>
#include <cucontextactionbridge.h>
#include <cumacros.h>
// cumbia-tango

#include <QApplication>
#include <QVBoxLayout>
#include <quplot_base.h>
#include <quplotcurve.h>
#include <quplot_base.h>
#include <qutimescaledraw.h>
#include <qwt_plot_curve.h>
#include <qwt_text.h>
#include <qwt_date_scale_engine.h>
#include <math.h>
#include <QTimer>
#include <QDateTime>

Cumbiaplots::Cumbiaplots(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent)
{
    printf("\e[1;32;4mOPTIONS:\e[0m %s nPlots curves_per_plot interval [nPoints] (default nPoints=1000)\n"
           "         if nPoints is < 2, a timestamp is generated and a new point is added to the\n"
           "         curves at the specified time interval\n\n", qstoc(qApp->arguments().first()));

    if(qApp->arguments().count() < 4) {
        exit(EXIT_FAILURE);
    }

    // cumbia-tango
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);

    ui = new Ui::Cumbiaplots;
    ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
    // cumbia-tango


    int nplots = qApp->arguments().at(1).toInt();
    int curvesPerPlot = qApp->arguments().at(2).toInt();
    int millis = qApp->arguments().at(3).toInt();

    if(qApp->arguments().count() > 4)
        m_nPoints = qApp->arguments().at(4).toInt();
    else
        m_nPoints = 1000;

    QVBoxLayout* vblo = new QVBoxLayout(this);

    for(int i = 0; i < nplots; i++) {
        QuPlotBase *plot = new QuPlotBase(this);
        plot->setXAxisAutoscaleEnabled(true);
        plot->setYAxisAutoscaleEnabled(true);
        if(m_nPoints < 2) {
            plot->setAxisScaleDraw(QwtPlot::xBottom, new QuTimeScaleDraw());
            plot->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
            plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtDateScaleEngine(Qt::LocalTime));
            plot->setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );
            plot->setDataBufferSize(3600);
            plot->setUpperBoundExtra(QwtPlot::xBottom, 0.02);
        }
        vblo->addWidget(plot);
        for(int j = 0; j < curvesPerPlot; j++) {
            QString curvenam = "curve-" + QString::number(j + 1);
            plot->addCurve(curvenam);
        }
    }


    updateAll();

    QTimer *timer = new QTimer(this);
    timer->setInterval(millis);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAll()));
    timer->start();
}

Cumbiaplots::~Cumbiaplots()
{
    delete ui;
}

void Cumbiaplots::updateAll()
{
    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());
    foreach(QuPlotBase *plot, findChildren<QuPlotBase *>()) {
        float sinu = qrand() / (float) RAND_MAX;
        foreach(QwtPlotCurve *c, plot->curves()) {
            if(m_nPoints < 2) {
                QuPlotCurve *pc = static_cast<QuPlotCurve * >(c);
                double x = static_cast<double>(QDateTime::currentMSecsSinceEpoch());
                double y = qrand() % 1000 * sinu;
                while(pc->count() > plot->dataBufferSize())
                    pc->popFront();
                pc->appendData(&x, &y, 1);
                pc->updateRawData();
            }
            else {
                QVector<double> yv, xv;
                for(int i = 0; i < m_nPoints; i++) {
                    xv << i;
                    yv << qrand() % 1000 * sinu;
                    // calling plot->setData would cause a refresh every time data is set on a curve
                    // instead, call plot->refresh later as we do with the qtango example, where
                    // plot->refresh is called after data is set on the curves
                    static_cast<QuPlotCurve * >(c)->setData(xv, yv);
                }
            }
        }
        plot->refresh();
    }
}
