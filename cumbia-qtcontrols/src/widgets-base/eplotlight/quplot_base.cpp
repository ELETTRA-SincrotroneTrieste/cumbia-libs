#include "quplot_base.h"
#include "quplotcurve.h"

#include <cumacros.h>
#include <cudata.h>
#include <elettracolors.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <sys/time.h>
#include <QDateTime>
#include <QPainter>
#include <QMouseEvent>
#include <QtDebug>
#include <QTimer>
#include <qwt_plot_marker.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>
#include <qwt_picker_machine.h>
#include <qupalette.h>


// plot components
#include "quplotzoomcomponent.h"
#include "quplotmarkercomponent.h"
#include "quplotcanvaspaintercomponent.h"
#include "quplotaxescomponent.h"
#include "quplotcontextmenucomponent.h"
#include "eplot_configuration_widget.h"
#include "quwidgetcontextmenustrategyi.h"
#include "quwidgetupdatestrategy_i.h"

#ifndef __FreeBSD__
#include <values.h>
#endif

using namespace std; /* std::isnan issue across different compilers/compiling standards */


class QuPlotBasePrivate
{
public:
    QuPlotBase::CurveStyle curvesStyle;
    QuWidgetUpdateStrategyI *updateStrategy;
    QuWidgetContextMenuStrategyI* ctxMenuStrategy;
    bool titleOnCanvasEnabled, displayZoomHint;
    QMap<QString, QuPlotCurve*> curvesMap;
    int bufSiz;
    int refresh_timeo;
    /* plot components */
    QMap<QString, QuPlotComponent *> components_map;
    // for marker

    QwtPlotPicker* picker;
};

bool ShiftClickEater::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if(me->modifiers() == Qt::ShiftModifier)
            return true;
    }
    return QObject::eventFilter(obj, event);
}

QuPlotBase::QuPlotBase(QWidget *parent) : QwtPlot(parent)
{
    init();
}

QuPlotBase::QuPlotBase(const QwtText &title, QWidget *parent) : QwtPlot(title, parent)
{
    init();
}

QuPlotBase::~QuPlotBase()
{
    if(d->updateStrategy)
        delete d->updateStrategy;
    if(d->ctxMenuStrategy)
        delete d->ctxMenuStrategy;

    foreach(QuPlotComponent *c, d->components_map.values())
        delete c;
    d->components_map.clear();
    delete d;
}

/* QwtPlot xAxis autoscale is disabled and axis autoscale is managed internally 
 * through the refresh() method.
 */
void QuPlotBase::init()
{
    d = new QuPlotBasePrivate;
    d->bufSiz = -1;
    d->refresh_timeo = -1;
    d->updateStrategy = NULL;
    d->ctxMenuStrategy = NULL;
    d->titleOnCanvasEnabled = false;
    d->displayZoomHint = false;
    d->curvesStyle = Lines;
    setFrameStyle(QFrame::NoFrame);
    /* white background */
    setCanvasBackground(Qt::white);
    /* disable qwt auto replot */
    setAutoReplot(false);
    /* grid */
    QwtPlotGrid* plotgrid = new QwtPlotGrid;
    plotgrid->setPen(QPen(QColor(230,230,248)));
    plotgrid->attach(this);

    setStyleSheet("QuPlotBase { background:white; border:2px solid LightGrey; "
                  "padding:2px; border-radius:4px} QwtPlotCanvas {border:1px solid LightGray;}");

    QuPlotZoomComponent *zoom_c = new QuPlotZoomComponent(this);
    zoom_c->attachToPlot(this);
    zoom_c->connectToPlot(this);
    d->components_map.insert(zoom_c->name(), zoom_c);

    QuPlotMarkerComponent *marker_c = new QuPlotMarkerComponent(this);
    marker_c->attachToPlot(this);
    marker_c->connectToPlot(this);
    d->components_map.insert(marker_c->name(), marker_c);

    /* draws canvas. No need to attach or connect */
    QuPlotCanvasPainterComponent *painter_c = new QuPlotCanvasPainterComponent();
    d->components_map.insert(painter_c->name(), painter_c);

    /* manages axes */
    QuPlotAxesComponent *axes_c = new QuPlotAxesComponent(this);
    d->components_map.insert(axes_c->name(), axes_c);

    QuPlotContextMenuComponent *ctx_menu = new QuPlotContextMenuComponent();
    ctx_menu->attachToPlot(this);
    ctx_menu->connectToPlot(this);
    d->components_map.insert(ctx_menu->name(), ctx_menu);

    QwtPlot::replot(); /* do not need QuPlotBase::replot() here */
}

