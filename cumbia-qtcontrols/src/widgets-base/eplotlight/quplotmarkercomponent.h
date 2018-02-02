#ifndef QUPLOTMARKERCOMPONENT_H
#define QUPLOTMARKERCOMPONENT_H

#include <quplotcomponent.h>
#include <qwt_plot_item.h>
#include <qwt_plot_curve.h>

class QwtPlotPicker;
class QwtPlotMarker;

/*! @private */
class Arrow: public QwtPlotItem
{
public:
    Arrow(const QwtText &title=QwtText()) : QwtPlotItem(title) {}
    void draw(QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRectF &) const;
    QPointF begin, end;
};

/**
 * \brief QuPlot component to draw markers on the canvas
 *
 * \ingroup plots
 */
class QuPlotMarkerComponent : public QuPlotComponent
{
public:
    QuPlotMarkerComponent(QuPlotBase *plot);

    // QuPlotComponent interface
public:
    QString name() const;

    bool update(const QuPlotBase *plot);

    void setLabel(const QwtText &text);

    void update(const QuPlotBase* plot, QwtPlotCurve *closestC, int closestPointIdx);

    void hide();

    void show();

    bool isVisible() const;

    QwtPlotCurve *currentClosestCurve() const;

    int currentClosestPoint() const;

private:
    void init(QuPlotBase *plot);

    QwtPlotMarker *m_marker;
    QwtPlotPicker *m_picker;
    Arrow *m_arrow;

    QwtPlotCurve *m_currentClosestCurve;
    int m_currentClosestPoint;

    // QuPlotComponent interface
public:
    void attachToPlot(QuPlotBase *plot);
    void connectToPlot(QuPlotBase *plot);

protected:

    QString markerText(const QuPlotBase *plot, const QwtPlotCurve *curve, const int index);

    QSet<QwtPlotCurve *> intersectingCurves(const QuPlotBase *plot, const double x, const double y, const QwtPlotCurve *curve);
};

#endif // QUPLOTMARKERCOMPONENT_H
