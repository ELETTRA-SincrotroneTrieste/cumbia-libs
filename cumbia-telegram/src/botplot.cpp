#include "botplot.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <cudata.h>
#include <QDir>
#include <cuvariant.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_grid.h>
#include <QTime>
#include <QBuffer>
#include <QtDebug>

BotPlot::BotPlot()
{
}

QByteArray BotPlot::drawPlot(const QString &src, const std::vector<double> &ve)
{
    QTime t;
    t.start();
    error_message = QString();

    QwtPlot plot(src);
    plot.setCanvasBackground(QColor(Qt::white));
    QwtPlotGrid grid;
    grid.setPen(QColor(Qt::lightGray), 0.0, Qt::DashLine);
    grid.attach(&plot);
    QwtPlotCurve c(src);
    c.setPen(QColor(Qt::green), 0.0);
    QVector<double> x, y;
    for(size_t i = 0; i < ve.size(); i++) {
        x << i;
        y << ve[i];
    }
    c.setSamples(x, y);
    c.attach(&plot);
    plot.setAxisAutoScale(QwtPlot::yLeft, true);
    plot.setAxisScale(QwtPlot::xBottom, 0, ve.size() -1);
    plot.replot();

    QImage img(1024, 768, QImage::Format_RGB32);
    QwtPlotRenderer renderer;
    renderer.renderTo(&plot, img);
    QByteArray ba;
    QBuffer bu(&ba);
    bu.open(QIODevice::WriteOnly);
    img.save(&bu, "PNG");

    qDebug() << __PRETTY_FUNCTION__ << "generated plot " << bu.data().count()<< "bytes in"   << t.elapsed() << "ms";
    return bu.data();
}
