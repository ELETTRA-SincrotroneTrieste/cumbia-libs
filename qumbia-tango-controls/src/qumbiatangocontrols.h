#ifndef QUMBIATANGOCONTROLS_H
#define QUMBIATANGOCONTROLS_H

#include "qumbia-tango-controls_global.h"

/** \mainpage qumbia-tango-controls: Qt + cumbia-tango module.
 *
 * qumbia-tango-controls is written in Qt in order to combine together the Qt cumbia-qtcontrols and the
 * cumbia-tango module together. It provides a higher level interface to use graphical elements and
 * QObjects from the Qt library and connect them to the tango control system.
 *
 * \section related_readings Related readings
 *
 * \subsection tutorials Tutorials
 *
 * |Tutorials                                     | Module               |
 * |-------------------------------------------------------------------|:--------------------------:|
 * |  <a href="../../cumbia/html/tutorial_cuactivity.html">Writing a *cumbia* activity</a> | <a href="../../cumbia/html/index.html">cumbia</a> |
 * |  <a href="../../cumbia-tango/html/tutorial_activity.html">Writing an activity</a> | <a href="../../cumbia-tango/html/index.html">cumbia-tango</a> |
 * |  <a href="../../cumbia-tango/html/cudata_for_tango.html">CuData for Tango</a> | <a href="../../cumbia-tango/html/index.html">cumbia-tango</a> |
 * |  <a href="../../qumbia-tango-controls/html/tutorial_cumbiatango_widget.html">Writing a Qt widget that integrates with cumbia</a> | <a href="../../qumbia-tango-controls/html/index.html">qumbia-tango-controls</a>  |
 * |  <a href="../../cuuimake/html/cuuimake.html">Using <em>cumbia ui make</em></a> to process Qt designer UI files | <a href="../../cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
 * |  <a href="../../qumbiaprojectwizard/html/tutorial_qumbiatango.html">Writing a <em>Qt application</em> with cumbia and Tango</em></a>. |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * |  <a href="../../qumbiaprojectwizard/html/tutorial_from_qtango.html">Porting a <em>QTango application</em> to <em>cumbia-tango</em></a>. |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * |  <a href="../../qumbianewcontrolwizard/html/tutorial_qumbianewcontrolwizard.html"><em>cumbia new control</em></a>: quickly add a custom Qt widget to a cumbia project | <a href="../../qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
 * |  <a href="../../cumbia-qtcontrols/html/understanding_cumbia_qtcontrols_constructors.html">Understanding <em>cumbia-qtcontrols constructors, sources and targets</em></a> |<a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols</a>. |
 *
 * \subsection cumodules Modules
 *
 * |Other *cumbia* modules  |
 * |-------------------------------------------------------------------|
 * | <a href="../../cumbia/html/index.html">cumbia module</a>. |
 * | <a href="../../cumbia-tango/html/index.html">cumbia-tango module</a>. |
 * | <a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols module</a>.  |
 * | <a href="../../qumbia-tango-controls/html/index.html">cumbia-qtcontrols module</a>.  |
 * | <a href="../../cumbia-epics/html/index.html">qumbia-epics module</a>.   |
 * | <a href="../../qumbia-epics-controls/html/index.html">qumbia-epics-controls module</a>.  |
 *
 *
 * \subsection cu_apps apps
 *
 * These applications (and their documentation, that has already been mentioned in the *Tutorials* table above)
 * must be installed from the *qumbia-apps* sub-directory of the *cumbia-libs* distribution.
 * To install them, *cd* into that folder and execute:
 *
 * \code
 * qmake
 * make
 * sudo make install
 * \endcode
 *
 * Along the applications executables and documentation, two bash scripts will be installed:
 *
 * - /etc/bash_completion.d/cumbia
 * - /etc/bash/bashrc.d/cumbia.sh
 *
 * They define shortcuts for the common operations provided by the *qumbia-apps* applications as follows:
 *
 *
 * |Applications (command line)   | description                                 | app
 * |------------------------------|--------------------------------------------|:---------------:|
 * | *cumbia new project*          | create a new cumbia project               |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * | *cumbia import*               | migrate a QTango project into cumbia      |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * | *cumbia new control*          | write a *cumbia control* reader or writer | <a href="../../qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
 * | *cumbia ui make*              | run *cuuimake* to generate *qt+cumbia* ui_*.h files | <a href="../../cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
 * | *cumbia client*               | run a generic cumbia client | <a href="../../cumbia_client/html/index.html">qumbia-apps/cumbia_client</a>  |
 *
 * *bash auto completion* will help you use these shortcuts: try
 *
 * \code
 * cumbia <TAB>
 * \endcode
 *
 * or
 *
 * \code
 * cumbia new <TAB>
 * \endcode
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
 * \li <a href="../../cuuimake/html/cuuimake.html">Using <em>cuuimake</em> to process Qt designer UI files</a>
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
