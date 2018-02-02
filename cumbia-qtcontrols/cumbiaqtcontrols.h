#ifndef CUMBIAQTCONTROLS_H
#define CUMBIAQTCONTROLS_H

#include "cumbia-qtcontrols_global.h"
#include <QMetaType>
#include <cudata.h>
/**
 *  \defgroup outputw Display widgets
 *  @{
 *      \defgroup plots Plots
 *      @{
 *      @}
 *
 *      \defgroup image Image widget
 *      @{
 *      @}
 *
 * @}
 *
 * \defgroup inputw Input widgets
 * @{
 * @}
 *
 * \defgroup utils Utilities
 * @{
 * @}
 *
 * \defgroup plugins Plugins
 * @{
 * @}
 *
 * \defgroup log Logging
 * @{
 * @}
 *
 * \defgroup core Core classes
 * @{
 * @}
 *
 * \defgroup basew Base widgets
 *
 */

/*! \mainpage Cumbia Qt controls module
 *
 *
 * \section Introduction Introduction
 * This module combines cumbia to the \a Qt cross platform software framework, offering graphical control system components.
 * Labels, gauges and advanced graphs are supplied, as well as buttons and boxes to set values.
 * As mentioned earlier, elementary data representation is provided, due to the component unawareness of the \a cumbia
 * engine lying beneath. In order to display real data on the controls, you have to combine different building
 * blocks at the moment of setting up each reader or writer in your application, as described later.
 * When data is ready, it is delivered to the main thread through the onUpdate method that there must be in the control
 * component (such as a label), for it must implement the \a CuDataListener interface.
 * For an event loop must be executing, messages are posted to the main thread relying on an implementation of the
 * \a CuThreadsEventBridge_I interface. In \a Qt, we use \a QCoreApplication’s event loop in conjunction with \a cumbia-qtcontrols’
 * \a QThreadsEventBridge, which exploits \a QCoreApplication’s \a postEvent, a familiar scheme for \a Qt developers.
 * From within \a onUpdate, data is extracted and presented to the user by way of the control widget.
 *
 * Not strictly related to widgets themselves, but targeted to associate with them are a couple of abstract classes
 * that define an interface to readers and writers. They compel readers and writers to provide methods to set and
 * remove sources and targets of execution, as well as means to send and receive messages to and from actions.
 * \a CuControlsReaderA and \a CuControlsWriterA, that’s their names, keep also references to the currently active
 * \a Cumbia and data listener instances. Object composition to accomplish the set up of a \a Tango (\a Epics) reader (writer)
 * will be discussed in the \a cumbia-tango-controls documentation.
 *
 *
 * \section DeveloperObs Observations for developers
 *
 * \subsection Unlinking Unlinking a source from cumbia
 * Example: reader implemented in the cumbia-tango and qumbia-tango-controls modules.
 * Every Cumbia Qt controls widget holds a reference to a CuControlsReaderA object. The unsetSource method,
 * called from the main thread, is used to unlink a reader from cumbia.
 * Let's take QuLabel as an example. QuLabel is a graphical Qt element representing a text label to display
 * values. QuLabel implements the CuDataListener interface to receive updates from the underline reader.
 * QuLabel::unsetSource calls
 *
 * \subsubsection mainthread1 In the main thread
 * CuControlsReaderA::unsetSource on the specific CuControlsReaderA implementation.
 * \li CuTControlsReader is an implementation of CuControlsReaderA from the qumbia-tango-controls module.
 *     CuTControlsReader::unsetSource is called.
 * \li CuEpControlsReader is an implementation of CuControlsReaderA from the qumbia-epics-controls module.
 *     CuEpControlsReader::unsetSource is called.
 * These implementations call CumbiaTango::unlinkListener for  CuTangoActionI::AttConfig and
 * CuTangoActionI::Reader action types in a row.
 * CumbiaTango::unlinkListener performs a search among all the registered CuTangoActionI actions by name and type.
 * It then removes a listener from the action found, by calling removeDataListener on the CuTangoActionI.
 * In our example, CuTReader::removeDataListener is called.
 * The removeDataListener method of CuTReader removes the data listener from the list of listeners it holds.
 * This immediately prevents the listener to receive further data. In our example, the listener is QuLabel.
 *
 * \par notes
 * \li QuLabel will not be updated right after QuLabel::unsetSource.
 * \li CuTReader, holding a list of registered listeners, may decide to call CuTReader::stop if the list
 *     of listeners is empty. Actually, one CuTReader can update several listeners connected to the same
 *     source.
 *
 * CuTReader::stop calls Cumbia::unregisterActivity, passing the current activity as argument. In the case
 * of the Tango CuTReader, the activity can be either CuPollingActivity or CuEventActivity.
 * Cumbia::unregisterActivity fetches the thread where the activity belongs through the CuActivityManager
 * service and calls CuThread::unregisterActivity on it.
 * CuThread::unregisterActivity grabs the lock on its event queue and enqueues an event of type
 * UnRegisterActivityEvent, to be processed in CuThread's thread execution context.
 *
 * \subsubsection cuthread In the CuThread's thread
 * In CuThread's execution thread the UnRegisterActivityEvent holds a reference to the CuActivity that
 * has to be stopped. CuActivity::doOnExit is called in the CuThread's thread.
 * CuActivity::doOnExit is a <cite>template method</cite>: sets the activity flags (an exit flag is activated)
 * and calls the pure virtual method CuActivity::onExit. In our example, CuTReader can be executing a
 * CuPollingActivity or a CuEventActivity. In the second case, CuEventActivity::onExit is called.
 * As in every CuActivity::init, CuActivity::execute and CuActivity::onExit, the execution takes place in the
 * CuThread's thread. So, CuEventActivity::onExit is the place where <em>Tango</em> event unsubscription
 * takes place. As usual within CuActivity::init, CuActivity::execute and CuActivity::onExit, a CuData
 * object is prepared in order to hand results to a CuDataListener (our QuLabel) in the main thread.
 * As you can see from CuEventActivity::onExit, CuActivity::publishResult is finally invoked.
 *
 * CuActivity::publishResult receives the CuData that must be transmitted to the CuDataListener in the main
 * thread. Here, through the CuActivityManager::getThread, the CuThread where the activity belongs is
 * retrieved.  CuActivityManager is a service (CuServiceI), only one instance is available and registered
 * in Cumbia.
 *
 * CuActivityManager::getThread grabs a lock on its thread list and returns the thread to that
 * the activity was assigned. CuActivityManager's thread list is actually a multi map: more CuActivity
 * objects can live in the same thread. In the Tango example, the cumbia-tango module chose to assign
 * all activities referring to the same Tango device to the same CuThread. In other words distinct Tango devices,
 * different threads.
 *
 * When CuActivity::publishResult gets its thread, calls CuThread::publishResult on it.
 *
 * CuThread::publishResult relies on the installed CuThreadsEventBridge_I implementation to post an event
 * from the CuThread's secondary thread to the main thread. In a Qt application, the Qt event loop is normally
 * used and the QThreadsEventBridge will invoke QCoreApplication::postEvent on the QApplication so as to
 * deliver the CuData to the main thread (the "ui" thread).
 *
 * We are ready to go back to the main execution thread.
 *
 * \subsubsection mainthread2 (Back) In the main thread
 *
 * \note If instead of calling QuLabel::unsetSource the QuLabel had been directly deleted,
 * QuLabel::unsetSource would have destroyed CuTControlsReader, which in turn would have called
 * CuTControlsReader::unsetSource. <br/>
 * <em>At this time, QuLabel would not exist anymore, and not even the CuTControlsReader</em>. Anyway, the underline
 * activity is still there, receiving events from CuThread. They will simply not be delivered
 * to the deleted CuDataListener. <em>Also CuTReader is still alive</em>. CuTReader is deleted only after it
 * receives a CuData with the "exit" flag set to true from the CuThread, in CuTReader::onResult.
 *
 * At the end of the section <em>In the CuThread's thread</em> we left the CuThread's thread execution
 * with an event bridge (with QApplication event system) posting an event on the main thread. That's where
 * we are now.
 *
 * QThreadsEventBridge is a QObject, so it receives events received with QCoreApplication::postEvent
 * within the QThreadsEventBridge::event method. QThreadsEventBridge implements CuThreadsEventBridge_I
 * and holds a reference to CuThreadsEventBridgeListener, which interface in turn is implemented by
 * CuThread. QThreadsEventBridge::event invokes CuThread::onEventPosted
 *
 * The event carried by CuThread::onEventPosted contains the destination activity and the data for it.
 * Through the CuActivityManager::getThreadListeners method, the list of CuThreadListner is fetched.
 * We remember that CuTReader implements CuTangoActionI that implements CuThreadListener. So, our
 * CuTReader will receive the data coming from the CuThread through CuTReader::onProgress or
 * CuTReader::onResult. Let's consider the CuTReader::onResult case.
 *
 * CuTReader::onResult, and this is the end of the long story, delivers the CuData to all the registered
 * CuDataListener objects, like QuLabel in our discussion, through QuLabel::onUpdate.
 *
 * Inside QuLabel::onUpdate it is important to avoid making calls that could potentially modify in place
 * the list of registered CuDataListener. This means calling unsetSource (or setSource with the same
 * source name) is therein discouraged. Delay setSource or unsetSource somehow outside QuLabel::onUpdate.
 *
 * \section Migrating QTango readers to cumbia or writing a cumbia reader.
 * A QTango reader typically implements a couple of methods of QTangoComProxyReader and some methods from
 * QTangoWidgetCommon. In cumbia these classes have completely disappeared. Multiple class inheritance has been
 * completely dropped in favor of interface only inheritance.
 *
 *
 * \code
 *
    #include <com_proxy_reader.h>
    #include <qtango_wcommon.h>

    // In cumbia inheritance from QTangoComProxyReader and QTangoWidgetCommon
    // must be removed
    class TLabel : public ELabel, public QTangoComProxyReader, public QTangoWidgetCommon
    {
public:
    TLabel(QWidget *parent, Qt::WindowFlags f = 0);

    virtual ~TLabel();

protected slots:

    // cumbia: these two slots must be removed, as well as a
    // all connect(qtangoComHandle(), signal/slot connections in the .cpp file
    void refresh(const TVariant &);
    void configure(const TangoConfigurationParameters*);

    };

    \endcode

 * The QTango class skeleton above would look like this in cumbia:
 *
 * \code
    #include <esimplelabel.h>
    #include <cudatalistener.h>
    #include <cucontexti.h>

    class QuLabelPrivate;
    class CuData;
    class Cumbia;
    class CumbiaPool;
    class CuControlsReaderFactoryI;
    class CuControlsFactoryPool;
    class CuContext;

    class QuLabel : public ESimpleLabel, public CuDataListener, public CuContextI
    {
        Q_OBJECT
        // source property, same property name, getter and setter as QTango's
        Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

    public:
        QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

        QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

        virtual ~QuLabel();

        // CuTangoListener interface
        void onUpdate(const CuData &d);
        // CuContextI interface
        CuContext *getContext() const;

        // return the source property
        QString source() const;
        // ...

    public slots:
        // same method name as QTango's
        void setSource(const QString& s);

        // same method name as QTango's
        void unsetSource();


    signals:
        // convenience signal for users of this class, they can be notified when
        // new data is available.
        void newData(const CuData&);

        // this signal must be provided in conjunction with contextMenuEvent
        // in order for the client to be able to
        // receive statistics and information about the health of the reader.
        void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

    protected:
        void contextMenuEvent(QContextMenuEvent* e);

    private:
        // private pointer (bridge) pattern. See .cpp implementation.
        // This is a suggested pattern, not compulsory.
        QuLabelPrivate *d;
    };
 *
 * \endcode
 *
 * CuDataListener and CuContextI are pure interfaces. onUpdate and getContext must be implemented
 * in order to make a <em>label</em> widget a cumbia widget.
 *
 * \li onUpdate replaces the work that used to be done by the QTango refresh and the configure methods at once.
 *     onUpdate's data paramter contains all the information needed to update the reader, such as value, error
 *     messages, data type, format and also configuration settings (if the data "type" is "property")
 *
 * Another major change affects the constructor parametrization. The reader is configured at creation time,
 * and must be written to display data in a way that is independent from the underlying engine. In other words,
 * a QuLabel will display a scalar value coming either from Tango or another control system environment.
 *
 * In the cpp file, a CuContext must be created and parametrized either with the Cumbia/CuControlsReaderFactoryI
 * pair or the CumbiaPool/CuControlsFactoryPool. All necessary include directives must be specified, as in
 * the example below.
 *
 * \note
 * The CumbiaPool / CuControlsFactoryPool version allows the reader to be automatically set up with one of
 * the available engines according to the form of the <em>source</em> property. This means that a source
 * named <em>control:ai1</em> will connect to an Epics channel (analog input), while
 * <em>b/power_supply/psdelta/Curren</em> will read the current from a Tango power supply.
 * Engine implementations are registered and set up through the CuControlsFactoryPool object.
 *
 *
 * \code
    #include "qulabel.h"
    #include "cucontrolsreader_abs.h"
    #include <cumacros.h>  // for pinfo, perr, qstoc (qstring to char *)...
    #include <cumbiapool.h>
    #include <cudata.h>  // exchanged data definition

    #include <cucontrolsfactories_i.h>
    #include <cucontrolsfactorypool.h>
    #include <culinkstats.h>

    #include <QContextMenuEvent> // for the context menu event
    #include <cucontextmenu.h>   // for the context menu event
    #include <cucontext.h> // the CuContext of a cumbia-qtcontrols object

    class QuLabelPrivate // use private pointer pattern (<em>bridge</em>)
    {
    public:
        CuContext *context;
        // ... other attributes here
    };

    QuLabel::QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
        ESimpleLabel(w), CuDataListener()
    {
        d = new QuLabelPrivate; // d private pointer. QuLabelPrivate stores class attributes
        d->context = new CuContext(cumbia, r_factory);
    }

    QuLabel::QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
        ESimpleLabel(w), CuDataListener()
    {
        d = new QuLabelPrivate; // d private pointer. QuLabelPrivate stores class attributes
        d->context = new CuContext(cumbia_pool, fpool);
    }

    // The label destructor. We delete the context and the private pointer
    QuLabel::~QuLabel()
    {
        delete d->context;
        delete d;
    }

    // if the reader has been configured with setSource, get a reference
    // and return the reader's source. Otherwise return an empty string.
    QString QuLabel::source() const
    {
        if(CuControlsReaderA* r = d->context->getReader())
            return r->source();
        return "";
    }

    // initialise the reader with the given source
    // The name of the method is the same used in QTango.
    // Since QuLabel displays the value of only one source at a time, we use the
    // replace_reader method from the context class.
    // replace_reader returns a CuControlsReaderA reference on which setSource must
    // be called
    void QuLabel::setSource(const QString &s)
    {
        CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
        if(r)
            r->setSource(s);
    }

    // stop reading.
    void QuLabel::unsetSource()
    {
        d->context->disposeReader();
    }

    CuContext *QuLabel::getContext() const
    {
        return d->context;
    }

    void QuLabel::onUpdate(const CuData &da)
    {
        QString txt;
        QColor background, border;
        // check the "err" value on the CuData for errors
        d->read_ok = !da["err"].toBool();
        setEnabled(d->read_ok); // disable widget if there's a read error

        // update link statistics: increment operation counter on the context's link statistics
        d->context->getLinkStats()->addOperation();
        if(!d->read_ok) // add the error on the context's link statistics
            d->context->getLinkStats()->addError(da["msg"].toString());

        // cumbia-tango implementation provides quality_color and success_color codes.
        // we can use them to decorate the label accordingly.
        if(da.containsKey("quality_color"))
            background = d->palette[QString::fromStdString(da["quality_color"].toString())];
        if(da.containsKey("success_color"))
            border = d->palette[QString::fromStdString(da["success_color"].toString())];

        // as we used to do with QTango widgets, set a tooltip with the reader's message.
        setToolTip(da["msg"].toString().c_str());

        // display '#'s a la QTango in case of error
        if(da["err"].toBool() )
            setText("####");
        else if(da.containsKey("value")) // read ok
        {
            CuVariant val = da["value"];
            // if the type of data is boolean, QuLabel, as QTango TLabel, can be configured to display
            // a special color for the true and false values.
            if(val.getType() == CuVariant::Boolean)
            {
                txt = (val.toBool() ? property("trueString").toBool() : property("falseString").toBool());
                background = val.toBool() ? property("trueColor").value<QColor>() : property("falseColor").value<QColor>();
            }
            else // convert everything to string.
            {
                // CuVariant toString returns a std::string
                txt = QString::fromStdString(da["value"].toString());
            }
            setText(txt);
        }

        // cumbia-tango provides a convenient property for the state color.
        if(da.containsKey("state_color"))
        {
            CuVariant v = da["state_color"];
            QuPalette p; // QuPalette associates color names to QColor
            background = p[QString::fromStdString(v.toString())]; // use color for background
        }
        decorate(background, border); // apply colors to label (method not shown in this example)
        emit newData(da); // clients can be notified
    }


 * \endcode
 *
 * \section Migrating QTango writers to cumbia or implementing a cumbia writer.
 *
 * As in readers, QTango writers derive from multiple base classes. Take the TNumeric widget as an
 * example. It derives from the pure control widget, ENumeric, and some more base classes:
 *
 * \code class TNumeric : public ENumeric,
 *              public QTangoComProxyWriter, public SimpleDataProxy,
 *              public QTangoWidgetCommon
 * \endcode
 *
 * \li QTangoComProxyWriter is the writer
 * \li SimpleDataProxy is an interface that provides the widget contents as a string.
 *     In QTango, all writers implement that interface so that the clients are
 *     given a string representation of the data to write to Tango. In cumbia we have
 *     given up this option in favour of Qt widget's native properties such as text or
 *     value, or currentText for QComboBox.
 * \li QTangoWidgetCommon provides an implementation for common QTango widgets, such as
 *     right click events, link health, copy/paste actions and so on.
 *
 * In analogy with cumbia readers, the writers must define two parametrized constructors and
 * inherit from CuDataListener to receive updates from the link:
 *
 * \code
 * #include <QPushButton>
#include <cudatalistener.h>

class QuButtonPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuControlsWriterFactoryI;

class QuApplyNumeric : public EApplyNumeric, public CuDataListener
{
    Q_OBJECT
    // perfect analogy with QTango for the targets property
    Q_PROPERTY(QString targets READ targets WRITE setTargets DESIGNABLE true)
public:
    // same as the reader, but use CuControlsWriterFactoryI instead of CuControlsReaderFactoryI
    QuApplyNumeric(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);
    // same as the reader.
    QuApplyNumeric(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

* \endcode
*
* The cumbia writer will have a destructor, the setTargets and execute slots and the implementation
* of the onUpdate pure virtual method. This last method is used to configure the limits for the values
* that can be written by the widget, according to the range of the connected quantity (properties normally
* available both from Tango and Epics). The onUpdate may in principle be used to receive the result of
* a write operation.
*
* \note To provide a contextual menu for the writer, you must follow the same instructions given for the
* reader, inheriting from CuContextI and implementing the contextMenuEvent method and the getContext
* pure virtual function.
*
* \code
    virtual ~QuApplyNumeric();

    QString targets() const;

    // CuDatListener interface
    void onUpdate(const CuData &d);

public slots:

    void setTargets(const QString& targets);

    void execute(double val);

private:
    QuApplyNumericPrivate *d;

    void m_init(;

 * \endcode
 *
 *
 * Let's have a look at the implementation within the cpp file.
 * In analogy with the reader, a CuContext is created. A <em>bridge pattern</em> is used in this
 * example to keep QuNumeric interface separate from implementation and class attributes.
 * Please note a reference to the CuLog object used to log errors. The same approach for error
 * logging can be adopted by a reader.
 *
 * \code
 *
    // necessary includes
    #include "quapplynumeric.h"
    #include <cumacros.h>   // pinfo pwarn perr qstoc
    #include <cudata.h>     // exchanged data class definition
    #include <cumbia.h>     // cumbia

    #include "cucontrolswriter_abs.h"
    #include "cucontrolsfactories_i.h"
    #include "cucontrolsutils.h"
    #include "cumbiapool.h"
    #include "cucontext.h"
    #include "qulogimpl.h"
    // bridge pattern: stores class attributes
    class QuApplyNumericPrivate
    {
    public:
        CuContext *context;
        bool auto_configure;
        bool write_ok;
        CuLog *log;
    };

  * \endcode
  *
  * The two constructors take advantage of the common initialization performed by m_init.
  * Please note the necessary signal/slot connection within the m_init method to invoke the
  * execute method when the <em>apply</em> button is clicked:
  *
  * \code

    QuApplyNumeric::QuApplyNumeric(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
        : EApplyNumeric(parent)
    {
        m_init();
        d->context = new CuContext(cumbia, w_fac);
    }

    QuApplyNumeric::QuApplyNumeric(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
        : EApplyNumeric(parent)
    {
        m_init();
        d->context = new CuContext(cumbia_pool, fpool);
    }

    void QuApplyNumeric::m_init()
    {
        d = new QuApplyNumericPrivate; // allocate the bridge
        connect(this, SIGNAL(clicked(double)), this, SLOT(execute(double)));
        d->auto_configure = true; // auto configure option: set limits on the values
        d->write_ok = false;
    }
  *
  * \endcode
  * The destructor deletes the context and the private pointer:
  *
  * \code
    QuApplyNumeric::~QuApplyNumeric()
    {
        delete d->context;
        delete d;
    }
  * \endcode
  *
  * The targets and setTargets methods set up the connection that is used to perform the writing later on.
  *
  * \code
    QString QuApplyNumeric::targets() const
    {
        CuControlsWriterA *w = d->context->getWriter();
        if(w != NULL)
            return w->targets(); // if the writer is configured
        return ""; // empty string otherwise
    }

    // set up the connection
    void QuApplyNumeric::setTargets(const QString &targets)
    {
        // only one writer at a time: use replace_writer from CuContext
        CuControlsWriterA* w = d->context->replace_writer(targets.toStdString(), this);
        if(w)
            w->setTargets(targets); // setTargets must be called on a valid writer
    }
  *
  * \endcode
  *
  *  When the user clicks on the OK or Apply button, execute is invoked.
  *  \li Use CuControlsUtils to interpret the targets correctly and pack them into a CuVariant.
  * \li use setArgs on the writer to pass the input value to be written
  * \li call execute on the writer itself
  * \code
    void QuApplyNumeric::execute(double val)
    {
        cuprintf("QuApplyNumeric.execute\n");
        CuVariant args(val);
        printf("QuApplyNumeric.execute: got args %s type %d format %d\n", args.toString().c_str(), args.getType(),
               args.getFormat());
        CuControlsWriterA *w = d->context->getWriter();
        if(w)
        {
            w->setArgs(args);
            w->execute();
        }
    }
  * \endcode
  *
  * The onUpdate method is used to configure the widget so that the user can write only values within
  * a given range provided by the target.
  * The first lines of the method deal with possible error conditions.
  * The log facility is supplied by a service. A service is registered in a service provider class
  * (CuServiceProvider) handled by Cumbia. So a reference to Cumbia must be obtained first. In the
  * simplest case where the QuApplyNumeric has been parametrized with a Cumbia reference, just fetch
  * such reference from the CuContext::cumbiaPool method. If the reference is not availabe, we pick
  * a Cumbia implementation from the CumbiaPool, that will do its best to find one according to the
  * source name.
  *
  * \code

void QuApplyNumeric::onUpdate(const CuData &da)
{
    if(da["err"].toBool())
    {

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) // pick from the CumbiaPool
            cumbia = d->context->cumbiaPool()->getBySrc(da["src"].toString());
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log)) )
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
            log->write(QString("QuApplyNumeric [" + objectName() + "]").toStdString(), da["msg"].toString(), CuLog::Error, CuLog::Write);
        }
    }
    else if(d->auto_configure && da["type"].toString() == "property")
    {
        QString desc = "";
        if(da["data_format_str"] == "scalar" && da["writable"].toInt() > 0)
        {

            CuVariant m, M;
            m = da["min"];
            M = da["max"];
            std::string print_format = da["format"].toString();
            double min, max;
            bool ok;
            ok = m.to<double>(min);
            if(ok)
                ok = M.to<double>(max);
            if(ok)
            {
                configureNumber(min, min, QString::fromStdString(print_format));
                setIntDigits(integerDigits());
                setDecDigits(decimalDigits());
                setMaximum(max);
                setMinimum(min);
                desc = "\n(min: "+ QString("%1").arg(min) + " max: "+ QString("%1").arg(max) + ")";
            }
            else
                pinfo("QuApplyNumeric: maximum and minimum values not set on the tango attribute \"%s\", object \"%s\": "
                      "not setting format nor maximum/minimum", qstoc(targets()), qstoc(objectName()));
            double val;
            bool can_be_double = da["w_value"].to<double>(val);
            if (can_be_double)
            {
                setValue(val);
                clearModified();
            }
            if(!da["description"].isNull()) {
                desc.prepend(QString::fromStdString(da["description"].toString()));
            }
            setWhatsThis(desc);
        }
        else
            perr("QuApplyNumeric [%s]: invalid data format \"%s\" or read only source (writable: %d)", qstoc(objectName()),
                 da["data_format_str"].toString().c_str(), da["writable"].toInt());

    }
}


 * \endcode
 *
 */
class CUMBIAQTCONTROLSSHARED_EXPORT CumbiaQtControls
{

public:
    CumbiaQtControls();
};

#endif // CUMBIAQTCONTROLS_H
