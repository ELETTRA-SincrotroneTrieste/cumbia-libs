#include "quplotaxescomponent.h"
#include <quplot_base.h>
#include <cumacros.h>

#include <QMap>

/** @private */
class QuPlotAxesComponentPrivate
{
public:
    QMap<int, QuPlotAxesComponent::ScaleMode> scale_mode_map;
    QMap<int, double> margin;
    QMap<int, double> ub_extra;
    QMap<int, QPair<double, double > > default_bounds;
    QMap<int, QPair<double, double > > bounds_from_curves;
};

QuPlotAxesComponent::QuPlotAxesComponent(QuPlotBase *plot)
{
    d = new QuPlotAxesComponentPrivate;
    d->scale_mode_map[QwtPlot::xBottom] = AutoScale;
    d->scale_mode_map[QwtPlot::xTop] = AutoScale;
    d->scale_mode_map[QwtPlot::yLeft] = SemiAutoScale;
    d->scale_mode_map[QwtPlot::yRight] = SemiAutoScale;

    d->margin[QwtPlot::xBottom] = 0.01;
    d->margin[QwtPlot::xTop] = 0.01;
    d->margin[QwtPlot::yLeft] = 0.01;
    d->margin[QwtPlot::yRight] = 0.01;

    d->ub_extra[QwtPlot::xBottom] = 0.0;
    d->ub_extra[QwtPlot::xTop] = 0.0;
    d->ub_extra[QwtPlot::yLeft] = 0.0;
    d->ub_extra[QwtPlot::yRight] = 0.0;

    d->bounds_from_curves[QwtPlot::xBottom].first = 0.0;
    d->bounds_from_curves[QwtPlot::xBottom].second = 1000.0;
    d->bounds_from_curves[QwtPlot::xTop].first = 0.0;
    d->bounds_from_curves[QwtPlot::xTop].second = 1000.0;
    d->bounds_from_curves[QwtPlot::yLeft].first = 0.0;
    d->bounds_from_curves[QwtPlot::yLeft].second = 1000.0;
    d->bounds_from_curves[QwtPlot::yRight].first = 0.0;
    d->bounds_from_curves[QwtPlot::yRight].second = 1000.0;

    /* NOTE: disable QwtPlot axis autoscale */
    plot->setAxisScale(QwtPlot::yLeft, d->bounds_from_curves[QwtPlot::yLeft].first, d->bounds_from_curves[QwtPlot::yLeft].second);
    plot->setAxisScale(QwtPlot::yRight, d->bounds_from_curves[QwtPlot::yRight].first, d->bounds_from_curves[QwtPlot::yRight].second);
    plot->setAxisScale(QwtPlot::xBottom, d->bounds_from_curves[QwtPlot::xBottom].first, d->bounds_from_curves[QwtPlot::xBottom].second);
    plot->setAxisScale(QwtPlot::xTop, d->bounds_from_curves[QwtPlot::xTop].first, d->bounds_from_curves[QwtPlot::xTop].second);

    plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
}

QuPlotAxesComponent::~QuPlotAxesComponent()
{
    delete d;
}

QString QuPlotAxesComponent::name() const
{
    return "axes";
}

void QuPlotAxesComponent::attachToPlot(QuPlotBase *)
{

}

void QuPlotAxesComponent::connectToPlot(QuPlotBase *)
{

}

void QuPlotAxesComponent::setBounds(QuPlotBase *plot, int axisId, double lb, double ub)
{
    plot->setAxisScale(axisId, lb, ub);
}

void QuPlotAxesComponent::setManualBounds(QuPlotBase *plot, int axisId, double lb, double ub)
{
    d->scale_mode_map[axisId] = Manual;
    plot->setAxisScale(axisId, lb, ub);
}

/*
 * \brief Set default values for the axisId that can be restored later.
 *
 * \par Notes
 * \li autoscale is disabled for that axis
 * \li upper and lower bounds are saved for later use
 * \li restoreDefaultBounds can be called later to restore the default values
 */
void QuPlotAxesComponent::setDefaultBounds(QuPlotBase *plot, int axisId, double lb, double ub)
{
    d->default_bounds[axisId].first = lb;
    d->default_bounds[axisId].second = ub;
}

void QuPlotAxesComponent::restoreDefaultBounds(QuPlotBase *plot, int axisId)
{
    d->scale_mode_map[axisId] = SemiAutoScale;
    plot->setAxisScale(axisId, d->default_bounds[axisId].first, d->default_bounds[axisId].second);
}

/** \brief Calls setAxisScale on the plot.
 *
 * Returns true if the axis scale has changed, false otherwise
 */
