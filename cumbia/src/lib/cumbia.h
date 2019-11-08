#ifndef CUMBIA_H
#define CUMBIA_H

class CuServiceProvider;
class CumbiaPrivate;
class CuThreadListener;
class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuData;
class CuActivity;
class CuContinuousActivity;
class CuActivityEvent;

/** \mainpage
 *
 * Cumbia is a library that offers a carefree approach to multi thread application design and implementation.
 * The user writes \b Activities and decides when their instances are started and to which thread they belong.
 * A \b token is used to register an \b Activity and activities registered with the same token are run in
 * the same thread. Work is done inside the \a init, \a execute and \a exit methods of an \a Activity.
 * The library guarantees that the aforementioned methods are always called in the activity thread.
 * From within \a init, \a execute and \a exit, computed results can be forwarded to the main execution thread,
 * where they can be used to update a graphical interface.
 *
 *
 *  \section related_readings Related readings
 *
 * \subsection github.io
 * <a href="https://elettra-sincrotronetrieste.github.io/cumbia-libs/index.html">cumbia-libs</a> on github.io
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
 * | <a href="../../cumbia-qtcontrols/html/qumbia_plugins.html">qumbia-plugins module</a>.  |
 *
 *
 * \subsection cuplugins Plugins
 *
 * |*cumbia* plugins  |
 * |-------------------------------------------------------------------|
 * | <a href="../../cuformula/html/index.html">formula plugin</a>. |
 * | <a href="../../cumbia-dbus/html/index.html">dbus plugin</a>. |
 * | <a href="../../cumbia-multiread/html/index.html">multi reader plugin</a>. |
 * | <a href="../../widgets-std-context-menu-actions/html/index.html">context menu actions plugin</a>. |
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
 * They define shortcuts for the common operations provided by the *qumbia-apps* applications as follows.
 *
 *
 * |Applications (command line)   | description                                 | app
 * |------------------------------|--------------------------------------------|:---------------:|
 * | *cumbia new project*          | create a new cumbia project               |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * | *cumbia import*               | migrate a QTango project into cumbia      |<a href="../../qumbiaprojectwizard/html/index.html">qumbia-apps/qumbiaprojectwizard</a>  |
 * | *cumbia new control*          | write a *cumbia control* reader or writer | <a href="../../qumbianewcontrolwizard/html/index.html">qumbia-apps/qumbianewcontrolwizard</a>  |
 * | *cumbia ui make*              | run *cuuimake* to generate *qt+cumbia* ui_*.h files | <a href="../../cuuimake/html/index.html">qumbia-apps/cuuimake</a>  |
 * | *cumbia client*               | run a generic cumbia client | <a href="../../qumbia-client/html/index.html">qumbia-apps/cumbia_client</a>  | * | *cumbia client*               | run a generic cumbia client | <a href="../../cumbia_client/html/index.html">qumbia-apps/cumbia_client</a>  |
 * | *la-cumparsita*               | interpret a Qt designer *ui file* on the fly | <a href="../../la-cumparsita/html/index.html">qumbia-apps/cumbia_client</a>  |

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
 *  \section cumbia_mainpar Cumbia
 *
 * In asynchronous environments, threads have always posed some kind of challenge for the programmer. Shared data,
 * message exchange, proper termination are some aspects that cannot be overlooked. The \a Android \a AsyncTask offers a
 * simple approach to writing code that is executed in a separate thread.
 * The API provides a method that is called in the secondary thread context and functions to post results on the main one.
 *
 * \par Activities
 * Cumbia \a CuActivity’s purpose is to replicate the carefree approach supplied by the \a AsyncTask.
 * In this respect, a \a CuActivity is an interface to allow subclasses do work within three specific
 * methods: init, execute and onExit. Therein, the code is run in a separate thread. The \a publishProgress
 * and \a publishResult methods hand data to the main thread. To accomplish all this, an event loop must be running.
 * By an initial parametrization, either a custom one (such as Qt’s, used in qumbia-qtcontrols) or the builtin
 * cumbia \a CuEventLoop can be installed.
 * New activities must be registered in the \a CuActivityManager service, and unregistered when they are no
 * longer needed. In this way, a \a token can be used to group several activities by a smaller number of threads.
 * In other words, <em>activities with the same token run in the same thread</em>.
 *
 * \par Services
 * By means of the reference to the \a Cumbia instance, that must be maintained throughout the entire life of an application,
 * you can access services. They are registered in the \a CuServiceProvider and accessed by name. The activity manager,
 * the thread and the log services are some examples, but others can be written and installed, as long as they adhere
 * to the \a CuServiceI interface (e.g \a cumbia-tango’s \a CuActionFactoryService and \a CuDeviceFactoryService).
 * \a Cumbia can be subclassed in order to provide additional features specific to the engine employed.
 * \a CumbiaPool allows to register and use multiple engines in the same application.
 *
 * \par Data interchange
 * Data transfer is realised with the aid of the \a CuData and \a CuVariant classes. The former is a bundle pairing keys to values.
 * The latter memorises data and implements several methods to store, extract and convert it to different types and formats.
 * The \a cumbia-qtcontrols module handles these structures to provide a primary data display facility, unaware of the specific
 * engine underneath (\a Tango, \a Epics, ...)
 *
 */

