#include "qutrendplot.h"
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

class QuTrendPlotPrivate
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

QPair<double, double> QuTrendPlotPrivate::calc_bounds(const double x0,
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

QPair<double, double> QuTrendPlotPrivate::calc_bounds(const double new_y,
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

QuTrendPlot::QuTrendPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac) : QChartView(w)
{
    d = new QuTrendPlotPrivate;

    d->chart = new QChart();
    plot_common = new QuPlotCommon(d->chart, true, this);

    d->r_fac = r_fac.clone();
    d->cumbia = cumbia;
    d->auto_configure = true;
    d->read_ok = false;
    d->chart->legend()->show();
    this->setChart(d->chart);

    QDateTimeAxis *xaxis = new QDateTimeAxis(d->chart);
    xaxis->setObjectName("X1");
    xaxis->setFormat("MM.dd hh:mm:ss");
    xaxis->setLabelsAngle(60);
    xaxis->setRange(QDateTime::currentDateTime().addSecs(-3), QDateTime::currentDateTime());
    xaxis->setTickCount(-1);
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

QuTrendPlot::~QuTrendPlot()
{
    delete plot_common;
    foreach(QLineSeries *series, findChildren<QLineSeries *>())
        delete series;
    delete d->chart;
    delete d->r_fac; /* it was cloned */
    delete d;
}

QString QuTrendPlot::source() const
{
   return sources().join(";");
}

QStringList QuTrendPlot::sources() const
{
    return plot_common->sources();
}

bool QuTrendPlot::autoscaleEnabled() const
{
    return plot_common->autoscaleEnabled();
}

double QuTrendPlot::autoscaleAdjust() const
{
    return plot_common->autoscaleAdjust();
}

void QuTrendPlot::setAutoscaleEnabled(bool a)
{
    plot_common->setAutoscaleEnabled(a);
    scene()->update();
}

void QuTrendPlot::setAutoscaleAdjust(double a)
{
    plot_common->setAutoscaleAdjust(a);
}

void QuTrendPlot::moveToAxis(const QString &curve_name, const QString &axis)
{
    plot_common->moveCurveToAxis(curve_name, axis, d->chart);
}

void QuTrendPlot::setSource(const QString &s)
{
    QStringList sl = s.split(";", QString::SkipEmptyParts);
    setSources(sl);
}

void QuTrendPlot::setSources(const QStringList &l)
{
    plot_common->setSources(l, d->chart, d->r_fac, d->cumbia, this);
}

void QuTrendPlot::handleClickedPoint(const QPointF &)
{

}

void QuTrendPlot::plotAreaChanged(const QRectF &r)
{
    qDebug() << __FUNCTION__ << "new plot area " << r;
}

void QuTrendPlot::mousePressEvent(QMouseEvent *e)
{
    plot_common->mousePressed(e);
    QChartView::mousePressEvent(e);
}

void QuTrendPlot::mouseReleaseEvent(QMouseEvent *e)
{
    plot_common->mouseReleased(e, d->chart);
    QChartView::mouseReleaseEvent(e);
}

void QuTrendPlot::mouseDoubleClickEvent(QMouseEvent *e)
{
    QChartView::mouseDoubleClickEvent(e);
}

void QuTrendPlot::onUpdate(const CuData &da)
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
        /* data could possibly contain a piece of the value's history */
        if(da.containsKey("history_y") && da.containsKey("history_x"))
        {
            double min, max;
            CuVariant y = da["history_y"];
            CuVariant x = da["history_x"];
            if(x.getSize() == y.getSize() &&  x.getFormat() == y.getFormat() &&
                    x.getType() == y.getType() && x.getFormat() == CuVariant::Vector)
            {
                std::vector<double> v = y.toDoubleVector();
                std::vector<double> xtime = x.toDoubleVector();
                for(size_t i = 0; i < v.size(); i++)
                    ser->append(xtime[i], v[i]);
                /* recalculate bounds */
                if(v.size() > 0 && !d->chart->isZoomed() && plot_common->autoscaleEnabled())
                {
                    foreach(QAbstractAxis *a, ser->attachedAxes())
                    {
                        plot_common->calculateBounds(a, d->chart, &min, &max);
                        a->setRange(min, max);
                    }
                }
            }
        }

        if(da.containsKey("value"))
        {
            double t0 = -1;
            qint64 tN = static_cast<qint64>(da["timestamp_ms"].toLongInt());
            double y = da["value"].toDouble();

            if(!d->chart->isZoomed() && plot_common->autoscaleEnabled())
            {
                QValueAxis *yaxis = NULL;
                QDateTimeAxis *xaxis = NULL;
                foreach(QAbstractAxis *aa, ser->attachedAxes())
                {
                    if(aa->type() == QAbstractAxis::AxisTypeValue && aa->orientation() == Qt::Vertical)
                        yaxis = qobject_cast<QValueAxis *>(aa);
                    else if(aa->type() == QAbstractAxis::AxisTypeDateTime && aa->orientation() == Qt::Horizontal)
                        xaxis = qobject_cast<QDateTimeAxis *>(aa);
                }

                if(ser->count() > 0) /* provide a fictitious start */
                    t0 = ser->at(0).x();
                else
                    t0 = QDateTime::currentDateTime().toMSecsSinceEpoch() - 2000;

                bool force_calc = (ser->count() == 0);
                QPair<double, double> xrange = d->calc_bounds(t0, tN, xaxis, plot_common->autoscaleAdjust(), force_calc);
                xaxis->setRange(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(xrange.first)),
                                QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(xrange.second)));
                QPair<double, double> yrange = d->calc_bounds(y, yaxis, plot_common->autoscaleAdjust(), force_calc);
                yaxis->setRange(yrange.first, yrange.second);
            }

            if(plot_common->callout()->isVisibleFor(src))
                plot_common->updateCallout(plot_common->callout()->anchor(), ser, d->chart);

            ser->append(tN, y);
            qDebug() << __FUNCTION__ << "appended " << tN << ser->at(0) << da["value"].toDouble()
                     << ser->count() << "isZoomed" << d->chart->isZoomed()  << "autoscale" << plot_common->autoscaleEnabled();
        }
    }

    //   decorate(background, border);

    setToolTip(da["msg"].toString().c_str());

    emit newData(da);
}

void QuTrendPlot::onCalloutClicked(const QString &title, const QPointF &)
{
    QuChartConfDialog dialog(0);
    QXYSeries *s = d->chart->findChild<QXYSeries *>(title);
    dialog.addCurve(s);
    connect(&dialog, SIGNAL(curveAxisChanged(QString,QString)), this,
            SLOT(moveToAxis(QString,QString)));
    dialog.exec();
}