/*! \brief sets or replaces the update strategy (see the strategy design pattern)
 *
 * Set or replace the current QuWidgetUpdateStrategyI implementation.
 *
 * @param updateStrategy an implementation of QuWidgetUpdateStrategyI
 *
 * If a strategy is already in use, it is deleted and the new one is
 * set.
 */
void QuPlotBase::setUpdateStrategy(QuWidgetUpdateStrategyI *updateStrategy)
{
    if(d->updateStrategy)
        delete d->updateStrategy;
    d->updateStrategy = updateStrategy;
}

/*! \brief sets or replaces the context menu strategy (see the strategy design pattern)
 *
 * Set or replace the current QuWidgetContextMenuStrategyI implementation.
 *
 * @param updateStrategy an implementation of QuWidgetContextMenuStrategyI
 *
 * If a strategy is already in use, it is deleted and the new one is
 * set.
 */
void QuPlotBase::setContextMenuStrategy(QuWidgetContextMenuStrategyI *ctx_menu_strategy)
{
    if(d->ctxMenuStrategy)
        delete d->ctxMenuStrategy;
    d->ctxMenuStrategy = ctx_menu_strategy;
}

/*! \brief return the QuWidgetUpdateStrategyI implementation in use.
 *
 * @return QuWidgetUpdateStrategyI set with setUpdateStrategy
 */
QuWidgetUpdateStrategyI *QuPlotBase::updateStrategy() const
{
    return d->updateStrategy;
}

/*! \brief return the QuWidgetContextMenuStrategyI implementation in use.
 *
 * @return QuWidgetContextMenuStrategyI set with setContextMenuStrategy
 */
QuWidgetContextMenuStrategyI *QuPlotBase::contextMenuStrategy() const
{
    return d->ctxMenuStrategy;
}

void QuPlotBase::update(const CuData &)
{

}

/*! configure upper and lower bounds according to data set in the input CuData
 *
 * @param da a CuData with suggested minimum and maximum values to automatically
 * adjust upper and lower bounds
 *
 * \note
 * CuData must contain valid std::string minimum and maximum values that can be
 * converted to double. The required keys are "min" and "max", respectively.
 */
void QuPlotBase::configure(const CuData &da)
{
    CuVariant m, M;
    m = da["min"];  // min value
    M = da["max"];  // max value
    bool okl, oku;  // toDouble ok for lower and upper bound
    double lb, ub;  // double for lower and upper bound
    QString min = QString::fromStdString(m.toString()); // min is of type string
    QString max = QString::fromStdString(M.toString()); // max is of type string
    lb = min.toDouble(&okl);  // string to double, see if ok
    ub = max.toDouble(&oku);  // string to double, see if ok

    if(okl && oku)
    {
        // get current default lower and upper bounds
        double current_def_lb = defaultLowerBound(QwtPlot::yLeft);
        double current_def_ub = defaultUpperBound(QwtPlot::yLeft);
        // if the minimum saved into lb is smaller than the current lower bound
        if(current_def_lb > lb)
            current_def_lb = lb;
        if(current_def_ub < ub)
            current_def_ub = ub;
        cuprintf("\e[1;35mbounds from auto %f %f\n\e[0m\n", current_def_lb, current_def_ub);
        setDefaultBounds(current_def_lb, current_def_ub, QwtPlot::yLeft);
    }
}


/**
 * @brief QuPlotBase::contextMenuEvent executes a contextual menu when the plot is right clicked.
 *
 * A QuWidgetContextMenuStrategyI implementation must be provided.
 * A default strategy for QuPlotBase is provided by QuPlotBaseContextMenuStrategy, Install it calling
 * QuPlotBase::setContextMenuStrategy.
 *
 * \note
 * No strategy is installed by default.
 *
 * \note
 * The ownership of the context menu strategy is taken by QuPlotBase and destroyed within the class
 * destructor.
 */
void QuPlotBase::contextMenuEvent(QContextMenuEvent *)
{
    static_cast<QuPlotContextMenuComponent *>(d->components_map["context_menu"])->execute(this, d->ctxMenuStrategy, QCursor::pos());
}

int QuPlotBase::refreshTimeout() const
{
    return d->refresh_timeo;
}

double QuPlotBase::defaultLowerBound(QwtPlot::Axis axisId) const
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->defaultLowerBound(axisId);
}

