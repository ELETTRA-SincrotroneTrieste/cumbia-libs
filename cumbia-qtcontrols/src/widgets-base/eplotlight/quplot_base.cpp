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
#include <QPaintEngine>
#include <QPainter>
#include <QMouseEvent>
#include <QtDebug>
#include <QTimer>
#include <qwt_plot_marker.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_canvas.h>

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

#ifndef QWT_NO_OPENGL
#include <QwtPlotOpenGLCanvas>
#endif

class QuPlotBasePrivate
{
public:
    QuPlotBase::CurveStyle curvesStyle;
    QuWidgetUpdateStrategyI *updateStrategy;
    QuWidgetContextMenuStrategyI* ctxMenuStrategy;
    bool titleOnCanvasEnabled, displayZoomHint, open_gl;
    QMap<QString, QuPlotCurve*> curvesMap;
    int bufSiz;
    int refresh_timeo;
    /* plot components */
    QMap<QString, QuPlotComponent *> components_map;
    // for marker

    QwtPlotPicker* picker;
    QuPlotMarkerComponent *marker;
    QuPlotAxesComponent *axes;
    QuPlotCanvasPainterComponent *canvas_painter;
    QuPlotZoomComponent *zoom;
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

QuPlotBase::QuPlotBase(QWidget *parent, bool openGL) : QwtPlot(parent)
{
    QuPlotBase::init(openGL);
}

QuPlotBase::QuPlotBase(const QwtText &title, QWidget *parent, bool openGL) : QwtPlot(title, parent)
{
    QuPlotBase::init(openGL);
}

QuPlotBase::~QuPlotBase()
{
    if(d->updateStrategy)
        delete d->updateStrategy;
    if(d->ctxMenuStrategy)
        delete d->ctxMenuStrategy;
    delete d;
}

/* QwtPlot xAxis autoscale is disabled and axis autoscale is managed internally 
 * through the refresh() method.
 */
void QuPlotBase::init(bool opengl)
{
    d = new QuPlotBasePrivate;
    d->bufSiz = 3600 * 10;
    d->refresh_timeo = -1;
    d->updateStrategy = NULL;
    d->ctxMenuStrategy = NULL;
    d->titleOnCanvasEnabled = false;
    d->displayZoomHint = false;
    d->curvesStyle = Lines;
    d->open_gl = opengl;

    /* disable qwt auto replot */
    setAutoReplot(false);

    QWidget* pcanvas = createCanvas(opengl);
    m_canvas_conf(pcanvas);
    setCanvas(pcanvas);

    m_align_scales(); // 2.1, inspired by refreshtest example
    m_install_components();

    QwtPlot::replot(); /* do not need QuPlotBase::replot() here */
}

void QuPlotBase::m_align_scales() { // taken from Qwt examples' refreshtest
    for ( int axisPos = 0; axisPos < QwtAxis::AxisPositions; axisPos++ ) {
        QwtScaleWidget* scaleWidget = axisWidget( axisPos );
        if ( scaleWidget )
            scaleWidget->setMargin( 0 );
        QwtScaleDraw* scaleDraw = axisScaleDraw( axisPos );
        if ( scaleDraw )
            scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    }
    plotLayout()->setAlignCanvasToScales( true );
}

void QuPlotBase::m_install_components() {
    /* grid */
    QwtPlotGrid* plotgrid = new QwtPlotGrid;
    plotgrid->setPen(QPen(QColor(230,230,248)));
    plotgrid->attach(this);
    plotgrid->enableX(true);
    plotgrid->enableY(true);
    setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

    setZoomDisabled(false);

    d->marker = new QuPlotMarkerComponent(this);
    d->marker->attachToPlot(this);
    d->marker->connectToPlot(this);
    d->components_map.insert(d->marker->name(), d->marker);

    /* draws canvas. No need to attach or connect */
    d->canvas_painter = new QuPlotCanvasPainterComponent();
    d->components_map.insert(d->canvas_painter->name(), d->canvas_painter);

    /* manages axes */
    d->axes = new QuPlotAxesComponent(this);
    d->components_map.insert(d->axes->name(), d->axes);

    QuPlotContextMenuComponent *ctx_menu = new QuPlotContextMenuComponent();
    ctx_menu->attachToPlot(this);
    ctx_menu->connectToPlot(this);
    d->components_map.insert(ctx_menu->name(), ctx_menu);

    QuPlotConfigurator pco;
    pco.configure(this);
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
 *
 * \note
 * The method does not call *replot*
 */
void QuPlotBase::configure(const CuData &da)
{
    CuVariant m, M;
    m = da[TTT::Min];  // min value
    M = da[TTT::Max];  // max value
    bool okl, oku;  // toDouble ok for lower and upper bound
    double lb, ub;  // double for lower and upper bound
    double current_def_lb, current_def_ub;
    QwtPlot::Axis axisId = da.s("yaxis") == "right" ? QwtPlot::yRight : QwtPlot::yLeft;
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
        current_def_lb = d->axes->lowerBoundFromCurves(axisId);
        current_def_ub  = d->axes->upperBoundFromCurves(axisId);
    }
    pretty_pri("\e[1;36mconfigure\e[0m: lb %f ub %f okl %d oku %d", lb, ub, okl, oku);
    pretty_pri("\e[1;36mconfigure\e[0m: default %f,%f scale mode %d", current_def_lb, current_def_ub, d->axes->scaleMode(axisId));
    setDefaultBounds(current_def_lb, current_def_ub, axisId);
    if(d->axes->scaleMode(axisId) == QuPlotAxesComponent::SemiAutoScale) {
        setAxisScale(axisId, current_def_lb, current_def_ub);
        replot();
        pretty_pri("set axis scale ID %d from %f to %f", axisId, yLowerBound(), yUpperBound());
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
void QuPlotBase::contextMenuEvent(QContextMenuEvent *) {
    static_cast<QuPlotContextMenuComponent *>(d->components_map.value("context_menu"))->execute(this, d->ctxMenuStrategy, QCursor::pos());
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

double QuPlotBase::defaultLowerBound(QwtPlot::Axis axisId) const {
    return d->axes->defaultLowerBound(axisId);
}

double QuPlotBase::defaultUpperBound(QwtPlot::Axis axisId) const {
    return d->axes->defaultUpperBound(axisId);
}

bool QuPlotBase::inZoom() const {
    return d->zoom && d->zoom->inZoom();
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
    // shortcuts
    if(name == "zoom") d->zoom = static_cast<QuPlotZoomComponent *>(c);
    else if(name == "marker" ) d->marker = static_cast<QuPlotMarkerComponent *>(c);
    else if(name == "axes") d->axes = static_cast<QuPlotAxesComponent *>(c);
    else if(name == "canvas_painter") d->canvas_painter = static_cast<QuPlotCanvasPainterComponent *>(c);
}

/*!
 * \brief QuPlotBase::unregisterComponent unregister the component with the given name
 * \param name the name of the component, f.e. *zoom, marker, axes, canvas_painter*
 * \return the component removed from the map, if found. A null pointer otherwise.
 *         The returned pointer can be used to delete the component.
 */
QuPlotComponent *QuPlotBase::unregisterComponent(const QString &name) {
    if(name == d->zoom->name()) d->zoom = nullptr;
    else if(name == d->marker->name() ) d->marker = nullptr;
    else if(name == d->axes->name()) d->axes = nullptr;
    else if(name == d->canvas_painter->name()) d->canvas_painter = nullptr;
    QuPlotComponent *co = d->components_map.value(name);
    d->components_map.remove(name);
    return co;
}

bool QuPlotBase::isOpenGL() const {
    return qobject_cast<const QwtPlotOpenGLCanvas *>(canvas()) != nullptr;
}

/** \brief updates the marker, if visible, and returns true if it's visible, false otherwise
 *
 * @see refresh
 * @see updateScales
 */
bool QuPlotBase::updateMarker() {
    bool updated = false;
    updated = d->marker->update(this);
    if(updated) {
        double x = d->marker->currentClosestCurve()->data()->sample(d->marker->currentClosestPoint()).x();
        double y = d->marker->currentClosestCurve()->data()->sample(d->marker->currentClosestPoint()).y();
        emit markerTextChanged(d->marker->yLabel(), d->marker->xLabel(), x, y);

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
    double old_lb, old_ub;
    double xm, xM, ym, yM;
    bool need_xbounds = false, need_ybounds = false;
    QList<int> autoScaleAxisIds; // will contain only axes that need autoscaling
    QList<int> axisIds = QList<int>()<< QwtPlot::yLeft << QwtPlot::yRight << QwtPlot::xBottom << QwtPlot::xTop; // 0, 1, 2, 3
    foreach(int axisId, axisIds) {
        if(((axisId == QwtPlot::xBottom && axisEnabled(QwtPlot::xBottom) ) || (axisId == QwtPlot::xTop && axisEnabled(QwtPlot::xTop))) && d->axes->autoscale(axisId)) {
            autoScaleAxisIds << axisId;
            need_xbounds = true;
        }
        else if(((axisId == QwtPlot::yLeft && axisEnabled(QwtPlot::yLeft)) || (axisId == QwtPlot::yRight && axisEnabled(QwtPlot::yRight)) ) && d->axes->autoscale(axisId)) {
            autoScaleAxisIds << axisId;
            need_ybounds = true;
        }
    }
    if(need_xbounds || need_ybounds) { // get the bounds for the needed axes
        pretty_pri("need getting bounds for x: %s y %s", need_xbounds ? "YES" : "NO", need_ybounds ? "YES" : "NO");
        d->axes->getBoundsFromCurves(this, &xm, &xM, &ym, &yM, need_xbounds, need_ybounds);
        pretty_pri("got xm %f xM %f ym %f yM %f", xm, xM, ym, yM);
    }

    foreach(int axisId, autoScaleAxisIds) {
        old_lb = d->axes->lowerBoundFromCurves(axisId);
        old_ub = d->axes->upperBoundFromCurves(axisId);
        if(need_xbounds && (axisId == QwtPlot::xBottom || axisId == QwtPlot::xTop)) {
            pretty_pri("setting bounds %f %f on axis id %d", xm, xM, axisId);
            d->axes->setBoundsFromCurves(xm, xM, axisId);
        }
        else if(need_ybounds && (axisId == QwtPlot::yRight || axisId == QwtPlot::yLeft))
            d->axes->setBoundsFromCurves(ym, yM, axisId);

        if(d->zoom && !d->zoom->inZoom())
            boundsChanged |= d->axes->applyScaleFromCurveBounds(this, axisId); // no updates until replot
        else if(d->zoom)
            d->zoom->changeRect(axisId, d->axes->lowerBoundFromCurves(axisId) - old_lb,
                                d->axes->upperBoundFromCurves(axisId) - old_ub);
    }
    return boundsChanged;
}

void QuPlotBase::resetZoom()
{
    if(d->zoom && !d->zoom->inZoom())
        d->zoom->setZoomBase(false);
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
void QuPlotBase::refresh() {
    bool scales_changed = false;
    if(d->marker && d->marker->isVisible())
        updateMarker();
    if(d->axes) {
        scales_changed = updateScales();
    }
    scales_changed ? replot() :
        (isOpenGL() ? static_cast<QwtPlotOpenGLCanvas *>(canvas())->replot() :
                                     static_cast<QwtPlotCanvas *>(canvas())->replot());
    if(d->zoom && !d->zoom->inZoom())
        d->zoom->setZoomBase(false);
}

void QuPlotBase::drawCanvas(QPainter *p) {
    QwtPlot::drawCanvas(p);
    if(d->canvas_painter) {
        CuData options;
        options["show_title"] = titleOnCanvasEnabled();
        options["show_zoom_hint"] = d->displayZoomHint;
        options["zoom_disabled"] = zoomDisabled();
        d->canvas_painter->update(p, this,  curves(), options);
    }
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

void QuPlotBase::setDefaultBounds(double lb, double ub, QwtPlot::Axis axisId) {
     d->axes->setDefaultBounds(this, axisId, lb, ub);
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
    d->axes->restoreDefaultBounds(this, axisId);
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
    if(en) {
        d->axes->setAutoscale(axisId, false);
        restoreDefaultBounds(); // QuPlotAxesComponent::ScaleMode is set to SemiAutomatic
    }
}

void QuPlotBase::appendData(const QString& curveName, double *x, double *y, int size)
{
    QuPlotCurve* curve = this->curve(curveName);
    pretty_pri("curve: %p searcjed wotj ma,e %s", curve, qstoc(curveName));
    if(!curve)
        return;
    int bufSiz = dataBufferSize();
    while(bufSiz > 0 && curve->count() > bufSiz) {
        curve->popFront();
    }

    // curve->appendData(x, y, size);
    // curve->updateRawData();

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
    return d->axes->autoscale(axis);
}

bool QuPlotBase::yAxisAutoscaleEnabled(QwtPlot::Axis axis) {
    return d->axes->autoscale(axis);
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
    d->axes->setAutoscale(axis, autoscale);
    if(!autoscale) {
        d->axes->setBounds(this, axis, axisScaleDiv(axis).lowerBound(), axisScaleDiv(axis).upperBound());
    }
    replot();
}

void QuPlotBase::setYAxisAutoscaleEnabled(bool autoscale, QwtPlot::Axis axis)
{
    d->axes->setAutoscale(axis, autoscale);
    if(!autoscale)
        d->axes->setBounds(this, axis, axisScaleDiv(axis).lowerBound(), axisScaleDiv(axis).upperBound());
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

QWidget* QuPlotBase::createCanvas(bool opengl) {
    QWidget *ca = nullptr;
#ifndef QWT_NO_OPENGL
    if (opengl) {
        ca = qobject_cast< QwtPlotOpenGLCanvas* >( canvas() );
        if (ca == nullptr) {
            pretty_pri("switching to \e[1;32mopenGL\e[0m canvas");
            ca = new QwtPlotOpenGLCanvas();
        }
    }
    else
#endif   // ifndef QWT_NO_OPENGL
    {
        ca = qobject_cast< QwtPlotCanvas* >( canvas() );
        if (ca == nullptr) {
            pretty_pri("disabling \033[1;33mopenGL\e[0m");
            ca = new QwtPlotCanvas();
        }
    }
    return ca;
}

void QuPlotBase::setOpenGL(bool openGL) {
    if(openGL != isOpenGL()) {
        QWidget* pcanvas = createCanvas(openGL);
        m_canvas_conf(pcanvas);
        setCanvas(pcanvas);
        m_align_scales(); // 2.1, inspired by refreshtest example
        QuPlotZoomComponent* z = static_cast<QuPlotZoomComponent *>(d->components_map.value("zoom"));
        if(z)
            z->canvasChanged(this);
    }
}

void QuPlotBase::m_canvas_conf(QWidget* canvasw) {
    QwtPlotCanvas *ca = qobject_cast<QwtPlotCanvas *>(canvasw);
    QwtPlotOpenGLCanvas *oca = nullptr;
    if(ca) {
        ca->setFrameStyle( QFrame::Box | QFrame::Plain );
        ca->setLineWidth( 1 );
        ca->setPalette( Qt::white );
    } else if((oca = qobject_cast<QwtPlotOpenGLCanvas *>(canvasw)) != nullptr) {
        oca->setFrameStyle( QFrame::Box | QFrame::Plain );
        oca->setLineWidth( 1 );
        oca->setPalette( QColor("WhiteSmoke"));
    }
}

void QuPlotBase::setXTopAxisAutoscaleEnabled(bool autoscale) {
    d->axes->setAutoscale(QwtPlot::xTop, autoscale);
    if(!autoscale)
        d->axes->setBounds(this, QwtPlot::xTop, axisScaleDiv(QwtPlot::xTop).lowerBound(), axisScaleDiv(QwtPlot::xTop).upperBound());
    replot();
}

void QuPlotBase::setYRightAxisAutoscaleEnabled(bool autoscale) {
    d->axes->setAutoscale(QwtPlot::yRight, autoscale);
    if(!autoscale)
        d->axes->setBounds(this, QwtPlot::yRight, axisScaleDiv(QwtPlot::yRight).lowerBound(), axisScaleDiv(QwtPlot::yRight).upperBound());
    replot();
}

double QuPlotBase::yUpperBound(QwtPlot::Axis axis) {
    return axisScaleDiv(axis).upperBound();
}

double QuPlotBase::yLowerBound(QwtPlot::Axis axis) {
    return axisScaleDiv(axis).lowerBound();
}

/*
 * \brief Disable autoscale and set y lower bound
 */
void QuPlotBase::setYLowerBound(double l) {
    d->axes->setManualBounds(this, QwtPlot::yLeft, l, axisScaleDiv(QwtPlot::yLeft).upperBound());
    replot();
}

/*
 * \brief Disable autoscale and set x upper bound
 */
void QuPlotBase::setYUpperBound(double u) {
    d->axes->setManualBounds(this, QwtPlot::yLeft, axisScaleDiv(QwtPlot::yLeft).lowerBound(), u);
    replot();
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
void QuPlotBase::setXLowerBound(double l) {
    d->axes->setManualBounds(this, QwtPlot::xBottom, l, axisScaleDiv(QwtPlot::xBottom).upperBound());
    replot();
}

/*
 * \brief Disable autoscale and set x upper bound
 */
void QuPlotBase::setXUpperBound(double u) {
    d->axes->setManualBounds(this, QwtPlot::xBottom, axisScaleDiv(QwtPlot::xBottom).lowerBound(), u);
    replot();
}

void QuPlotBase::setXAutoscaleMargin(double adj) {
    d->axes->setAutoscaleMargin(QwtPlot::xBottom, adj);
}

/*! \brief add some extra upper bound to the specified axis to optimize replot operations when the
 *         refresh rate is fast.
 *
 * @param axisId the axis id, one of QwtPlot::xBottom, QwtPlot::xTop, QwtPlot::yLeft, QwtPlot::yRight
 * @param e the extra bound to add to the scale, percentage expressed from 0 to 1
 */
void QuPlotBase::setUpperBoundExtra(int axisId, double e){
    d->axes->setUpperBoundExtra(axisId, e);
}

double QuPlotBase::upperBoundExtra(int axisId) const {
    return d->axes->upperBoundExtra(axisId);
}

double QuPlotBase::xAutoscaleMargin() {
    return d->axes->autoscaleMargin(QwtPlot::xBottom);
}

void QuPlotBase::setYAutoscaleAdjustment(double a) {
    d->axes->setAutoscaleMargin(QwtPlot::yLeft, a);
}

double QuPlotBase::yAutoscaleAdjustment() {
    return d->axes->autoscaleMargin(QwtPlot::yLeft);
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

QuPlotAxesComponent *QuPlotBase::axes_c() const {
    return d->axes;
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
    QuPlotZoomComponent *zoom_c = static_cast<QuPlotZoomComponent *>(d->components_map.value("zoom"));
    if(!disable && !zoom_c) {
        zoom_c = new QuPlotZoomComponent(this);
        zoom_c->attachToPlot(this);
        zoom_c->connectToPlot(this);
        d->components_map.insert(zoom_c->name(), zoom_c);
        d->zoom = zoom_c;
    }
    else if(zoom_c) {
        d->components_map.remove("zoom");
        d->zoom = nullptr;
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
    if(d->marker) {
        d->marker->hide();
        replot();
    }
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

void QuPlotBase::plotZoomed(const QRectF&) {
    if(d->marker && d->marker->isVisible()) {
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
    pretty_pri("show marker called");
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
    if(d->marker && closestCurve && closestPointIdx > -1 && d->marker->isVisible()) {
        d->marker->update(this, closestCurve, closestPointIdx);
    }
}

void QuPlotBase::print() {
    static_cast<QuPlotContextMenuComponent *>(d->components_map.value("context_menu"))->print(this);
}

void QuPlotBase::snapshot(){
    static_cast<QuPlotContextMenuComponent *>(d->components_map.value("context_menu"))->snapshot(this);
}

void QuPlotBase::copyImage(){
    static_cast<QuPlotContextMenuComponent *>(d->components_map.value("context_menu"))->copyImage(this);
}

void QuPlotBase::saveData(){
    static_cast<QuPlotContextMenuComponent *>(d->components_map.value("context_menu"))->saveData(this);
}

void QuPlotBase::configurePlot(){
    static_cast<QuPlotContextMenuComponent *>(d->components_map.value("context_menu"))->configurePlot(this);
}

QDialog *QuPlotBase::createConfigureDialog()
{
    EPlotConfigurationDialog  *d = new EPlotConfigurationDialog(this);
    return d;
}
