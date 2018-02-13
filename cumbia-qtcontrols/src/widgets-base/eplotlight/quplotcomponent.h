#ifndef QUPLOTCOMPONENT_H
#define QUPLOTCOMPONENT_H

#include <QString>

class  QuPlotBase;

/*! \brief Interface defining a plot component. Operations can be delegated to
 *         plot components
 *
 * \par Usage
 * QuPlotBase uses QuPlotComponent implementations to perform zooming, manage axes,
 * drawing markers on the canvas, create the contextual menu.
 *
 * Each QuPlotComponent must have a unique name and a couple of methods
 * to initialise QwtPlotItem items and create Qt signal/slot connections:
 *
 * \li attachToPlot
 * \li connectToPlot
 */
class QuPlotComponent
{
public:

    virtual ~QuPlotComponent() {}

    virtual QString name() const = 0;

    /*! \brief in this method you would call QwtPlot::attach for the
     *         QwtPlotItem items in the component
     *
     * \par Example
     * See QuPlotMarkerComponent::attachToPlot
     */
    virtual void attachToPlot(QuPlotBase *plot) = 0;

    /*! \brief in this method you would create Qt signal/slot connections
     *
     * \par Example
     * See QuPlotMarkerComponent::connectToPlot and QuPlotZoomComponent::connectToPlot
     */
    virtual void connectToPlot(QuPlotBase *plot) = 0;
};

#endif // QUPLOTCOMPONENTI_H
