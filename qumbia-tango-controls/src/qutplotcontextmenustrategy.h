#ifndef QUTPLOTCONTEXTMENUSTRATEGY_H
#define QUTPLOTCONTEXTMENUSTRATEGY_H

#include <quplotbasecontextmenustrategy.h>
#include <QWidget>

class QDialog;
class QuPlotBase;
class CuData;

/*! \brief strategy pattern. Provide a contextual menu for tango plots
 *
 * This class implements the QuPlotBaseContextMenuStrategy::createMenu
 * virtual method offering a menu with an action to
 * execute a configuration dialog to change Tango specific settings.
 */
class QuTPlotContextMenuStrategy : public QuPlotBaseContextMenuStrategy
{
public:
    QuTPlotContextMenuStrategy(bool realtime = false);
    // QuWidgetContextMenuStrategyI interface

    QMenu *createMenu(QWidget *w);

private:
    bool m_realtime;

};

/*! \brief an object (QObject) that applies settings to a plot
 *
 * A pointer to a plot is passed in the constructor and the configure method
 * creates a dialog to change its settings.
 */
class QuTPlotSettings : public QObject
{
    Q_OBJECT
public:
    QuTPlotSettings(QuPlotBase *plot, bool realtime = false);

    virtual ~QuTPlotSettings();

public slots:
    void configure();

    void apply(const CuData& da);

private:
    QuPlotBase *m_plot;
    bool m_realtime;
};

#endif // QUTPLOTCONFIGURATION_H
