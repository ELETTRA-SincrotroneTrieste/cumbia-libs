#include "qmlchartbackend.h"
#include <qmlreaderbackend.h>
#include <cucontext.h>
#include <quplotcommon.h>
#include <cumbiapool_o.h>
#include <culinkstats.h>

#include <QMap>
#include <QtCharts/QXYSeries>
#include <QtDebug>
#include <QThread> // for current thread

class QmlChartBackendPrivate
{
public:
    QuPlotCommon *plot_common;
    bool read_ok;
    bool auto_configure;
    bool x_autoscale, y_autoscale;
    unsigned char configure_cnt;
    int bufsiz;
    unsigned long refresh_cnt;
    int period;
    QStringList sources;

    // the following stores spectrum data. It is updated on the C++ side
    // as in the Qt Qml oscilloscope example, using the XYSeries replace
    // method
    QMap<QString, QVector<QPointF> > data;

    // for each curve with a given name, its minimum and maximum values
    // are stored within minmax_map as QPointF.x (min) and QPointF.y (max)
    QMap<QString, QPointF> minmax_map;

    // if a source minimum and maximum values are known (e.g. from a database)
    // it is assumed that the curve values fall within that defined interval
    // and no minimum and maximum calculation is done when new data arrives for
    // the curves in this list
    QStringList autoconfigured_curves;

    QDateTime t1, t2;
    double xm, xM;
    double ym, yM;
    double refcnt;
    bool is_scalar_trend;
};

QmlChartBackend::QmlChartBackend(QObject *parent) : QObject(parent)
{
    d = new QmlChartBackendPrivate;
    d->plot_common = NULL;
    d->read_ok = false;
    d->auto_configure = true;
    d->x_autoscale = d->y_autoscale = true;
    d->xm = d->ym = 0.0;
    d->xM = d->yM = 1000.0;
    d->refcnt = 0;
    d->configure_cnt = 0;
    d->is_scalar_trend = false;
    d->bufsiz = -1;
    d->refresh_cnt = 0;
    d->period = 1000;
}

QmlChartBackend::~QmlChartBackend()
{
    pdelete("~QmlChartBackend %p", this);
    if(d->plot_common)
        delete d->plot_common; // deletes its context
    delete d;
}

void QmlChartBackend::init(CumbiaPool_O *poo_o)
{
    qDebug() << __FUNCTION__ << "INIT DAFUQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ";
    d->plot_common = new QuPlotCommon(poo_o->getPool(), poo_o->getFactory());
    //    if(!d->sources.isEmpty()) {
    //        m_setSources(d->sources);
    //    }
}

CuContext *QmlChartBackend::getContext() const
{
    return d->plot_common->getContext();
}

void QmlChartBackend::m_configure(const CuData &da)
{
    printf("QmlChartBackend::m_configure configuring QmlChart with %s\n", da.toString().c_str());
    QString src = QString::fromStdString(da["src"].toString());
    d->data[src] = QVector<QPointF>();

    // spectrum or scalar trend over time?
    if(da.containsKey("data_format_str")) {
        bool scalar_trend = da["data_format_str"].toString() == "scalar";
        if(scalar_trend != d->is_scalar_trend) {
            d->is_scalar_trend = scalar_trend;
        }
    }
    emit scalarTrendChanged(d->is_scalar_trend);

    QVariant v;
    char* endptr;
    double min = 0.0,  max = 0.0;
    try {
        if(da.containsKey("min")) {
            printf("1: %s->%f\n", da["min"].toString().c_str(), strtod(da["min"].toString().c_str(),  &endptr));
            v = QVariant(strtod(da["min"].toString().c_str(), &endptr));
            min = v.toDouble();
            if(!d->configure_cnt || min < d->ym) {
                printf("2\n");
                d->ym = min;
                emit yMinChanged();
            }
        }
        if(da.containsKey("max")) {
            printf("3: %s->%f\n", da["min"].toString().c_str(), strtod(da["max"].toString().c_str(),  &endptr));
            v = QVariant(strtod(da["max"].toString().c_str(),  &endptr));
            max = v.toDouble();
            if(!d->configure_cnt || max > d->yM) {
                d->yM = max;
                emit yMaxChanged();
                printf("4\n");
            }
        }
    }
    catch (const std::invalid_argument& ia) {
        perr("QmlChartBackend.m_configure: double conversion failed: %s", ia.what());
    }

    printf("QmlChartBackend min %f max %f\n", min, max);
    if(min != max) {
        d->autoconfigured_curves << src;
        d->configure_cnt++;
        d->minmax_map[src] = QPointF(min, max);
    }
}

