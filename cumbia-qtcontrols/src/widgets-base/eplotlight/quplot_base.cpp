#include "quplot_base.h"
#include "quplotconfigurator.h"
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

// plot components
#include "quplotzoomcomponent.h"
#include "quplotmarkercomponent.h"
#include "quplotcanvaspaintercomponent.h"
#include "quplotaxescomponent.h"
#include "quplotcontextmenucomponent.h"
#include "eplot_configuration_widget.h"
#include "quwidgetcontextmenustrategyi.h"
#include "quwidgetupdatestrategy_i.h"

//#ifndef __FreeBSD__
//#include <values.h>
//#endif

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
    QuPlotBase::init();
}

QuPlotBase::QuPlotBase(const QwtText &title, QWidget *parent) : QwtPlot(title, parent)
{
    QuPlotBase::init();
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

    plotLayout()->setAlignCanvasToScales(true);
    plotLayout()->setCanvasMargin(0, QwtPlot::yLeft);
    plotLayout()->setCanvasMargin(0, QwtPlot::yRight);

    /* white background */
    setCanvasBackground(Qt::white);
    /* disable qwt auto replot */
    setAutoReplot(false);
    /* grid */
    QwtPlotGrid* plotgrid = new QwtPlotGrid;
    plotgrid->setPen(QPen(QColor(230,230,248)));
    plotgrid->attach(this);
    plotgrid->enableX(true);
    plotgrid->enableY(true);
    setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
    setZoomDisabled(false);

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

    QuPlotConfigurator pco;
    pco.configure(this);

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
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    m = da["min"];  // min value
    M = da["max"];  // max value
    bool okl, oku;  // toDouble ok for lower and upper bound
    double lb, ub;  // double for lower and upper bound
    double current_def_lb, current_def_ub;
    QwtPlot::Axis axisId = (da.s("yaxis") == "left" ? QwtPlot::yLeft : QwtPlot::yRight);
    QString min = QString::fromStdString(m.toString()); // min is of type string
    QString max = QString::fromStdString(M.toString()); // max is of type string
    lb = min.toDouble(&okl);  // string to double, see if ok
    ub = max.toDouble(&oku);  // string to double, see if ok

    if(okl && oku && lb != ub)
    {
        // get current default lower and upper bounds
        current_def_lb = defaultLowerBound(axisId);
        current_def_ub = defaultUpperBound(axisId);
        // if the minimum saved into lb is smaller than the current lower bound
        if(current_def_lb > lb)
            current_def_lb = lb;
        if(current_def_ub < ub)
            current_def_ub = ub;
    }
    else {

        // initialised (to 0 and 1000) in QuPlotAxesComponent's constructor
        current_def_lb = axes_c->lowerBoundFromCurves(axisId);
        current_def_ub  = axes_c->upperBoundFromCurves(axisId);
    }
    setDefaultBounds(current_def_lb, current_def_ub, axisId);
    if(axes_c->scaleMode(axisId) == QuPlotAxesComponent::SemiAutoScale) {
        setAxisScale(axisId, current_def_lb, current_def_ub);
    }
    // if configuration happens after data, need replot
    replot();
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

/*!
 * \brief convert input vector of timestamps in microseconds to a vector of timestamps in
 *        milliseconds
 * \param ts_us *non const* reference vector of double representing timestamps in the format
 * seconds.microseconds that will be converted into milliseconds.microseconds
 */
void QuPlotBase::us_to_ms(std::vector<double> &ts_us) const {
    for(size_t i = 0; i < ts_us.size(); i++)
        ts_us[i] = ts_us[i] * 1000.0;
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
    QuPlotZoomComponent* z = static_cast<QuPlotZoomComponent *>(d->components_map.value("zoom"));
    return z && z->inZoom();
}

/*! \brief returns the QuPlotComponent corresponding to the given *name*, nullptr if *name* is not valid
 *
 * \par Component list
 * \li "context_menu"  returns QuPlotContextMenuComponent
 * \li "axes" returns QuPlotAxesComponent
 * \li "zoom" returns QuPlotZoomComponent
 * \li "marker" returns QuPlotMarkerComponent
 * \li "canvas_painter" QuPlotCanvasPainterComponent
 *
 */
QuPlotComponent *QuPlotBase::getComponent(const QString &name) const
{
    if(d->components_map.contains(name))
        return d->components_map[name];
    return nullptr;
}

void QuPlotBase::registerComponent(QuPlotComponent *c, const QString &name) {
    d->components_map[name] = c;
}

/*!
 * \brief QuPlotBase::unregisterComponent unregister the component with the given name
 * \param name the name of the component, f.e. *zoom, marker, axes, canvas_painter*
 * \return the component removed from the map, if found. A null pointer otherwise.
 *         The returned pointer can be used to delete the component.
 */
QuPlotComponent *QuPlotBase::unregisterComponent(const QString &name) {
    QuPlotComponent *co = d->components_map[name];
    d->components_map.remove(name);
    return co;
}

/** \brief updates the marker, if visible, and returns true if it's visible, false otherwise
 *
 * @see refresh
 * @see updateScales
 */
bool QuPlotBase::updateMarker()
{
    bool updated = false;
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    if(marker->isVisible()) {
        updated = marker->update(this);
        if(updated) {
            double x = marker->currentClosestCurve()->data()->sample(marker->currentClosestPoint()).x();
            double y = marker->currentClosestCurve()->data()->sample(marker->currentClosestPoint()).y();
            emit markerTextChanged(marker->yLabel(), marker->xLabel(), x, y);
        }
    }
    return updated;
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
    double xm, xM, ym, yM;
    bool need_xbounds = false, need_ybounds = false;
    QList<int> autoScaleAxisIds; // will contain only axes that need autoscaling
    QList<int> axisIds = QList<int>()<< QwtPlot::yLeft << QwtPlot::yRight << QwtPlot::xBottom << QwtPlot::xTop; // 0, 1, 2, 3
    foreach(int axisId, axisIds) {
        if(((axisId == QwtPlot::xBottom && axisEnabled(QwtPlot::xBottom) ) || (axisId == QwtPlot::xTop && axisEnabled(QwtPlot::xTop))) && axes_c->autoscale(axisId)) {
            autoScaleAxisIds << axisId;
            need_xbounds = true;
        }
        else if(((axisId == QwtPlot::yLeft && axisEnabled(QwtPlot::yLeft)) || (axisId == QwtPlot::yRight && axisEnabled(QwtPlot::yRight)) ) && axes_c->autoscale(axisId)) {
            autoScaleAxisIds << axisId;
            need_ybounds = true;
        }
    }
    if(need_xbounds || need_ybounds) // get the bounds for the needed axes
        axes_c->getBoundsFromCurves(this, &xm, &xM, &ym, &yM, need_xbounds, need_ybounds);

    foreach(int axisId, autoScaleAxisIds) {
        old_lb = axes_c->lowerBoundFromCurves(axisId);
        old_ub = axes_c->upperBoundFromCurves(axisId);
        if(need_xbounds && (axisId == QwtPlot::xBottom || axisId == QwtPlot::xTop)) {
            axes_c->setBoundsFromCurves(xm, xM, axisId);
        }
        else if(need_ybounds && (axisId == QwtPlot::yRight || axisId == QwtPlot::yLeft))
            axes_c->setBoundsFromCurves(ym, yM, axisId);

        if(zoomer && !zoomer->inZoom())
            boundsChanged |= axes_c->applyScaleFromCurveBounds(this, axisId); // no updates until replot
        else if(zoomer)
            zoomer->changeRect(axisId, axes_c->lowerBoundFromCurves(axisId) - old_lb,
                               axes_c->upperBoundFromCurves(axisId) - old_ub);
    }
    return boundsChanged;
}

void QuPlotBase::resetZoom()
{
    QuPlotZoomComponent* zoomer = static_cast<QuPlotZoomComponent *>(d->components_map.value("zoom"));
    if(zoomer && !zoomer->inZoom())
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
    QuPlotConfigurator pco;
    if(d->curvesMap.contains(title)) // remove existing curve
        delete d->curvesMap[title];
    // either load from saved settings or pick from a palette
    pco.configure_curve(this, curve, d->curvesMap.size());
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

/*!
 * \brief insert data into the plot ordered by x values
 *
 * \param curveName the name of the curve
 * \param xData pointer to x data
 * \param yData pointer of y data
 * \param size size of x and y
 *
 * Data contained in points (xData,yData) is inserted into the existing data for the given curve
 * so that the resulting set is the union of existing and new data ordered by x
 *
 * \since v1.1.0
 */
void QuPlotBase::insertData(const QString &curveName, double *xData, double *yData, int size, double default_y)
{
    QuPlotCurve* curve = d->curvesMap.value(curveName);
    if(curve) {
        curve->insertData(xData, yData, size, default_y);
        int bufSiz = dataBufferSize();
        while(bufSiz > 0 && curve->count() > bufSiz) {
            curve->popFront();
        }
        curve->updateRawData();
        if(d->refresh_timeo <= 0) {
            refresh();
        }
    }
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

/*! \brief restores the bounds set with setDefaultBounds
 *
 * \par Note
 * The QuPlotAxesComponent::ScaleMode flag is set to SemiAutomatic if en is true
 *
 * While scale mode is *semi automatic*, the axis can be (re)configured
 * by subclasses, for example with bounds associated to a source obtained from a file
 * or database
 */
void QuPlotBase::restoreDefaultBounds(QwtPlot::Axis axisId)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->restoreDefaultBounds(this, axisId);
    replot();
}

/*! \brief Restores the bounds of axisId to their default values if en is true, does nothing if en is false
 *
 * \par Note
 * The QuPlotAxesComponent::ScaleMode flag is set to SemiAutomatic if en is true
 *
 * @param en true the default bounds of the axisId are reset to their default values
 * @param en false the function does nothing
 * @param axisId the id of the axis which boundaries are to be restored
 *
 */
void QuPlotBase::setAxisScaleDefaultEnabled(bool en, QwtPlot::Axis axisId)
{
    if(en)
    {
        QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
        axes_c->setAutoscale(axisId, false);
        restoreDefaultBounds(); // QuPlotAxesComponent::ScaleMode is set to SemiAutomatic
    }
}

void QuPlotBase::appendData(const QString& curveName, double *x, double *y, int size)
{
    QuPlotCurve* curve = this->curve(curveName);
    if(!curve)
        return;
    int bufSiz = dataBufferSize();
    while(bufSiz > 0 && curve->count() > bufSiz) {
        curve->popFront();
    }

    curve->appendData(x, y, size);
    curve->updateRawData();

    if(d->refresh_timeo <= 0) {
        refresh();
    }
}

void QuPlotBase::setData(const QString& curveName, const QVector< double > &xData, const QVector< double > &yData)
{
    QuPlotCurve* curve = d->curvesMap.value(curveName);
    if(!curve)
        return;
    curve->setData(xData, yData);
    if(d->refresh_timeo <= 0) {
        refresh();
    }
}

void QuPlotBase::setData(const QString &curveName, double *xData, double *yData, int size)
{
    QuPlotCurve* curve = d->curvesMap.value(curveName);
    if(curve) {
        curve->setSamples(xData, yData, size);
        if(d->refresh_timeo <= 0) {
            refresh();
        }
    }
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

bool QuPlotBase::xAxisAutoscaleEnabled(QwtPlot::Axis axis) {
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->autoscale(axis);
}

bool QuPlotBase::yAxisAutoscaleEnabled(QwtPlot::Axis axis) {
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    return axes_c->autoscale(axis);
}

void QuPlotBase::setDataBufferSize(int s)
{
    d->bufSiz = s;
}

int QuPlotBase::dataBufferSize() {
    return d->bufSiz;
}

void QuPlotBase::setXAxisAutoscaleEnabled(bool autoscale, QwtPlot::Axis axis)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscale(axis, autoscale);
    if(!autoscale) {
        axes_c->setBounds(this, axis, axisScaleDiv(axis).lowerBound(), axisScaleDiv(axis).upperBound());
    }
    replot();
}

void QuPlotBase::setYAxisAutoscaleEnabled(bool autoscale, QwtPlot::Axis axis)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscale(axis, autoscale);
    if(!autoscale)
        axes_c->setBounds(this, axis, axisScaleDiv(axis).lowerBound(), axisScaleDiv(axis).upperBound());
    replot();
}

void QuPlotBase::setXAxisAutoscaleEnabled(bool autoscale)
{
    setXAxisAutoscaleEnabled(autoscale, QwtPlot::xBottom);
}

void QuPlotBase::setYAxisAutoscaleEnabled(bool autoscale)
{
    setYAxisAutoscaleEnabled(autoscale, QwtPlot::yLeft);
}


void QuPlotBase::setXTopAxisAutoscaleEnabled(bool autoscale) {
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscale(QwtPlot::xTop, autoscale);
    if(!autoscale)
        axes_c->setBounds(this, QwtPlot::xTop, axisScaleDiv(QwtPlot::xTop).lowerBound(), axisScaleDiv(QwtPlot::xTop).upperBound());
    replot();
}

void QuPlotBase::setYRightAxisAutoscaleEnabled(bool autoscale) {
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscale(QwtPlot::yRight, autoscale);
    if(!autoscale)
        axes_c->setBounds(this, QwtPlot::yRight, axisScaleDiv(QwtPlot::yRight).lowerBound(), axisScaleDiv(QwtPlot::yRight).upperBound());
    replot();
}

double QuPlotBase::yUpperBound(QwtPlot::Axis axis)
{
    return axisScaleDiv(axis).upperBound();
}

double QuPlotBase::yLowerBound(QwtPlot::Axis axis)
{
    return axisScaleDiv(axis).lowerBound();
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

double QuPlotBase::xUpperBound(QwtPlot::Axis axis)
{
    return axisScaleDiv(axis).upperBound();
}

double QuPlotBase::xLowerBound(QwtPlot::Axis axis)
{
    return axisScaleDiv(axis).lowerBound();
}

/*
 * \brief Disable autoscale and set x lower bound
 */
void QuPlotBase::setXLowerBound(double l)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setManualBounds(this, QwtPlot::xBottom, l, axisScaleDiv(QwtPlot::xBottom).upperBound());
    replot();
}

/*
 * \brief Disable autoscale and set x upper bound
 */
void QuPlotBase::setXUpperBound(double u)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setManualBounds(this, QwtPlot::xBottom, axisScaleDiv(QwtPlot::xBottom).lowerBound(), u);
    replot();
}

