#ifndef QUTRENDPLOT_H
#define QUTRENDPLOT_H

#include <esimplelabel.h>
#include <cudatalistener.h>
#include <QtCharts/QChartView>
#include "callout.h"

class QuTrendPlotPrivate;
class QuPlotCommon;
class Cumbia;
class CuControlsReaderFactoryI;

using namespace QtCharts;

class QuTrendPlot : public QChartView, public CuDataListener, public CalloutListener
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
    Q_PROPERTY(bool autoscaleEnabled READ autoscaleEnabled WRITE setAutoscaleEnabled NOTIFY autoscaleChanged DESIGNABLE true)
    Q_PROPERTY(double autoscale_adjust READ autoscaleAdjust WRITE setAutoscaleAdjust NOTIFY autoscaleAdjustChanged DESIGNABLE true)

public:
    QuTrendPlot(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    virtual ~QuTrendPlot();

    QString source() const;

    QStringList sources() const;

    bool autoscaleEnabled() const;

    double autoscaleAdjust() const;

public slots:
    void setSource(const QString& s);

    void setSources(const QStringList& l);

//    void updateScene(const QList<QRectF> &rects);

    void setAutoscaleEnabled(bool a);

    void setAutoscaleAdjust(double a);

    void moveToAxis(const QString& curve_name, const QString& axis);

private Q_SLOTS:
    void handleClickedPoint(const QPointF &point);

    void plotAreaChanged(const QRectF &r);

signals:
    void newData(const CuData&);

    void autoscaleChanged(bool);

    void autoscaleAdjustChanged(double);

protected:

    void mousePressEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

    void mouseDoubleClickEvent(QMouseEvent *e);

private:
    QuTrendPlotPrivate *d;
    QuPlotCommon *plot_common;

    // CuTangoListener interface
public:
    void onUpdate(const CuData &d);

    // CalloutListener interface
public:
    void onCalloutClicked(const QString &title, const QPointF &xy);
};

#endif // QUTLABEL_H