double QuPlotBase::defaultUpperBound(QwtPlot::Axis axisId) const
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->defaultUpperBound(axisId);
}

bool QuPlotBase::inZoom() const
{
    return static_cast<QuPlotZoomComponent *>(d->components_map.value("zoom"))->inZoom();
}

/** \brief updates the marker, if visible, and returns true if it's visible, false otherwise
 *
 * @see refresh
 * @see updateScales
 */
bool QuPlotBase::updateMarker()
{
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    return marker->update(this);
}

/** \brief Updates the axes bounds for which autoscale is enabled.
 *
 * \note replot is not called
 *
 * @return true if the one or more axes bounds have changed, false otherwise.
 *
 * @see refresh
 */
bool QuPlotBase::updateScales()
{
    bool boundsChanged = false;
    QuPlotZoomComponent* zoomer = static_cast<QuPlotZoomComponent *>(d->components_map.value("zoom"));
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));

    double old_lb, old_ub;
    QList<int> axisIds = QList<int>() << QwtPlot::xBottom << QwtPlot::yLeft;
    foreach(int axisId, axisIds)
    {
        if(axes_c->autoscale(axisId))
        {
            old_lb = axes_c->lowerBoundFromCurves(axisId);
            old_ub = axes_c->upperBoundFromCurves(axisId);
            axes_c->setBoundsFromCurves(this, axisId);
            if(!zoomer->inZoom())
                boundsChanged |= axes_c->applyScaleFromCurveBounds(this, axisId); // no updates until replot
            else if(zoomer->inZoom())
                zoomer->changeRect(axisId, axes_c->lowerBoundFromCurves(axisId) - old_lb,
                                 axes_c->upperBoundFromCurves(axisId) - old_ub);
        }
    }
    return boundsChanged;
}

void QuPlotBase::resetZoom()
{
    QuPlotZoomComponent* zoomer = static_cast<QuPlotZoomComponent *>(d->components_map.value("zoom"));
    if(!zoomer->inZoom())
        zoomer->setZoomBase(false);
}
/** \brief Calls updateMarkers, updateScales and replot, resetting the zoom base in the end.
 *
 * \note reimplement in subclasses to customise the behaviour (for example, to perform an
 *       incremental replot)
 *
 * @see TrendPlot::refresh
 * @see updateMarkers
 * @see updateScales
 */
void QuPlotBase::refresh()
{
    updateMarker();
    updateScales();
    replot();
    resetZoom();
}

void QuPlotBase::drawCanvas(QPainter *p)
{
    QwtPlot::drawCanvas(p);

    CuData options;
    QuPlotCanvasPainterComponent *painter_c =
            static_cast<QuPlotCanvasPainterComponent *>(d->components_map.value("canvas_painter"));
    options["show_title"] = titleOnCanvasEnabled();
    options["show_zoom_hint"] = d->displayZoomHint;
    options["zoom_disabled"] = zoomDisabled();
    painter_c->update(p, this,  curves(), options);
}

void QuPlotBase::setTitleOnCanvasEnabled(bool en)
{
    d->titleOnCanvasEnabled = en;
    replot();
}

QList<QwtPlotCurve *> QuPlotBase::curves() const
{
    QList<QwtPlotCurve *> curves;
    foreach(QwtPlotItem* i, itemList())
    {
        if(i->rtti() == QwtPlotItem::Rtti_PlotCurve ||
                i->rtti() == QuPlotCurve::Rtti_PlotUserItem + RTTI_CURVE_OFFSET)
            curves.push_back(static_cast<QwtPlotCurve* >(i));
    }
    return curves;
}

QuPlotCurve *QuPlotBase::curve(const QString& name)
{
    return d->curvesMap.value(name);
}

void QuPlotBase::addCurve(const QString &curveName)
{
    QuPlotCurve *plotCurve = new QuPlotCurve(curveName);
    /* and add it to the plot. This will select a color for the curve */
    addCurve(curveName, plotCurve);
}

void QuPlotBase::addCurve(const QString& title, QuPlotCurve *curve)
{
    if(d->curvesMap.contains(title))
        delete curve;
    QuPalette palette;
    QStringList colors = QStringList() << "dark_green" << "blue" << "yellow" << "violet"
                                       << "red" << "black" << "light_gray" << "green" << "gray"
                                       << "orange" << "pink" << "dark_red";
    QString color_nam = colors.at(d->curvesMap.size() % colors.size());
    QColor curveColor = palette.value(color_nam);
    QPen curvePen(curveColor);
    curvePen.setWidth(0);
    curve->setStyle(QwtPlotCurve::Lines);
    curve->setPen(curvePen);
    d->curvesMap.insert(title, curve);
    curve->attach(this);
    emit curveAdded(curve);
}

