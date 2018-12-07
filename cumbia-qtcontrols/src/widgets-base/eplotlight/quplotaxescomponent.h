#ifndef QUPLOTAXESCOMPONENT_H
#define QUPLOTAXESCOMPONENT_H

#include <quplotcomponent.h>

class QuPlotAxesComponentPrivate;

/** \brief Plot component that manages the axes
 *
 * \ingroup plots
 *
 *
*/
class QuPlotAxesComponent : public QuPlotComponent
{
public:
    enum ScaleMode { AutoScale = 0x0, SemiAutoScale = 0x01, Manual = 0x02 };

    QuPlotAxesComponent(QuPlotBase *plot);

    ~QuPlotAxesComponent();

    // QuPlotComponent interface
public:
    QString name() const;
    void attachToPlot(QuPlotBase *plot);
    void connectToPlot(QuPlotBase *plot);

    void setBounds(QuPlotBase *plot, int axisId, double lb, double ub);

    void setManualBounds(QuPlotBase *plot, int axisId, double lb, double ub);

    void setDefaultBounds(QuPlotBase *plot, int axisId, double lb, double ub);

    void restoreDefaultBounds(QuPlotBase *plot, int axisId);

    bool applyScaleFromCurveBounds(QuPlotBase *plot, int axisId);

    bool setBoundsFromCurves(double min, double max, int axisId);

    bool getBoundsFromCurves(const QuPlotBase *plot,
                             double* xmin,
                             double *xmax,
                             double *ymin,
                             double *ymax,
                             bool calc_x, bool calc_y) const;

    bool autoscale(int axisId) const;

    ScaleMode scaleMode(int axisId) const;

    void setAutoscale(int axisId, bool a);

    double autoscaleMargin(int axisId) const;

    void setAutoscaleMargin(int axisId, double m);

    double lowerBoundFromCurves(int axisId) const;

    double upperBoundFromCurves(int axisId) const;

    double defaultLowerBound(int axisId) const;

    double defaultUpperBound(int axisId) const;

    void setUpperBoundExtra(int axisId, double ube);

    double upperBoundExtra(int axisId) const;

private:
    QuPlotAxesComponentPrivate *d;
};

#endif // QUPLOTAXESCOMPONENT_H
