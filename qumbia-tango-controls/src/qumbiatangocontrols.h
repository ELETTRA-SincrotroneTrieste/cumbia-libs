#ifndef QUMBIATANGOCONTROLS_H
#define QUMBIATANGOCONTROLS_H

#include "qumbia-tango-controls_global.h"

/** \mainpage qumbia-tango-controls: Qt + cumbia-tango module.
 *
 * qumbia-tango-controls is written in Qt in order to combine together the Qt cumbia-qtcontrols and the
 * cumbia-tango module together. It provides a higher level interface to use graphical elements and
 * QObjects from the Qt library and connect them to the tango control system.
 *
 *
 *  \section related_readings Related readings
 *
 * |Tutorials                                     | Module               |
 * |-------------------------------------------------------------------|:--------------------------:|
 * |  <a href="../../cumbia/html/tutorial_cuactivity.html">Writing a *cumbia* activity</a> | <a href="../../cumbia/html/index.html">cumbia</a> |
 * |  <a href="../../cumbia-tango/html/tutorial_activity.html">Writing an activity</a> | <a href="../../cumbia-tango/html/index.html">cumbia-tango</a> |
 * |  <a href="../../cumbia-tango/html/cudata_for_tango.html">CuData for Tango</a> | <a href="../../cumbia-tango/html/index.html">cumbia-tango</a> |
 * |  <a href="../../qumbia-tango-controls/html/tutorial_cumbiatango_widget.html">Writing a Qt widget that integrates with cumbia</a> | <a href="../../qumbia-tango-controls/html/index.html">this module</a>  |
 * |  <a href="../../qumbia-tango-controls/html/tutorial_qumbianewcontrolwizard.html"><em>qumbianewcontrolwizard</em></a>: quickly add a custom Qt widget to a cumbia project | <a href="../../qumbia-tango-controls/html/index.html">qumbia-tango-controls</a>  |
 * |  <a href="../../qumbia-tango-controls/html/cuuimake.html">Using <em>cuuimake</em></a> to process Qt designer UI files | <a href="../../qumbia-tango-controls/html/index.html">qumbia-tango-controls</a>  |
 * |  <a href="../../qumbia-tango-controls/html/tutorial_qumbiatango.html">Writing a <em>Qt application</em> with cumbia and Tango</em></a>. |<a href="../../qumbia-tango-controls/html/index.html">this module</a>  |
 * |  <a href="../../qumbia-tango-controls/html/tutorial_from_qtango.html">Porting a <em>QTango application</em> to <em>cumbia-tango</em></a>. |<a href="../../qumbia-tango-controls/html/index.html">this module</a>  |
 * |  <a href="../../cumbia-qtcontrols/html/understanding_cumbia_qtcontrols_constructors.html">Understanding <em>cumbia-qtcontrols constructors, sources and targets</em></a> |<a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols</a>. |
 *
 * <br/>
 *
 * |Other *cumbia* modules  |
 * |-------------------------------------------------------------------|
 * | <a href="../../cumbia/html/index.html">cumbia module</a>. |
 * | <a href="../../cumbia-tango/html/index.html">cumbia-tango module</a>. |
 * | <a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols module</a>.  |
 * | <a href="../../cumbia-epics/html/index.html">qumbia-epics module</a>.   |
 * | <a href="../../qumbia-epics-controls/html/index.html">qumbia-epics-controls module</a>.  |
 *
 * \section main_classes Main classes
 *
 * \li CuTControlsReader and CuTReaderFactory to create, start, stop, pause, modify and destroy readers.
 * \li CuTControlsWriter and CuTWriterFactory to create, execute and destroy writers.
 * \li CuEpControlsUtils helper utilities, for example to replace wildcards within readers' sources and
 *     writers' targets.
 *
 * Other classes implement strategy patterns to manage contextual menus for plots and plot updates,
 * QuTPlotContextMenuStrategy and QuTPlotUpdateStrategy.
 *
 * \section Tutorials
 * \li <a href="../../qumbia-tango-controls/html/tutorial_cumbiatango_widget.html">Writing a Qt widget that integrates with cumbia</a>
 * \li <a href="../../qumbia-tango-controls/html/cuuimake.html">Using <em>cuuimake</em> to process Qt designer UI files</a>
 * \li  <a href="../../qumbia-tango-controls/html/tutorial_qumbiatango.html">Writing a <em>Qt application</em> with cumbia and Tango</em></a>
 * \li  <a href="../../qumbia-tango-controls/html/tutorial_from_qtango.html">Porting a <em>QTango application</em> to <em>cumbia-tango</em></a>
 *
 *
 */

/*! @private
 */
class QUMBIATANGOCONTROLSSHARED_EXPORT QumbiaTangoControls
{

public:
    QumbiaTangoControls();
};

#endif // QUMBIATANGOCONTROLS_H
