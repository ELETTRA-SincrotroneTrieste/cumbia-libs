#ifndef QUTPLOTUPDATESTRATEGY_H
#define QUTPLOTUPDATESTRATEGY_H

#include <quwidgetupdatestrategy_i.h>

class QuEpicsPlotUpdateStrategyPrivatevate;
class CuVariant;

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

    QuEpicsPlotUpdateStrategyPrivatevate *d;
};

#endif // QUTPLOTUPDATESTRATEGY_H
