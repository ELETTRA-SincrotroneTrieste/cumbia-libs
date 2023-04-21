#ifndef QUPLOTCONFIGURATOR_H
#define QUPLOTCONFIGURATOR_H

class QuPlotCurve;

#include <QString>

class QuPlotConfigurator
{
public:
    QuPlotConfigurator();

    bool hasCustomConf(const QuPlotCurve *c) const;
    void save(const QuPlotCurve *c);
    void configure(QuPlotCurve *c, int curves_cnt);

private:
    QString m_get_id(const QuPlotCurve *c) const;
};

#endif // QUPLOTCONFIGURATOR_H
