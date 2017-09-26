#ifndef QUPLOTAXESCOMPONENT_H
#define QUPLOTAXESCOMPONENT_H

#include <quplotcomponent.h>

class QuPlotAxesComponentPrivate;

class QuPlotAxesComponent : public QuPlotComponent
{
public:
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

    bool setBoundsFromCurves(const QuPlotBase *plot, int axisId);

    bool autoscale(int axisId) const;

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
