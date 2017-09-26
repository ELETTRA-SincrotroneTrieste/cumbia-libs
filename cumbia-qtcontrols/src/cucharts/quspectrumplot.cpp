#include "quspectrumplot.h"
#include "callout.h"
#include <cumacros.h>
#include <cudata.h>
#include <QPainter>
#include <QPaintEvent>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QMap>
#include <QtDebug>
#include <QDateTimeAxis>
#include <QDateTime>
#include <QValueAxis>
#include <math.h>

#include "qupalette.h"
#include "quplotcommon.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsreader_abs.h"
#include "quchartconfdialog.h"

class QuSpectrumPlotPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    QuPalette palette;
    Cumbia *cumbia;
    QChart *chart = NULL;
    const CuControlsReaderFactoryI *r_fac;

    QPair<double, double> calc_bounds(const double x0,
                                      const double xN,
                                      const QDateTimeAxis *axis,
                                      const double adjust,
                                      bool force) const;

    QPair<double, double> calc_bounds(const double new_y,
                                      const QValueAxis* a,
                                      const double adjust,
                                      bool force) const;
};

QPair<double, double> QuSpectrumPlotPrivate::calc_bounds(const double x0,
                                                         const double xN,
                                                         const QDateTimeAxis *axis,
                                                         const double adjust,
                                                         bool force) const
{
    double start = x0, end = xN, margin = 0.0;
    margin = fabs(end - start) * adjust / 2;
    if(start < axis->min().toMSecsSinceEpoch() || force)
        start -= margin;
    else
        start = axis->min().toMSecsSinceEpoch();

    if(end > axis->max().toMSecsSinceEpoch() || force)
        end  += margin;
    else
        end = axis->max().toMSecsSinceEpoch();

    qDebug() << __FUNCTION__ << axis <<
                axis->min() <<
                QDateTime::fromMSecsSinceEpoch(start) <<
                QDateTime::fromMSecsSinceEpoch(end) <<
                axis->max() << adjust;

    return QPair<double, double>(start, end);
}

QPair<double, double> QuSpectrumPlotPrivate::calc_bounds(const double new_y,
                                                         const QValueAxis* a,
                                                         const double adjust,
                                                         bool force) const
{
    double min = a->min();
    double max = a->max();
    double margin = 0.0;
    bool changed = (new_y < min || new_y > max || force);
    if(new_y < min || force)
        min = new_y;
    else if(new_y > max || force)
        max = new_y;
    if(changed)
        margin = fabs(max - min) * adjust / 2;
    return QPair<double, double> (min - margin, max + margin);
}

QuSpectrumPlot::QuSpectrumPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac) : QChartView(w)
{
    d = new QuSpectrumPlotPrivate;
    d->chart = new QChart();
    plot_common = new QuPlotCommon(d->chart, true, this);
    d->r_fac = r_fac.clone();
    d->cumbia = cumbia;
    d->auto_configure = true;
    d->read_ok = false;
    d->chart->legend()->show();
    this->setChart(d->chart);
    QValueAxis *xaxis = new QValueAxis(d->chart);
    xaxis->setObjectName("X1");
    xaxis->setRange(0, 1000);
    QFont f = xaxis->labelsFont();
    f.setPointSizeF(f.pointSizeF() * 0.8);
    xaxis->setLabelsFont(f);
    QValueAxis *yaxis = new QValueAxis(d->chart);
    yaxis->setObjectName("Y1");
    d->chart->setAxisX(xaxis);
    d->chart->setAxisY(yaxis);
    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::RectangleRubberBand);
    connect(d->chart, SIGNAL(plotAreaChanged(QRectF)), this, SLOT(plotAreaChanged(QRectF)));
}

QuSpectrumPlot::~QuSpectrumPlot()
{
    delete plot_common;
    foreach(QLineSeries *series, findChildren<QLineSeries *>())
        delete series;
    delete d->chart;
    delete d->r_fac; /* it was cloned */
    delete d;
}

