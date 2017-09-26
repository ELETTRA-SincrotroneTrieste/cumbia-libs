#ifndef QUPLOTCOMPONENT_H
#define QUPLOTCOMPONENT_H

#include <QString>

class  QuPlotBase;

class QuPlotComponent
{
public:

    virtual ~QuPlotComponent() {}

    virtual QString name() const = 0;

    virtual void attachToPlot(QuPlotBase *plot) = 0;

    virtual void connectToPlot(QuPlotBase *plot) = 0;
};

#endif // QUPLOTCOMPONENTI_H