void QmlChartBackend::m_setSources(const QStringList &l)
{
    d->sources = l;
    if(d->plot_common) {
        unsetSources();
        CuData periodConf("period", d->period);
        d->plot_common->getContext()->setOptions(periodConf);
        d->plot_common->setSources(l, this);
    }
    if(d->plot_common) {
        emit sourcesChanged(d->plot_common->sources());
    }
}

void QmlChartBackend::onUpdate(const CuData &da)
{
    printf("\e[0;32mQmlChartBackend.onUpdate DATA %s\e[0m\n", da.toString().c_str());
    bool readOk = !da["err"].toBool();
    if(readOk != d->read_ok) {
        d->read_ok = readOk;
        emit readOkChanged();
    }
    QString src = QString::fromStdString(da["src"].toString());
    QVector<QPointF> &qvd = d->data[src];

    // update link statistics
    CuLinkStats *link_s = d->plot_common->getContext()->getLinkStats();
    link_s->addOperation();
    if(!d->read_ok)
        link_s->addError(da["msg"].toString());

    // configure triggers replot at the end but should not be too expensive
    // to do it once here at configuration time and once more from appendData
    if(d->read_ok && d->auto_configure && da["type"].toString() == "property")
        m_configure(da);

    const CuVariant &v = da["value"];
    if(d->read_ok && v.isValid() && v.getFormat() == CuVariant::Scalar)
    {
        qreal x, y;
        bool time_scale = da.containsKey("timestamp_ms");
        if(time_scale) {
            x = static_cast<qint64>(da["timestamp_ms"].toLongInt());
            d->xM = x;
            if(d->x_autoscale && !d->t1.isValid()) {
                d->t1 = QDateTime::fromMSecsSinceEpoch(x);
                d->xm = x;
                emit t1Changed();
                emit xMinChanged();
            }
            else if(d->x_autoscale) {
                d->t2 = QDateTime::fromMSecsSinceEpoch(x);
                x = d->t2.toMSecsSinceEpoch();
                emit xMaxChanged();
                emit t2Changed();
            }
        }
        else { // no time scale
            x = qvd.size();
        }

        v.to(y);

        qvd.append(QPointF(x, y));

        if(d->y_autoscale && !d->autoconfigured_curves.contains(src)
                && (d->bufsiz < 0 || qvd.size() < d->bufsiz)) {
            m_update_curve_y_bounds(src, y);
            m_find_absolute_bounds_and_notify();
        }
        emit newPoint(src);
    }
    else  if(d->read_ok && v.isValid() && v.getFormat() == CuVariant::Vector)
    {
        // vector
        //
        // 1 resize and create x values only if size of data changes x: [0, ..., data_size]
        // 2 calculate max and min values of new curve and place min and max into d->minmax_map
        // 3 within d->minmax_map, find the absolute minimum and maximum amongst all curves
        // 4 update d->ym and d->yM and notify bounds changed if necessary
        //
        std::vector<double> yv = v.toDoubleVector();
        if(qvd.size() != (int) yv.size()) {
            qvd.clear();
            qvd.resize(yv.size());
        }
        for(size_t i = 0; i < yv.size(); i++) {
            qvd[i] = QPointF(static_cast<double>(i), yv[i]);
        }

        // calculate min and max among the values of yv only if the minimum and maximum values
        // for that curve have not been auto configured.
        if(d->y_autoscale && !d->autoconfigured_curves.contains(src)) {
            m_update_curve_y_bounds(src, qvd);
            m_find_absolute_bounds_and_notify();
        }

        // need to update x bounds? In spectrum mode, only if data size changed
        if(d->x_autoscale && d->xm != 0) {
            d->xm = 0;
            emit xMinChanged();
        }
        if(d->x_autoscale && d->xM != yv.size() - 1) {
            d->xM = yv.size() -1;
            emit xMaxChanged();
        }

        emit newSpectrum(src);
    }
    d->refresh_cnt++;
//    int qdebug_rate = 1000 / d->period;
//    if(d->refresh_cnt % qdebug_rate == 0)
//        qDebug() << "QmlChartBackend" <<this<< d->refresh_cnt << da["src"].toString().c_str() << "thread"  << QThread::currentThread();
}

