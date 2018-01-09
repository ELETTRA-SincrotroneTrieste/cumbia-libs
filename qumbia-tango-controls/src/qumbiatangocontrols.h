#ifndef QUMBIATANGOCONTROLS_H
#define QUMBIATANGOCONTROLS_H

#include "qumbia-tango-controls_global.h"

/** \mainpage qumbia-tango-controls: Qt + cumbia-tango module.
 *
 * qumbia-tango-controls is written in Qt in order to combine together the Qt cumbia-qtcontrols and the
 * cumbia-tango module together. It provides a higher level interface to use graphical elements and
 * QObjects from the Qt library and connect them to the tango control system.
 *
 * \subsection main_classes Main classes
 *
 * \li CuTControlsReader and CuTReaderFactory to create, start, stop, pause, modify and destroy readers.
 * \li CuTControlsWriter and CuTWriterFactory to create, execute and destroy writers.
 * \li CuEpControlsUtils helper utilities, for example to replace wildcards within readers' sources and
 *     writers' targets.
 *
 * Other classes implement strategy patterns to manage contextual menus for plots and plot updates,
 * QuTPlotContextMenuStrategy and QuTPlotUpdateStrategy.
 *
 * \subsection tutorials Tutorials
 * \li \ref md_src_tutorial_qumbiatango_app
 *
 */
class QUMBIATANGOCONTROLSSHARED_EXPORT QumbiaTangoControls
{

public:
    QumbiaTangoControls();
};

#endif // QUMBIATANGOCONTROLS_H