void QuPlotBase::removeCurve(const QString &curveName)
{
    if(!d->curvesMap.contains(curveName))
        return;

    QuPlotCurve* curve = d->curvesMap.value(curveName);
    d->curvesMap.remove(curveName);
    curve->detach();
    emit curveRemoved(curve);
    delete curve;
}

void QuPlotBase::clearPlot()
{
    foreach(QuPlotCurve *c, d->curvesMap.values())
        c->clearData();
    replot();
}

void QuPlotBase::appendData(const QString& curveName, double x, double y)
{
    appendData(curveName, &x, &y, 1);
}

void QuPlotBase::setRefreshTimeout(int millis)
{
    d->refresh_timeo = millis;
    if(millis > 0)
    {
        QTimer *t = new QTimer(this);
        t->setObjectName("quplot_refresh_timer");
        t->setInterval(millis);
        connect(t, SIGNAL(timeout()), this, SLOT(refresh()));
        t->start();
    }
    else
    {
        QTimer *t = findChild<QTimer *>("quplot_refresh_timer");
        if(t) delete t;
    }
}

void QuPlotBase::setDefaultBounds(double lb, double ub, QwtPlot::Axis axisId)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setDefaultBounds(this, axisId, lb, ub);
}

void QuPlotBase::restoreDefaultBounds(QwtPlot::Axis axisId)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->restoreDefaultBounds(this, axisId);
}

void QuPlotBase::setAxisScaleDefaultEnabled(bool en, QwtPlot::Axis axisId)
{
    if(en)
    {
        QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
        axes_c->setAutoscale(axisId, false);
        restoreDefaultBounds();
    }
}

void QuPlotBase::appendData(const QString& curveName, double *x, double *y, int size)
{
    QuPlotCurve* curve = this->curve(curveName);
    if(!curve)
        return;
    int bufSiz = dataBufferSize();
    while(bufSiz > 0 && curve->count() > bufSiz)
        curve->popFront();

    curve->appendData(x, y, size);
    curve->updateRawData();

    if(d->refresh_timeo <= 0)
        refresh();
}

void QuPlotBase::setData(const QString& curveName, const QVector< double > &xData, const QVector< double > &yData)
{
    QuPlotCurve* curve = d->curvesMap.value(curveName);
    if(!curve)
        return;
    curve->setData(xData, yData);
    if(d->refresh_timeo <= 0)
        refresh();
}

void QuPlotBase::setCurveStyle(const QString &name, QwtPlotCurve::CurveStyle style)
{
    QwtPlotCurve *c = curve(name);
    if(c)
    {
        c->setStyle(style);
        replot();
    }
}

bool QuPlotBase::titleOnCanvasEnabled() { return d->titleOnCanvasEnabled; }

bool QuPlotBase::xAxisAutoscaleEnabled() {
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->autoscale(QwtPlot::xBottom);
}

bool QuPlotBase::yAxisAutoscaleEnabled() {
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->autoscale(QwtPlot::yLeft);
}

void QuPlotBase::setDataBufferSize(int s)
{
    d->bufSiz = s;
}

int QuPlotBase::dataBufferSize() {
    return d->bufSiz;
}

void QuPlotBase::setXAxisAutoscaleEnabled(bool autoscale)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscale(QwtPlot::xBottom, autoscale);
    if(!autoscale)
        axes_c->setBounds(this, QwtPlot::xBottom, axisScaleDiv(QwtPlot::xBottom).lowerBound(), axisScaleDiv(QwtPlot::xBottom).upperBound());
    refresh();
}

void QuPlotBase::setYAxisAutoscaleEnabled(bool autoscale)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscale(QwtPlot::yLeft, autoscale);
    if(!autoscale)
        axes_c->setBounds(this, QwtPlot::yLeft, axisScaleDiv(QwtPlot::yLeft).lowerBound(), axisScaleDiv(QwtPlot::yLeft).upperBound());
    refresh();
}

double QuPlotBase::yUpperBound()
{
    return axisScaleDiv(QwtPlot::yLeft).upperBound();
}

double QuPlotBase::yLowerBound()
{
    return axisScaleDiv(QwtPlot::yLeft).lowerBound();
}

