#ifndef QUPLOTMARKERCOMPONENT_H
#define QUPLOTMARKERCOMPONENT_H

#include <quplotcomponent.h>
#include <qwt_plot_item.h>
#include <qwt_plot_curve.h>
#include <qwt_text.h>

class QwtPlotPicker;
class QwtPlotMarker;
class QuPlotMarkerComponentPrivate;

class QuPlotMarkerFormat_I {
public:
    virtual ~QuPlotMarkerFormat_I() {}

    /*! \brief reimplement this method to provide custom text for the marker
     *
     * @param plot a pointer to the QuPlotBase in use
     * @param curves the curves crossing the closest curve at (x,y) corresponding to index
     *        The first item in the list is the curve *closest to the click position*.
     * @param index the index of the curve closest to the click position
     * @param x shall contain the section of the text involving the x value
     * @param y shall contain the section of the text involving the y value
     *
     * \return the string to display on the marker
     *
     * \note
     * The curve closest to the click position is stored as first item in the curve
     * list.
     */
    virtual QString format(const QuPlotBase *plot,
                   QList<QwtPlotCurve *>curves,
                   const int index, QString& x, QString& y) const = 0;
};

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
    virtual ~QuPlotMarkerComponent();

    // QuPlotComponent interface
public:
    QString name() const;

    bool update(const QuPlotBase *plot);

    QString xLabel() const;
    QString yLabel() const;
    QString label() const;

    void setFormatter(QuPlotMarkerFormat_I *pmf);
    QuPlotMarkerFormat_I *formatter() const;

    void setLabel(const QwtText &text);

    virtual void update(const QuPlotBase* plot, QwtPlotCurve *closestC, int closestPointIdx);

    void hide();
    void show();

    bool isVisible() const;

    QwtPlotCurve *currentClosestCurve() const;
    int currentClosestPoint() const;

    Arrow* getArrow() const;
    QwtPlotMarker *qwtPlotMarker() const;
    QwtPlotPicker *qwtPlotPicker() const;


private:
    void init(QuPlotBase *plot);
    QuPlotMarkerComponentPrivate *d;

    // QuPlotComponent interface
public:
    void attachToPlot(QuPlotBase *plot);
    void connectToPlot(QuPlotBase *plot);
    virtual QString markerText(const QuPlotBase *plot, const QwtPlotCurve *closest_curve, const int index);
    QList<QwtPlotCurve *> intersectingCurves(const QuPlotBase *plot, const double x, const double y, const QwtPlotCurve *curve);
};

#endif // QUPLOTMARKERCOMPONENT_H
