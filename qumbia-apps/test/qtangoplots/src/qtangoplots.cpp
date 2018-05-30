#include "qtangoplots.h"
#include "ui_qtangoplots.h"

#include <QApplication>
#include <QVBoxLayout>
#include <eplotlight_marker.h>
#include <qwt_plot_curve.h>
#include <eplotcurve.h>
#include <qwt_text.h>
#include <math.h>
#include <QTimer>
#include <QDateTime>
#include <macros.h>

QTangoPlots::QTangoPlots(QWidget *parent) :
    QWidget(parent)
{
    printf("\e[1;32;4mOPTIONS:\e[0m %s nPlots curves_per_plot interval [nPoints] (default nPoints=1000)\n"
           "         if nPoints is < 2, a timestamp is generated and a new point is added to the\n"
           "         curves at the specified time interval\n\n", qstoc(qApp->arguments().first()));
    if(qApp->arguments().count() < 4) {
        exit(EXIT_FAILURE);
    }
    ui = new Ui::QTangoPlots;

    m_bufsiz = 3600;
    if(qApp->arguments().count() > 4)
        m_nPoints = qApp->arguments().at(4).toInt();
    else
        m_nPoints = 1000;

    ui->setupUi(this);

    int nplots = qApp->arguments().at(1).toInt();
    int curvesPerPlot = qApp->arguments().at(2).toInt();
    int millis = qApp->arguments().at(3).toInt();

    QVBoxLayout* vblo = new QVBoxLayout(this);

    for(int i = 0; i < nplots; i++) {
        EPlotLightMarker *plot = new EPlotLightMarker(this);
        plot->setXAxisAutoscaleEnabled(true);
        plot->setYAxisAutoscaleEnabled(true);
        plot->setTimeScaleDrawEnabled(m_nPoints < 2);
        vblo->addWidget(plot);
        for(int j = 0; j < curvesPerPlot; j++) {
            QString curvenam = "curve-" + QString::number(j + 1);
            plot->addCurve(curvenam, new EPlotCurve(plot, curvenam));
        }
    }


    updateAll();

    QTimer *timer = new QTimer(this);
    timer->setInterval(millis);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAll()));
    timer->start();

}

QTangoPlots::~QTangoPlots()
{
    delete ui;
}

void QTangoPlots::updateAll()
{
    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());
    foreach(EPlotLightMarker *plot, findChildren<EPlotLightMarker *>()) {
        float sinu = qrand() / (float) RAND_MAX;
        foreach(QwtPlotCurve *c, plot->curves()) {
            EPlotCurve *pc = static_cast<EPlotCurve *>(c);
            if(m_nPoints < 2) {
                double y = qrand() % 1000 * sinu;
                double x = static_cast<double>(QDateTime::currentMSecsSinceEpoch());
                while(pc->count() > m_bufsiz)
                    pc->popFront();
                plot->appendData(c->title().text(), &x, &y, 1);
            }
            else {
                QVector<double> yv, xv;
                for(int i = 0; i < m_nPoints; i++) {
                    xv << i;
                    yv << qrand() % 1000 * sinu;
                }
                plot->setData(c->title().text(), xv, yv);
            }
        }
        plot->refresh();
    }
}