/*
 * \brief Disable autoscale and set y lower bound
 */
void QuPlotBase::setYLowerBound(double l)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setManualBounds(this, QwtPlot::yLeft, l, axisScaleDiv(QwtPlot::yLeft).upperBound());
    refresh();
}

/*
 * \brief Disable autoscale and set x upper bound
 */
void QuPlotBase::setYUpperBound(double u)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setManualBounds(this, QwtPlot::yLeft, axisScaleDiv(QwtPlot::yLeft).lowerBound(), u);
    refresh();
}

double QuPlotBase::xUpperBound()
{
    return axisScaleDiv(QwtPlot::xBottom).upperBound();
}

double QuPlotBase::xLowerBound()
{
    return axisScaleDiv(QwtPlot::xBottom).lowerBound();
}

/*
 * \brief Disable autoscale and set x lower bound
 */
void QuPlotBase::setXLowerBound(double l)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setManualBounds(this, QwtPlot::xBottom, l, axisScaleDiv(QwtPlot::xBottom).upperBound());
    refresh();
}

/*
 * \brief Disable autoscale and set x upper bound
 */
void QuPlotBase::setXUpperBound(double u)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setManualBounds(this, QwtPlot::xBottom, axisScaleDiv(QwtPlot::xBottom).lowerBound(), u);
    refresh();
}

void QuPlotBase::setXAutoscaleMargin(double adj)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscaleMargin(QwtPlot::xBottom, adj);
}

void QuPlotBase::setUpperBoundExtra(int axisId, double e)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setUpperBoundExtra(axisId, e);
}

double QuPlotBase::upperBoundExtra(int axisId) const
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->upperBoundExtra(axisId);
}

double QuPlotBase::xAutoscaleMargin() {
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->autoscaleMargin(QwtPlot::xBottom);
}

void QuPlotBase::setYAutoscaleAdjustment(double a)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscaleMargin(QwtPlot::yLeft, a);
}

double QuPlotBase::yAutoscaleAdjustment()
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->autoscaleMargin(QwtPlot::yLeft);
}

bool QuPlotBase::xAxisLogScale()
{
    if(dynamic_cast<QwtLogScaleEngine *>(axisScaleEngine(QwtPlot::xBottom)))
        return true;
    return false;
}

bool QuPlotBase::yAxisLogScale()
{
    if(dynamic_cast<QwtLogScaleEngine *>(axisScaleEngine(QwtPlot::yLeft)))
        return true;
    return false;
}

void QuPlotBase::setXAxisLogScale(bool l)
{
    if(l)
        setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine());
    else
        setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine());
}

void QuPlotBase::setYAxisLogScale(bool l)
{
    if(l)
        setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
    else
        setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine());
}

void QuPlotBase::displayZoomHint()
{
    d->displayZoomHint = true;
    this->replot();
}

void QuPlotBase::eraseZoomHint()
{
    d->displayZoomHint = false;
    this->replot();
}

bool QuPlotBase::zoomDisabled() const
{
    return findChild<ShiftClickEater *>("shiftClickEater") != NULL;
}

void QuPlotBase::setZoomDisabled(bool disable)
{
    ShiftClickEater *shiftClickEater = findChild<ShiftClickEater *>("shiftClickEater");
    if(disable && !shiftClickEater)
    {
        pinfo("creating shift + click eater to eat zoom events");
        shiftClickEater = new ShiftClickEater(this);
        shiftClickEater->setObjectName("shiftClickEater");
        canvas()->installEventFilter(shiftClickEater);
    }
    if(!disable && shiftClickEater)
    {
        canvas()->removeEventFilter(shiftClickEater);
        delete shiftClickEater;
    }
}

int QuPlotBase::findClosestPoint(QPoint p, QwtPlotCurve **closestCrv)
{
    QList<double> distances;
    QMap<QwtPlotCurve*, double> curveDistancesMap;
    QMap<QwtPlotCurve*, int> curveClosestPointMap;
    int closestPoint = -1;
    double dist = -1, minDist = -1;
    *closestCrv = NULL;

    foreach(QwtPlotItem* i, itemList())
    {
        if(i->rtti() == QwtPlotItem::Rtti_PlotUserItem + RTTI_CURVE_OFFSET ||
                i->rtti() == QwtPlotItem::Rtti_PlotCurve)
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve* >(i);
            if(c->isVisible())
            {
                closestPoint = c->closestPoint(p, &dist);
                if(closestPoint > -1)
                {
                    curveDistancesMap.insert(c, dist);
                    curveClosestPointMap.insert(c, closestPoint);
                }
//                printf("curve %s, dist %.2f closestPoint %d\n", qstoc(c->title().text()), dist, closestPoint);
            }
        }
    }

    distances = curveDistancesMap.values();
    if(distances.size() > 0)
    {
        std::sort(distances.begin(), distances.end());
        minDist = distances.first();
        closestPoint = curveClosestPointMap.value(curveDistancesMap.key(minDist));
        if(curveClosestPointMap.values().contains(closestPoint))
            *closestCrv = curveDistancesMap.key(minDist);
    }
    return closestPoint;
}

