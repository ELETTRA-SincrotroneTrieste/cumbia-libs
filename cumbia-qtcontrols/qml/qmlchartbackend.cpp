#include "qmlchartbackend.h"
#include <qmlreaderbackend.h>
#include <cucontext.h>
#include <quplotcommon.h>
#include <cumbiapool_o.h>
#include <culinkstats.h>

#include <QMap>
#include <QtCharts/QXYSeries>
#include <QtDebug>

class QmlChartBackendPrivate
{
public:
    QuPlotCommon *plot_common;
    bool read_ok;
    bool auto_configure;
    QStringList sources;
    QMap<QString, QVector<QPointF> > data;

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
    d->xm = d->ym = 0.0;
    d->xM = d->yM = 1000.0;
    d->refcnt = 0;
    d->is_scalar_trend = false;
}

void QmlChartBackend::init(CumbiaPool_O *poo_o)
{
    d->plot_common = new QuPlotCommon(poo_o->getPool(), poo_o->getFactory());
    if(!d->sources.isEmpty()) {
        printf("QmlChartBackend: setting sources size %d\n", d->sources.size());
        setSources(d->sources);
    }
}

CuContext *QmlChartBackend::getContext() const
{
    return d->plot_common->getContext();
}

void QmlChartBackend::m_configure(const CuData &da)
{
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
    try {
    if(da.containsKey("min"))
        v = QVariant(std::stod(da["min"].toString()));
    //...
    if(da.containsKey("max"))
        v = QVariant(std::stod(da["max"].toString()));
    //...
    }
    catch (const std::invalid_argument& ia) {

    }
}

void QmlChartBackend::onUpdate(const CuData &da)
{
    bool readOk = !da["err"].toBool();
    if(readOk != d->read_ok) {
        d->read_ok = readOk;
        emit readOkChanged();
    }
    QString src = QString::fromStdString(da["src"].toString());

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
        QVector <QPointF> &dat = d->data[src];
        qreal x, y;
        bool time_scale = da.containsKey("timestamp_ms");
        if(time_scale) {
            x = static_cast<qint64>(da["timestamp_ms"].toLongInt());
            d->xM = x;
            if(!d->t1.isValid()) {
                d->t1 = QDateTime::fromMSecsSinceEpoch(x);
                d->xm = x;
                emit t1Changed();
                emit xMinChanged();
            }
            else {
                    d->t2 = QDateTime::fromMSecsSinceEpoch(x);
                    x = d->t2.toMSecsSinceEpoch();
                    emit xMaxChanged();
                    emit t2Changed();
            }
        }
        else {
            x = dat.size();
        }

        v.to(y);

        emit dataChanged(src, x, y);
    }
    else  if(d->read_ok && v.isValid() && v.getFormat() == CuVariant::Vector) {
        std::vector<double> yv = v.toDoubleVector();
        QVector<QPointF> &qvd = d->data[src];
        if(qvd.size() != (int) yv.size()) {
            qvd.clear();
            qvd.resize(yv.size());
        }
        for(size_t i = 0; i < yv.size(); i++) {
            // simple for now
            if(d->yM < yv[i] || i == 0) {
                d->yM = yv[i];
                emit yMaxChanged();
            }
            if(d->ym > yv[i] || i == 0) {
                d->ym = yv[i];
                emit yMinChanged();
            }
            qvd[i] = QPointF(static_cast<double>(i), yv[i]);
        }


        if(d->xm != 0) {
            d->xm = 0;
            emit xMinChanged();
        }
        if(d->xM != yv.size() - 1) {
            d->xM = yv.size() -1;
            emit xMaxChanged();
        }

        emit spectrumPointsChanged(src);
    }
}

void QmlChartBackend::setSources(const QStringList &l)
{
    printf("QmlChartBackend.setSources: size %d\n", l.size());
    if(d->plot_common) {
        unsetSources();
        d->plot_common->setSources(l, this);
    }
    d->sources = l;
    if(d->plot_common) {
        emit sourcesChanged(d->plot_common->sources());
    }
}

void QmlChartBackend::replaceData(QtCharts::QAbstractSeries *series)
{
    if (series) {
        QtCharts::QXYSeries *xySeries = static_cast<QtCharts::QXYSeries *>(series);
        if(xySeries) {
            printf("\e[1;32m!! its series!!!\e[0m\n");
            const QVector<QPointF> &points = d->data[series->name()];
            xySeries->replace(points);
        }
    }
}

QStringList QmlChartBackend::sources() const
{
    if(!d->plot_common)
        return QStringList();
    return d->plot_common->sources();
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

void QmlChartBackend::unsetSources()
{
    if(d->plot_common && d->plot_common->getContext())
        d->plot_common->getContext()->disposeReader();
    // remove curves
}

void QmlChartBackend::setXMin(qreal xm)
{
    d->xm = xm;
}

void QmlChartBackend::setXMax(qreal xM)
{
    d->xM = xM;
}

void QmlChartBackend::setYMin(qreal ym)
{
    d->ym = ym;
}

void QmlChartBackend::setYMax(qreal yM)
{
    d->yM = yM;
}