void QuPlotBase::setXAutoscaleMargin(double adj)
{
    QuPlotAxesComponent *axes_c = static_cast<QuPlotAxesComponent *>(d->components_map.value("axes"));
    axes_c->setAutoscaleMargin(QwtPlot::xBottom, adj);
}

/*! \brief add some extra upper bound to the specified axis to optimize replot operations when the
 *         refresh rate is fast.
 *
 * @param axisId the axis id, one of QwtPlot::xBottom, QwtPlot::xTop, QwtPlot::yLeft, QwtPlot::yRight
 * @param e the extra bound to add to the scale, percentage expressed from 0 to 1
 */
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

bool QuPlotBase::xAxisLogScale(QwtPlot::Axis axis)
{
    if(dynamic_cast<QwtLogScaleEngine *>(axisScaleEngine(axis)))
        return true;
    return false;
}

bool QuPlotBase::yAxisLogScale(QwtPlot::Axis axis)
{
    if(dynamic_cast<QwtLogScaleEngine *>(axisScaleEngine(axis)))
        return true;
    return false;
}

void QuPlotBase::setXAxisLogScale(bool l, QwtPlot::Axis axis)
{
    if(l)
        setAxisScaleEngine(axis, new QwtLogScaleEngine());
    else
        setAxisScaleEngine(axis, new QwtLinearScaleEngine());
}

