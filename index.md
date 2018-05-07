# cumbia library documentation

### Tutorials

 |Tutorials                                     | Module               |
 |-------------------------------------------------------------------|--------------------------|
 |  <a href="html/cumbia/html/tutorial_cuactivity.html">Writing a *cumbia* activity</a> | <a href="html/cumbia/html/index.html">cumbia</a> |
 |  <a href="html/cumbia-tango/html/tutorial_activity.html">Writing an activity</a> | <a href="html/cumbia-tango/html/index.html">cumbia-tango</a> |
 |  <a href="html/cumbia-tango/html/cudata_for_tango.html">CuData for Tango</a> | <a href="html/cumbia-tango/html/index.html">cumbia-tango</a> |
 |  <a href="html/qumbia-tango-controls/html/tutorial_cumbiatango_widget.html">Writing a Qt widget that integrates with cumbia</a> | <a href="html/qumbia-tango-controls/html/index.html">qumbia-tango-controls</a>  |
 |  <a href="html/cuuimake/html/cuuimake.html">Using <em>cumbia ui make</em></a> to process Qt designer UI files | <a href="html/cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
 |  <a href="html/qumbiaprojectwizard/html/tutorial_qumbiatango.html">Writing a <em>Qt application</em> with cumbia and Tango</a>. |<a href="html/qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 |  <a href="html/qumbiaprojectwizard/html/tutorial_from_qtango.html">Porting a <em>QTango application</em> to <em>cumbia-tango</em></a>. |<a href="html/qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 |  <a href="html/qumbianewcontrolwizard/html/tutorial_qumbianewcontrolwizard.html"><em>cumbia new control</em></a>: quickly add a custom Qt widget to a cumbia project | <a href="html/qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
 |  <a href="html/cumbia-qtcontrols/html/understanding_cumbia_qtcontrols_constructors.html">Understanding <em>cumbia-qtcontrols constructors, sources and targets</em></a> |<a href="html/cumbia-qtcontrols/html/index.html">cumbia-qtcontrols</a>. |

### Modules

 |*cumbia* modules  |
 |-------------------------------------------------------------------|
 | <a href="html/cumbia/html/index.html">cumbia base</a>. |
 | <a href="html/cumbia-tango/html/index.html">cumbia-tango</a>. |
 | <a href="html/cumbia-qtcontrols/html/index.html">cumbia-qtcontrols</a>.  |
 | <a href="html/qumbia-tango-controls/html/index.html">cumbia-qtcontrols</a>.  |
 | <a href="html/cumbia-epics/html/index.html">qumbia-epics</a>.   |
 | <a href="html/qumbia-epics-controls/html/index.html">qumbia-epics-controls</a>.  |


### Apps
 
These applications (and their documentation, that has already been mentioned in the *Tutorials* table above)
must be installed from the *qumbia-apps* sub-directory of the *cumbia-libs* distribution.
To install them, *cd* into that folder and execute:
 
\code
qmake
make
sudo make install
\endcode
 
Along the applications executables and documentation, two bash scripts will be installed:
 
- /etc/bash_completion.d/cumbia
- /etc/bash/bashrc.d/cumbia.sh

They define shortcuts for the common operations provided by the *qumbia-apps* applications as follows:
 
 
|Applications (command line)   | description                                 | app
|------------------------------|--------------------------------------------|:---------------:|
| *cumbia new project*          | create a new cumbia project               |<a href="html/qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
| *cumbia import*               | migrate a QTango project into cumbia      |<a href="html/qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
| *cumbia new control*          | write a *cumbia control* reader or writer | <a href="html/qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
| *cumbia ui make*              | run *cuuimake* to generate *qt+cumbia* ui_*.h files | <a href="html/cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
| *cumbia client*               | run a generic cumbia client | <a href="html/cumbia_client/html/index.html">qumbia-apps/cumbia_client</a>  |
 
*bash auto completion* will help you use these shortcuts: try

\code
cumbia <TAB>
\endcode
 
or
 
\code
cumbia new <TAB>
\endcode


 
Start reading the <a href="html/cumbia/html/index.html">cumbia base library documentation</a>.


