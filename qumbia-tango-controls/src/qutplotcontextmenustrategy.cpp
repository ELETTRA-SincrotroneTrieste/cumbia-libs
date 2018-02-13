#include "qutplotcontextmenustrategy.h"
#include "quconnectionsettingswidget.h"
#include "qutrealtimeplot.h"
#include "rtconfwidget.h"
#include "cucontext.h"
#include <qutrendplot.h>
#include <quspectrumplot.h>
#include <cumacros.h>
#include <cudata.h>
#include <QMenu>
#include <QAction>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QtDebug>
#include <eplot_configuration_widget.h>

/*! the class constructor
 *
 * @param realtime true if the plot is a Tango *real time* plot
 * @param realtime false if the plot is not a Tango *real time* plot
 *
 * \note real time plots may offer a special configuration section.
 */
QuTPlotContextMenuStrategy::QuTPlotContextMenuStrategy(bool realtime)
{
    m_realtime = realtime;
}

/*! \brief returs a QMenu with an action that executes a configuration dialog
 *
 * @param w the parent widget (plot)
 *
 * A QMenu is created invoking the parent class' method QuPlotBaseContextMenuStrategy::createMenu
 * The "configure" action executes QuTPlotSettings::configure
 */
QMenu *QuTPlotContextMenuStrategy::createMenu(QWidget *w)
{
    QMenu *m = QuPlotBaseContextMenuStrategy::createMenu(w);
    QuPlotBase *plot = qobject_cast<QuPlotBase *>(w);
    QuTPlotSettings *pconf = plot->findChild<QuTPlotSettings *>();
    if(!pconf) /* reuse pconf if possible */
        pconf = new QuTPlotSettings(plot, m_realtime);

    m->findChild<QAction *>("configure")->disconnect();
    QObject::connect(m->findChild<QAction *>("configure"), &QAction::triggered, pconf, &QuTPlotSettings::configure);
    return m;
}

/*! \brief creates the QuTPlotSettings (QObject)
 *
 * @param plot a pointer to the QuPlotBase that will be configured
 * @param realtime true if the plot is a *real time* plot, false otherwise.
 *        Default: false (*real time plot* configuration widget unimplemented yet)
 *
 * \note
 * the *real time plot* configuration widget has not yet been made compatible
 * with cumbia-qtcontrols plots
 */
QuTPlotSettings::QuTPlotSettings(QuPlotBase *plot, bool realtime) : QObject(plot)
{
    m_plot = plot;
    m_realtime = realtime;
}

/*! \brief the class destructor
 */
QuTPlotSettings::~QuTPlotSettings()
{
    pdelete("~QuTPlotSettings %p", this);
}

/*! \brief configuration slot that creates a QDialog and populates it with configuration
 *         elements
 *
 * The dialog contains two tabs:
 * \li one configures the plot aspect through EPlotConfigurationWidget
 * \li one configures the Tango connection through either RtConfWidget or
 *     QuConnectionSettingsWidget, according to the real time parameter being
 *     true or false respectively.
 *
 * The dialog is executed at the end of the method
 *
 * \note
 * the *real time plot* configuration widget has not yet been made compatible
 * with cumbia-qtcontrols plots
 */
void QuTPlotSettings::configure()
{
    QDialog d(m_plot);
    QVBoxLayout *lo = new QVBoxLayout(&d);
    QGroupBox *gb = new QGroupBox(&d);  // connection settings groupbox
    QVBoxLayout *gblo = new QVBoxLayout(gb); // group box layout
    EPlotConfigurationWidget *base_conf_w = new EPlotConfigurationWidget(m_plot);
    QWidget *connection_conf_w;
    gb->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    if(m_realtime && qobject_cast<QuTRealtimePlot *>(m_plot))
    {
        gb->setTitle("Real Time configuration");
        connection_conf_w = new RtConfWidget(qobject_cast<QuTRealtimePlot *>(m_plot));
    }
    else if(!m_realtime)
    {
        gb->setTitle("Connection settings");
        connection_conf_w = new QuConnectionSettingsWidget(m_plot);
        connect(connection_conf_w, SIGNAL(settingsChanged(const CuData&)), this, SLOT(apply(const CuData&)));
    }
    gblo->addWidget(connection_conf_w);
    QTabWidget tab (&d);
    tab.insertTab(0, base_conf_w, "Aspect");
    tab.insertTab(1, gb, "Connection");
    lo->addWidget(&tab);
    d.exec();
}

/*! \brief the CuData passed as parameter is forwarded to the plot through its CuContext
 *
 * @param da the CuData to be forwarded to the plot in order to change its configuration
 *
 * \par Note
 * The input CuData comes from a signal emitted by a QuConnectionSettingsWidget
 * and will typically store settings to change the period and refresh mode of a
 * Tango source.
 */
void QuTPlotSettings::apply(const CuData &da)
{
    if(qobject_cast<QuTrendPlot *>(m_plot))
        qobject_cast<QuTrendPlot *>(m_plot)->getContext()->setOptions(da);
    else if(qobject_cast<QuSpectrumPlot *>(m_plot))
        qobject_cast<QuSpectrumPlot *>(m_plot)->getContext()->setOptions(da);
}