/*! \brief primary cumbia class where activities are registered and unregistered.
 *         Provides access to the cumbia *service provider*
 *
 * A *cumbia* application will hold a reference to this central class throughout its lifetime,
 * to register and unregister *activities* and to register and access *services*.
 * This is achieved creating a Cumbia instance either in the main function, passing it around to
 * objects that need access to it, or in the application's main class (as for example in the
 * main widget of a Qt application).
 * *Activities* (CuActivity objects) are the main actors of a *cumbia* application. They execute
 * work in the background, publishing the results to the foreground thread when they are ready.
 * Activities are *registered* to and *unregistered* from *cumbia*.
 *
 * The *cumbia-tango* and *cumbia-epics* modules *extend* Cumbia to provide extra functionalities.
 *
 * \par Functions
 * \li Cumbia::registerActivity: a new cumbia *activity*, *CuActivity* is registered and bound to a *thread
 *     listener, CuThreadListener*
 * \li Cumbia::unregisterActivity an activity is unregistered from *cumbia*
 * \li if an activity is executed periodically, the period can be changed and the timer can be paused
 *     and resumed
 * \li Cumbia::getServiceProvider returns a pointer to the CuServiceProvider created and managed
 *     by Cumbia itself. CuThreadService and CuActivityManager are the two *services* run
 *     by this class.
 *
 * \par Example
 * Read \ref md_src_tutorial_cuactivity for a simple example involving Cumbia and CuActivity
 *
 * Other examples and tutorials can be found in the cumbia-qtcontrols and qumbia-tango-controls
 * modules that depend on *cumbia*:
 *
 * \li <a href="../cumbia-tango/md_lib_tutorial_activity.html">Writing an activity to read Tango
 *      device, attribute and class properties</a>
 * \li <a href="../../qumbia-tango-controls/html/md_src_tutorial_qumbiatango_app.html">Setting up a graphical application with cumbia and tango</a>.
 * \li <a href="../../cuuimake/html/cuuimake.html">Using <em>cuuimake</em></a>.
 * \li <a href="../../qumbia-tango-controls/html/md_src_tutorial_qumbiatango_widget.html">Writing a <em>Qt widget</em> to <em>read from Tango</em></a>.
 *
 * \par See also
 * \li <a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols module</a>.
 * \li <a href="../../qumbia-tango-controls/html/index.html">qumbia-tango-controls module</a>.
 * \li <a href="../../cumbia-epics/index.html">qumbia-epics module</a>.
 * \li <a href="../../qumbia-epics-controls/html/index.html">qumbia-epics-controls module</a>.
 *
 */
class Cumbia
{

public:
    enum Type { CumbiaBaseType = 0, ///< basic Cumbia type
                CumbiaUserType = 10,  ///< starting value for user defined Cumbia subclasses
                CumbiaMaxUserType = 256 ///< upper bound for the Type enumeration
              };

    Cumbia();

    CuServiceProvider *getServiceProvider() const;

    virtual ~Cumbia();

    void registerActivity(CuActivity *activity,
                          CuThreadListener *dataListener,
                          const CuData& thread_token,
                          const CuThreadFactoryImplI& thread_factory_impl,
                          const CuThreadsEventBridgeFactory_I& eventsBridgeFactoryImpl);

    void unregisterActivity(CuActivity *activity);

    CuActivity *findActivity(const CuData& token) const;

    void setActivityPeriod(CuActivity *a, int timeout);

    unsigned long getActivityPeriod(CuActivity *a) const;

    void pauseActivity(CuActivity *a);

    void resumeActivity(CuActivity *a);

    void postEvent(CuActivity *a, CuActivityEvent *e);

    void finish();

    virtual int getType() const;

    CuData threadToken(const CuData &options) const;

private:
    CumbiaPrivate *d;

};

#endif // CUMBIA_H
