#ifndef CUMBIATANGO_H
#define CUMBIATANGO_H

class CuTangoActionFactoryI;
class CuDataListener;

#include <cumbia.h>
#include <cutangoactioni.h>
#include<string>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;

/** \mainpage
 * \a cumbia-tango integrates cumbia with the Tango control system framework, providing specialised Activities to read,
 * write attributes and impart commands.
 * Readings are accomplished through either a poller or the Tango event system, for those attributes suitably configured.
 * Write operations are always executed in an asynchronous thread and the result is delivered later in the main thread.
 * Cumbia activities are employed by the module to setup the connection, access the database, subscribe to events or carry
 * out periodic readings. Progress and result events are delivered to the main thread from the background activity.
 * As stated in the previous section, activities identified by the same token belong to the same thread.
 * Here, the token is the Tango device name. Applications that connect to the Tango control system will typically
 * instantiate a CumbiaTango object that defines which kind of threads will be used (e.g. Qt’s for graphical interfaces)
 * and thereafter parametrizes each reader or writer. Several modern design patterns have been exploited to provide
 * a flexible and scalable architecture. Singletons have been completely replaced by service providers in order to offer
 * services For graphical applications. The component provides helpful classes that can be used from outside an activity
 * to access devices, fetch database properties or interpret exceptions raised from within the engine. Aside from these
 * utilities, one would not normally employ this module directly. Cumbia-qtcontrols and qumbia-tango-controls is where
 * to look for when the integration between the control system and the user interface is the objective.
 *
 *  \section related_readings Related readings
 *
 * \subsection github.io
 * <a href="https://elettra-sincrotronetrieste.github.io/cumbia-libs/index.html">cumbia-libs</a> on github.io
 *
 * \subsection tutorials Tutorials
 *
 * \subsection tutorials Tutorials
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
 * |  <a href="../../qumbia-tango-controls/html/tutorial_faq.html">Frequently Asked Questions (Tango)</a> | <a href="../../qumbia-tango-controls/html/index.html">qumbia-tango-controls</a>  |

 *
 * \subsection cumodules Modules
 *
 * |Other *cumbia* modules  |
 * |-------------------------------------------------------------------|
 * | <a href="../../cumbia/html/index.html">cumbia module</a>. |
 * | <a href="../../cumbia-tango/html/index.html">cumbia-tango module</a>. |
 * | <a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols module</a>.  |
 * | <a href="../../qumbia-tango-controls/html/index.html">qumbia-tango-controls module</a>.  |
 * | <a href="../../cumbia-epics/html/index.html">qumbia-epics module</a>.   |
 * | <a href="../../qumbia-epics-controls/html/index.html">qumbia-epics-controls module</a>.  |
 * | <a href="../../qumbia-plugins/html/index.html">qumbia-plugins module</a>.  |
 *
 *
 *
 * \section implementation Implementation
 * The \a CumbiaTango class is an extension of the \a Cumbia base one. Its main task is managing the so called  \a actions.
 * An \a action represents a task associated to either a Tango device attribute or a command (called source).
 * Read, write, configure are the main sort of jobs an action can accomplish.
 * More types of actions are foreseen, such as multiple readings or writings in sequence.
 * \a CuTangoActionI defines the interface of an action. Operations include adding or removing data listeners,
 * starting and stopping an action, sending and getting data to and from the underlying thread (for example
 * retrieve or change the polling period of a source).
 *  \a CuTReader implements the interface and holds a reference to either an activity designed to receive events
 * from \a Tango or another intended to poll a source.
 *
 * \a Activities is where the \a Tango connection is setup, database is accessed for configuration, events are subscribed,
 * a poller is started or a write operation is performed. This is done inside the thread safe \a init, \a execute
 * and \a onExit methods, invoked from another thread.
 * Progress and results are forwarded by the \a publishProgress and \a publishResult methods in the activity
 * and received in the \a onProgress and \a onResult ones implemented by the action.
 * Therein, CuDataListener’s \a onUpdate method is invoked with the new data. Reception safely occurs in
 * the main thread.
 *
 * As previously stated, activities identified by the same token (a CuData object) belong to the same thread.
 * <em>cumbia-tango groups threads by Tango device name</em>.
 *
 * \section rel_classes Relevant classes
 *
 * \subsection readers Readers
 *
 * \subsubsection read_att_cmd Tango attributes and commands with output arguments.
 *
 * The CuTReader is able to read device attributes as well as commands that return a value. What they read is
 * determined by the source, expressed as a string with the same syntax known by QTango users.
 *
 * \li tango/device/devname/attribute_name reads an attribute
 * \li tango/device/devname->command_name reads a command
 * \li tango/device/devname->command_name(10,100) reads the output of a command accepting two numbers in input
 *
 * The refresh mode is normally automatically chosen by cumbia-tango. If possible, events are used. If not,
 * the second choice is polling. CuEventActivity and CuPollingActivity are the two cumbia <em>activities</em>
 * involved respectively. Before starting either activity, CuTAttConfigActivity is used to get either the attribute
 * properties or the command information.
 * In any of the methods involving data exchange, CuData is used to hand results to the CuDataListener implementations
 * installed on the reader.
 *
 * The <em>refresh mode</em> as well as the polling period (if pertinent) can be changed at runtime.
 *
 * \subsubsection read_dbprop Tango database properties.
 *
 * CuTDbPropertyReader class deals with fetching <em>device, attribute and class</em> properties from the Tango
 * database. Property retrieval is performed in a separate thread. CuData and CuDataListener have the same role
 * as the aforementioned reader. A list of properties (also of mixed type - device, attribute or class) can be
 * requested setting up a set of CuData input descriptive elements. See CuTDbPropertyReader documentation.
 *
 * \subsubsection multi_readers Multiple source readers.
 * A cumbia <em>plugin</em> is available to sequentially or concurrently perform readings.
 * See qumbia-plugins/cumbia-multiread.
 *
 * \subsection writers Writers
 *
 * CuTWriter is the class in charge of writing attributes and imparting commands. The syntax used for the target name
 * is the same as the =reader's one. Writing is performed in a separate thread.
 *
 * \section cut_graphical cumbiatango in graphical applications
 *
 * The classes listed above are rarely supposed to be directly instantiated and used. In graphical applications, the
 * cumbia-qtcontrols and the cumbia-tango-controls modules have to be used.
 * See the cumbia-qtcontrols and the cumbia-tango-controls modules for further information.
 *
 * Graphical applications would normally instantiate CumbiaTango as explained in the constructor documentation and then
 * pass the reference to the objects (readers, writers) that need it to communicate with the cumbia-tango engine.
 * The other class methods should seldom be necessary.
 *
 *
 * \par Tutorials
 * \li <a href="tutorial_activity.html">Writing an activity</a>
 * \li <a href="cudata_for_tango.html">CuData for Tango</a>
 * \li <a href="../../qumbia-tango-controls/html/tutorial_cumbiatango_widget.html">Writing a Qt widget that integrates with cumbia</a>
 * \li <a href="../../cuuimake/html/cuuimake.html">Using <em>cuuimake</em></a>.
 * \li <a href="../../qumbia-tango-controls/html/tutorial_qumbiatango.html">Writing a <em>Qt application</em> with cumbia and Tango</em></a>.
 *
 * \par See also
 * \li <a href="../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols module</a>.
 * \li <a href="../qumbia-tango-controls/html/index.html">qumbia-tango-controls module</a>.
 * \li <a href="../cumbia-epics/index.html">qumbia-epics module</a>.
 * \li <a href="../qumbia-epics-controls/html/index.html">qumbia-epics-controls module</a>.
 *
 */
class CumbiaTango : public Cumbia
{

public:
    enum Type { CumbiaTangoType = Cumbia::CumbiaUserType + 1 };

    CumbiaTango(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaTango();

    void addAction(const std::string& source, CuDataListener *l, const CuTangoActionFactoryI &f);

    void unlinkListener(const std::string& source, CuTangoActionI::Type t, CuDataListener *l);

    CuTangoActionI *findAction(const std::string& source, CuTangoActionI::Type t) const;

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    virtual int getType() const;

private:

    void m_init();

    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
};

#endif // CUMBIATANGO_H
