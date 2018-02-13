#ifndef QUWIDGETUPDATESTRATEGYINTERFACE_H
#define QUWIDGETUPDATESTRATEGYINTERFACE_H

class QWidget;
class CuData;

/*! \brief strategy pattern: this class provides a strategy to customise the update
 *         of graphs.
 *
 * For example, the qumbia-tango-controls module implements QuWidgetUpdateStrategyI in the
 * QuTPlotUpdateStrategy class. QuTPlotUpdateStrategy::update can set an appropriate background
 * color if the reading is not valid and set minimum and maximum values according to
 * the "min_value" and "max_value" configuration parameters.
 *
 */
class QuWidgetUpdateStrategyI
{
public:
    virtual ~QuWidgetUpdateStrategyI() {}

    virtual void update(const CuData& data, QWidget *widget) = 0;
};

#endif // QUWIDGETUPDATESTRATEGYINTERFACE_H