QString QuSpectrumPlot::source() const
{
    return sources().join(";");
}

QStringList QuSpectrumPlot::sources() const
{
    return plot_common->sources();
}

bool QuSpectrumPlot::autoscaleEnabled() const
{
    return plot_common->autoscaleEnabled();
}

double QuSpectrumPlot::autoscaleAdjust() const
{
    return plot_common->autoscaleAdjust();
}

void QuSpectrumPlot::setAutoscale(bool a)
{
    plot_common->setAutoscaleEnabled(a);
    scene()->update();
}

void QuSpectrumPlot::setAutoscaleAdjust(double a)
{
    plot_common->setAutoscaleAdjust(a);
}

void QuSpectrumPlot::moveToAxis(const QString &curve_name, const QString &axis)
{
    plot_common->moveCurveToAxis(curve_name, axis, d->chart);
}

void QuSpectrumPlot::setSource(const QString &s)
{
    QStringList sl = s.split(";", QString::SkipEmptyParts);
    setSources(sl);
}

void QuSpectrumPlot::setSources(const QStringList &l)
{
    plot_common->setSources(l, d->chart, d->r_fac, d->cumbia, this);
}

void QuSpectrumPlot::handleClickedPoint(const QPointF &)
{

}

void QuSpectrumPlot::plotAreaChanged(const QRectF &r)
{
    qDebug() << __FUNCTION__ << "new plot area " << r;
}

void QuSpectrumPlot::mousePressEvent(QMouseEvent *e)
{
    plot_common->mousePressed(e);
    QChartView::mousePressEvent(e);
}

void QuSpectrumPlot::mouseReleaseEvent(QMouseEvent *e)
{
    plot_common->mouseReleased(e, d->chart);
    QChartView::mouseReleaseEvent(e);
}

void QuSpectrumPlot::mouseDoubleClickEvent(QMouseEvent *e)
{
    QChartView::mouseDoubleClickEvent(e);
}

void QuSpectrumPlot::onUpdate(const CuData &da)
{
    QColor background, border;
    QString src = QString::fromStdString(da["src"].toString());
    QLineSeries *ser = d->chart->findChild<QLineSeries *>(src);
    d->read_ok = !da["err"].toBool() && ser != NULL;

    setEnabled(d->read_ok);

    if(da.containsKey("quality_color"))
        background = d->palette[QString::fromStdString(da["quality_color"].toString())];
    if(da.containsKey("success_color"))
        border = d->palette[QString::fromStdString(da["success_color"].toString())];

    if(!d->read_ok)
    {

    }
    else
    {
        if(da.containsKey("value") && da["compound_type"].toString() == "spectrum")
        {
            double min, max;
            std::vector<double> y = da["value"].toDoubleVector();
            QVector<QPointF> points;
            points.reserve(y.size());
            double marker_x;
            bool callout_visible = plot_common->findCalloutX(&marker_x, src);
            for(size_t i = 0; i < y.size(); i++)
            {
                points << QPointF(i, y[i]);
                if(i == marker_x && callout_visible)
                    plot_common->updateCallout(points.last(), ser, d->chart);
            }
            ser->replace(points);

            if(y.size() > 0 && !d->chart->isZoomed() && plot_common->autoscaleEnabled())
            {
                foreach(QAbstractAxis *a, ser->attachedAxes())
                {
                    plot_common->calculateBounds(a, d->chart, &min, &max);
                    a->setRange(min, max);
                }
            }
        }
    }

    //   decorate(background, border);

    setToolTip(da["msg"].toString().c_str());

    emit newData(da);
}

void QuSpectrumPlot::onCalloutClicked(const QString &title, const QPointF &xy)
{
    QuChartConfDialog dialog(0);
    QXYSeries *s = d->chart->findChild<QXYSeries *>(title);
    dialog.addCurve(s);
    connect(&dialog, SIGNAL(curveAxisChanged(QString,QString)), this,
            SLOT(moveToAxis(QString,QString)));
    dialog.exec();
}