void QmlChartBackend::m_update_curve_y_bounds(const QString& src, const QVector<QPointF> &pts) {
//    printf("\e[1;32mrecalc y bounds needed for %s...\n", qstoc(src));
    // calculate minimum and maximum amongst yv values
    // and update d->minmax_map for that src
    for(int i = 0; i < pts.size(); i++) {
        if(!d->minmax_map.contains(src)){
            d->minmax_map[src] =  QPointF(pts[i].y(), pts[i].y());
        }
        else {
            double &min = d->minmax_map[src].rx();
            double &max = d->minmax_map[src].ry();
            if(max < pts[i].y()) {
                max = pts[i].y(); // reference update
            }
            else if(min > pts[i].y()) {
                min = pts[i].y(); // reference update
            }
        }
    }

//    printf("... done recalc\e[0m\n");
}

// see if y is smaller than minimum for curve src or greater than maximum.
// If so, update bounds for curve src within d->minmax_map
//
void QmlChartBackend::m_update_curve_y_bounds(const QString &src, const double &y)
{
    printf("\e[1;33mrecalc y bounds needed for %s (append mod)...\n", qstoc(src));
    if(!d->minmax_map.contains(src)){
        d->minmax_map[src] =  QPointF(y, y);
    }
    else {
        double &ymax = d->minmax_map[src].ry();
        double &ymin = d->minmax_map[src].rx();
        if(y > ymax)
            ymax = y;
        else if(y < ymin)
            ymin = y;
    }
}

bool QmlChartBackend::m_find_absolute_bounds_and_notify()
{
    bool scales_changed = false;
    // see if new min and max fall outside the smallest min and the biggest max
    // among all other curves' min and max values
    const QList<QPointF> &pl = d->minmax_map.values();
    double ymin, ymax;
    for(int i = 0; i < pl.size(); i++) {
        const double& mi = pl[i].x();
        const double& ma = pl[i].y();
        if(ymin > mi || i == 0)
            ymin = mi;
        if(ymax < ma || i == 0)
            ymax = ma;
    }

    // update min and max and notify if necessary
    if(d->ym != ymin) {
        scales_changed = true;
        d->ym = ymin;
        printf("recalc needed and new min is %f\n", d->ym);
        emit yMinChanged();
    }
    if(d->yM != ymax) {
        scales_changed = true;
        d->yM = ymax;
        printf("recalc needed and new MAX is %f\n", d->yM);
        emit yMaxChanged();
    }
    return scales_changed;
}

void QmlChartBackend::setSources(const QString &sl)
{
    QStringList l = sl.split(";", QString::SkipEmptyParts);
    d->sources = l;
}

void QmlChartBackend::replaceData(QtCharts::QAbstractSeries *series)
{
    if (series) {
        QtCharts::QXYSeries *xySeries = static_cast<QtCharts::QXYSeries *>(series);
        if(xySeries) {
            const QVector<QPointF> &points = d->data[series->name()];
            xySeries->replace(points);
        }
    }
}

