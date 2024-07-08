#ifndef QUPLOTZOOMCOMPONENT_H
#define QUPLOTZOOMCOMPONENT_H

#include <quplotcomponent.h>
#include <scrollzoomer.h>
#include <qwt_plot_zoomer.h>

class QuPlotBase;

/**
 * \brief QuPlot component to zoom the plot.
 *
 * \ingroup plots
 */
class QuPlotZoomComponent : public QuPlotComponent
{
public:
    QuPlotZoomComponent(QuPlotBase *plot);

    virtual ~QuPlotZoomComponent();

    bool inZoom() const;

    void changeRect(int axisId, double delta_lower, double delta_upper);

    void setZoomBase(bool do_replot = false);

    void setZoomBase(const QRectF& r);

    QRectF zoomBase() const;

    QRectF zoomRect() const;

private:
    ScrollZoomer *m_zoomer;


    // QuPlotComponent interface
public:
    QString name() const;

private:
    void init(QuPlotBase *plot);

    // QuPlotComponent interface
public:
    void attachToPlot(QuPlotBase *plot);
    void connectToPlot(QuPlotBase *plot);
};

#endif // ZOOMCOMPONENT_H
