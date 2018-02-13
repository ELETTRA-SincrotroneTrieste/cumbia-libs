#ifndef QUTPLOTUPDATESTRATEGY_H
#define QUTPLOTUPDATESTRATEGY_H

#include <quwidgetupdatestrategy_i.h>

class QuTPlotUpdateStrategyPrivate;
class CuVariant;

/*! \brief strategy pattern. A class that configures and updates a Tango plot
 *         according to the properties of Tango attributes (minimum and maximum
 *         values)
 *
 * The update implementation configures the plot with the default Y lower and upper
 * bounds taken from the attribute properties (if the source is a Tango attribute).
 *
 * After the configuration phase, QuPlotBase::update is called in order to draw
 * the graph.
 */
class QuTPlotUpdateStrategy : public QuWidgetUpdateStrategyI
{
public:
    QuTPlotUpdateStrategy(bool auto_configure = true);

    virtual ~QuTPlotUpdateStrategy();

    // QuWidgetUpdateStrategyInterface interface
public:
    void update(const CuData &data, QWidget *widget);

private:

    void m_fillFromHistory(const CuVariant &x, const CuVariant &y);
    void m_configure(const CuData &da, QWidget *widget);

    QuTPlotUpdateStrategyPrivate *d;
};

#endif // QUTPLOTUPDATESTRATEGY_H