void QmlChartBackend::appendPoint(QtCharts::QAbstractSeries *series)
{
    if (series) {
        QtCharts::QXYSeries *xySeries = static_cast<QtCharts::QXYSeries *>(series);
        if(xySeries) {
            const QVector<QPointF> &points = d->data[series->name()];
            xySeries->append(points[points.size() - 1]);
        }
    }
}

QString QmlChartBackend::sources() const
{
    if(!d->plot_common)
        return QString();
    return d->plot_common->sources().join(";");
}

bool QmlChartBackend::readOk() const
{
    return d->read_ok;
}

qreal QmlChartBackend::xMin() const
{
    return d->xm;
}

qreal QmlChartBackend::xMax() const
{
    return d->xM;
}

qreal QmlChartBackend::yMin() const
{
    return d->ym;
}

qreal QmlChartBackend::yMax() const
{
    return d->yM;
}

bool QmlChartBackend::timeScale() const
{
    return true;
}

bool QmlChartBackend::scalarTrend() const
{
    return d->is_scalar_trend;
}

bool QmlChartBackend::xAutoscale() const
{
    return d->x_autoscale;
}

bool QmlChartBackend::yAutoscale() const
{
    return d->y_autoscale;
}

int QmlChartBackend::period() const
{
    return d->period;
}

int QmlChartBackend::bufsiz() const
{
    return d->bufsiz;
}

QDateTime QmlChartBackend::t1() const
{
    if(!d->t1.isValid())
        return QDateTime::currentDateTime();
    return d->t1;
}

QDateTime QmlChartBackend::t2() const
{
    if(!d->t2.isValid())
        return QDateTime::currentDateTime().addSecs(5);
    return d->t2;
}

void QmlChartBackend::suspend()
{
    qDebug() << __FUNCTION__ << "SUSPENDING!!!";
    QStringList srcs = d->sources;
    unsetSources();
    d->sources = srcs; // preserve sources for resume
}

void QmlChartBackend::start() {
    qDebug() << __FUNCTION__ << "STARTINNG!!!";
    m_setSources(d->sources);
}

void QmlChartBackend::unsetSources()
{
    if(d->plot_common && d->plot_common->getContext())
        d->plot_common->getContext()->disposeReader();
    // remove curves
}

void QmlChartBackend::setXMin(qreal xm)
{
    if(d->xm  != xm) {
        d->xm = xm;
        emit xMinChanged();
    }
    if(d->x_autoscale) {
        d->x_autoscale = false;
        emit xAutoscaleChanged();
    }
}

void QmlChartBackend::setXMax(qreal xM)
{
    if(d->xM != xM) {
        d->xM = xM;
        emit xMaxChanged();
    }
    if(d->x_autoscale) {
        d->x_autoscale = false;
        emit xAutoscaleChanged();
    }
}

void QmlChartBackend::setYMin(qreal ym)
{
    if(d->ym != ym) {
        d->ym = ym;
        emit yMinChanged();
    }
    if(d->y_autoscale) {
        d->y_autoscale = false;
        emit yAutoscaleChanged();
    }
}

void QmlChartBackend::setYMax(qreal yM)
{
    if(d->yM != yM) {
        d->yM = yM;
        emit yMaxChanged();
    }
    if(d->y_autoscale) {
        d->y_autoscale = false;
        emit yAutoscaleChanged();
    }
}

void QmlChartBackend::setXAutoscale(bool a)
{
    if(a != d->x_autoscale) {
        d->x_autoscale = a;
        emit xAutoscaleChanged();
    }
}

void QmlChartBackend::setYAutoscale(bool a)
{
    printf("setYAutoscale %d\n", a);
    if(a != d->y_autoscale) {
        d->y_autoscale = a;
        emit yAutoscaleChanged();
    }
}

void QmlChartBackend::setBufsiz(int b)
{
    if(b != d->bufsiz) {
        d->bufsiz = b;
        emit bufsizChanged();
    }
}

void QmlChartBackend::setPeriod(int millis)
{
    if(d->period != millis) {
        d->period = millis;
        emit periodChanged();
    }
}