void QuPlotBase::setYAxisLogScale(bool l, QwtPlot::Axis axis)
{
    if(l)
        setAxisScaleEngine(axis, new QwtLogScaleEngine());
    else
        setAxisScaleEngine(axis, new QwtLinearScaleEngine());
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
        shiftClickEater = new ShiftClickEater(this);
        shiftClickEater->setObjectName("shiftClickEater");
        canvas()->installEventFilter(shiftClickEater);
    }
    if(!disable && shiftClickEater)
    {
        canvas()->removeEventFilter(shiftClickEater);
        delete shiftClickEater;
    }
    QuPlotZoomComponent *zoom_c = static_cast<QuPlotZoomComponent *>(d->components_map["zoom"]);
    if(!disable && !zoom_c) {
        zoom_c = new QuPlotZoomComponent(this);
        zoom_c->attachToPlot(this);
        zoom_c->connectToPlot(this);
        d->components_map.insert(zoom_c->name(), zoom_c);
    }
    else if(zoom_c) {
        d->components_map.remove("zoom");
        delete zoom_c;
    }
}

int QuPlotBase::findClosestPoint(QPoint p, QwtPlotCurve **closestCrv)
{
    QList<double> distances;
    QHash<QwtPlotCurve*, double> curveDistancesMap;
    QHash<QwtPlotCurve*, int> curveClosestPointMap;
    int closestPoint = -1;
    double dist = -1, minDist = -1;
    *closestCrv = nullptr;
    foreach(QwtPlotItem* i, itemList()) {
        if(i->rtti() == QwtPlotItem::Rtti_PlotUserItem + RTTI_CURVE_OFFSET ||
                i->rtti() == QwtPlotItem::Rtti_PlotCurve)
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve* >(i);
            if(c->isVisible())
            {
                closestPoint = c->closestPoint(p, &dist);
                if(closestPoint > -1) {
                    curveDistancesMap.insert(c, dist);
                    curveClosestPointMap.insert(c, closestPoint);
                }
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
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    if(ev->button() == Qt::MiddleButton && marker->isVisible()) {
        hideMarker();
        emit markerVisibilityChanged(false);
    }
    QWidget::mouseReleaseEvent(ev);
}

void QuPlotBase::keyPressEvent(QKeyEvent *ke)
{
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    if(marker->isVisible())
    {
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
    if(marker->isVisible()) {
        hideMarker();
        emit markerVisibilityChanged(false);
    }
}

void QuPlotBase::moveCurveToYRight(QwtPlotCurve *c, bool yr)
{
    int yAxis = c->yAxis();
    if(yAxis == QwtPlot::yLeft && yr)
    {
        printf("moving to --->\n");
        if(!axisEnabled(QwtPlot::yRight))
            enableAxis(QwtPlot::yRight, true);
        c->setAxes(c->xAxis(), QwtPlot::yRight);
    }
    else if(yAxis == QwtPlot::yRight && !yr)
    {
        printf("<---- moving to\n");
        if(!axisEnabled(QwtPlot::yLeft))
            enableAxis(QwtPlot::yLeft, true);
        c->setAxes(c->xAxis(), QwtPlot::yLeft);
    }
}

void QuPlotBase::showMarker(const QPolygon &p)
{
    int closestPoint;
    QwtPlotCurve *closestCurve = nullptr;
    double x = 0.0, y = 0.0;
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    closestPoint = findClosestPoint(p.point(0), &closestCurve);

    if (closestPoint != -1 && closestCurve != nullptr) {
        marker->show();
        m_updateLabel(closestCurve, closestPoint); // does not emit marker text changed. Does not replot
        x = closestCurve->data()->sample(closestPoint).x();
        y = closestCurve->data()->sample(closestPoint).y();
        emit plotClicked(QCursor::pos(), closestCurve, x, y, QPoint(transform(closestCurve->xAxis(), x),
                                                                    transform(closestCurve->yAxis(), y)));
    }
    else {
        marker->hide();
    }
    replot();

    emit markerTextChanged(closestPoint != -1 ? marker->yLabel() : "", closestPoint != -1 ? marker->xLabel() : "", x, y);
    emit markerVisibilityChanged(closestPoint != -1);
}

void QuPlotBase::m_updateLabel(QwtPlotCurve *closestCurve, int closestPointIdx)
{
    QuPlotMarkerComponent *marker = static_cast<QuPlotMarkerComponent *>(d->components_map.value("marker"));
    if(closestCurve && closestPointIdx > -1 && marker->isVisible()) {
        marker->update(this, closestCurve, closestPointIdx);
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
