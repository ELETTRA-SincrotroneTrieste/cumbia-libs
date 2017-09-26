#ifndef QUTPLOTUPDATESTRATEGY_H
#define QUTPLOTUPDATESTRATEGY_H

#include <quwidgetupdatestrategy_i.h>

class QuEpicsPlotUpdateStrategyPrivate;
class CuVariant;

class QuEpicsPlotUpdateStrategy : public QuWidgetUpdateStrategyI
{
public:
    QuEpicsPlotUpdateStrategy(bool auto_configure = true);

    virtual ~QuEpicsPlotUpdateStrategy();

    // QuWidgetUpdateStrategyInterface interface
public:
    void update(const CuData &data, QWidget *widget);

private:

    void m_configure(const CuData &da, QWidget *widget);

    QuEpicsPlotUpdateStrategyPrivate *d;
};

#endif // QUTPLOTUPDATESTRATEGY_H