bool QuPlotAxesComponent::applyScaleFromCurveBounds(QuPlotBase *plot, int axisId)
{

    QPair<double, double> &bounds = d->bounds_from_curves[axisId];
    double lb = plot->axisScaleDiv(axisId).lowerBound();
    double ub = plot->axisScaleDiv(axisId).upperBound();
    double margin;
    if(bounds.second == bounds.first) {
        bounds.second += d->margin[axisId] / 2.0;
        bounds.first -= d->margin[axisId] / 2.0;
    }

    if(bounds.second == bounds.first) {
        bounds.second += 5;
        bounds.first -= 5;
    }
    margin = (bounds.second - bounds.first) *  d->margin[axisId];
    bounds.second += margin/2.0;
    bounds.first -= margin/2.0;
    if(d->ub_extra[axisId] > 0)
    {
        margin = (bounds.second - bounds.first) *  d->ub_extra[axisId];
        if(bounds.second > ub || bounds.first < lb)
        {
            bounds.second += margin;
            plot->setAxisScale(axisId, bounds.first, bounds.second);
            return true;
        }
    }
    else if(bounds.first != lb || bounds.second != ub)
    {
        plot->setAxisScale(axisId, bounds.first, bounds.second);
        return true;
    }
    return false;
}

bool QuPlotAxesComponent::setBoundsFromCurves(double min, double max, int axisId)
{
    if(min <= max) /* values must be well ordered */
    {
        d->bounds_from_curves[axisId].first  = min;
        d->bounds_from_curves[axisId].second = max;
    }
    return min < max;
}

bool QuPlotAxesComponent::getBoundsFromCurves(const QuPlotBase *plot,
                                              double *xmin, double *xmax,
                                              double *ymin, double *ymax,
                                              bool calc_x, bool calc_y) const
{
    *xmin = *xmax = *ymin = *ymax = 0;
    double crvmin, crvmax;
    QList<QwtPlotCurve *> crvs = plot->curves();
    foreach(QwtPlotCurve *c, crvs)
    {
        if(c->dataSize() < 1 || !c->isVisible()) /* it is not possible  to adjust scales if the curves haven't enough data yet. */
            continue;
        if(calc_x) {
            crvmin = c->minXValue();
            crvmax = c->maxXValue();
//            printf("QuPlotAxesComponent::getBoundsFromCurves: X min %s, X max %s\n",
//                   qstoc(QDateTime::fromMSecsSinceEpoch(crvmin).toString()),
//                    qstoc(QDateTime::fromMSecsSinceEpoch(crvmax).toString()));
            if(*xmin == *xmax) {
                *xmin = crvmin;
                *xmax = crvmax;
            }
            else {
                if(crvmin < *xmin)
                    *xmin = crvmin;
                if(crvmax > *xmax)
                    *xmax = crvmax;
            }
        }

        if(calc_y) {
            crvmin = c->minYValue();
            crvmax = c->maxYValue();
            if(*ymin == *ymax) {
                *ymin = crvmin;
                *ymax = crvmax;
            }
            else {
                if(crvmin < *ymin)
                    *ymin = crvmin;
                if(crvmax > *ymax)
                    *ymax = crvmax;
            }
        }

//        printf("QuPlotAxesComponent.getBoundsFromCurves \e[1;32m min max for curve %s calc_x %d calc_y %d mx=%f Mx=%f"
//               " my=%f My=%f\e[0m\n", qstoc(c->title().text()), calc_x, calc_y, *xmin, *xmax, *ymin, *ymax);
    }
    return *xmin <= *xmax && *ymin <= *ymax;
}

bool QuPlotAxesComponent::autoscale(int axisId) const
{
    return d->scale_mode_map[axisId] == AutoScale;
}

QuPlotAxesComponent::ScaleMode QuPlotAxesComponent::scaleMode(int axisId) const
{
    return d->scale_mode_map[axisId];
}

void QuPlotAxesComponent::setAutoscale(int axisId, bool a)
{
    d->scale_mode_map[axisId] = AutoScale;
}

double QuPlotAxesComponent::autoscaleMargin(int axisId) const
{
    return d->margin[axisId];
}

void QuPlotAxesComponent::setAutoscaleMargin(int axisId, double m)
{
    if(m < 0) m = 0;
    d->margin[axisId] = m;
}

double QuPlotAxesComponent::lowerBoundFromCurves(int axisId) const
{
    return d->bounds_from_curves[axisId].first;
}

double QuPlotAxesComponent::upperBoundFromCurves(int axisId) const
{
    return d->bounds_from_curves[axisId].second;
}

double QuPlotAxesComponent::defaultLowerBound(int axisId) const
{
    return d->default_bounds[axisId].first;
}

double QuPlotAxesComponent::defaultUpperBound(int axisId) const
{
    return d->default_bounds[axisId].second;
}

void QuPlotAxesComponent::setUpperBoundExtra(int axisId, double ube)
{
    d->ub_extra[axisId] = ube;
}

double QuPlotAxesComponent::upperBoundExtra(int axisId) const
{
    return d->ub_extra[axisId];
}