void QuPlotBase::hideMarker()
{
    static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"))->hide();
    replot();
}

void QuPlotBase::mouseReleaseEvent(QMouseEvent *ev)
{
    printf("\e[1;32mMouseRELEASE\e[0m\n");
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    if(ev->button() == Qt::MidButton && marker->isVisible())
        hideMarker();
    QWidget::mouseReleaseEvent(ev);
}

void QuPlotBase::keyPressEvent(QKeyEvent *ke)
{
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    if(marker->isVisible())
    {
        printf("key press move marker\n");
        ke->ignore();
        if(ke->key() == Qt::Key_Left)
            moveCurveToYRight(marker->currentClosestCurve(), false);
        else if(ke->key() == Qt::Key_Right)
            moveCurveToYRight(marker->currentClosestCurve(), true);
    }
    else
        QwtPlot::keyPressEvent(ke);
}

void QuPlotBase::plotZoomed(const QRectF&)
{
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    if(marker->isVisible())
        hideMarker();
}

void QuPlotBase::moveCurveToYRight(QwtPlotCurve *c, bool yr)
{
    int yAxis = c->yAxis();
    if(yAxis == QwtPlot::yLeft && yr)
    {
        printf("moving to --->\n");
        c->setAxes(c->xAxis(), QwtPlot::yRight);
    }
    else if(yAxis == QwtPlot::yRight && !yr)
    {
        printf("<---- moving to\n");
        c->setAxes(c->xAxis(), QwtPlot::yLeft);
    }
}

void QuPlotBase::showMarker(const QPolygon &p)
{
    int closestPoint;
    QwtPlotCurve *closestCurve;
    double x, y;
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    closestPoint = findClosestPoint(p.point(0), &closestCurve);

    if (closestPoint != -1)
    {
        marker->show();
        updateLabel(closestCurve, closestPoint);
        x = closestCurve->data()->sample(closestPoint).x();
        y = closestCurve->data()->sample(closestPoint).y();
        emit plotClicked(QCursor::pos(), closestCurve, x, y, QPoint(transform(closestCurve->xAxis(), x),
                                                                    transform(closestCurve->yAxis(), y)));
        qDebug() << mapFromGlobal(QCursor::pos()) << "title: " << closestCurve->title().text() << "x " << x <<
                    y << "point on plot " <<
                    QPoint(transform(closestCurve->xAxis(), x), transform(closestCurve->yAxis(), y));
    }
    else
    {
        marker->hide();
    }
    replot();
}

void QuPlotBase::updateLabel(QwtPlotCurve *closestCurve, int closestPointIdx)
{
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
//    printf("\e[1;32m updateLabel() closestPoint %d\e[0m\n", closestPointIdx);

    if(closestCurve && closestPointIdx > -1 && marker->isVisible())
    {
        marker->update(this, closestCurve, closestPointIdx);
        replot();
    }
}


void QuPlotBase::print() {
    static_cast<QuPlotContextMenuComponent *>(d->components_map["context_menu"])->print(this);
}

void QuPlotBase::snapshot(){
    static_cast<QuPlotContextMenuComponent *>(d->components_map["context_menu"])->snapshot(this);
}

void QuPlotBase::copyImage(){
    static_cast<QuPlotContextMenuComponent *>(d->components_map["context_menu"])->copyImage(this);
}

void QuPlotBase::saveData(){
    static_cast<QuPlotContextMenuComponent *>(d->components_map["context_menu"])->saveData(this);
}

void QuPlotBase::configurePlot(){
    static_cast<QuPlotContextMenuComponent *>(d->components_map["context_menu"])->configurePlot(this);
}

QDialog *QuPlotBase::createConfigureDialog()
{
    EPlotConfigurationDialog  *d = new EPlotConfigurationDialog(this);
    return d;
}
