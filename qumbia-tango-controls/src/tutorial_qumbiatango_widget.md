Writing a Qt widget that integrates with cumbia {#tutorial_cumbiatango_widget}
==============================================================================

In this tutorial we will learn how to adapt a Qt widget to display data from an engine like Tango or Epics.
In other words, we will write a *cumbia-qtcontrols* widget.
The tutorial is inspired by QuLabel from the cumbia-qtcontrols library. See qulabel.h and qulabel.cpp for the complete widget.

## Interface design

### Inheritance
Let the widget inherit from the Qt widget suitable to display the data in the form you require.
It must implement the CuDataListener interface in order to receive updates and CuContextI so that the clients can get a 
reference to the CuContext used by the object.

\code
#include <QLabel>
#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>

class QuLabel : public QLabel, public CuDataListener, public CuContextI
{
    Q_OBJECT
\endcode

### The class constructor
In order to offer maximum flexibility, two constructors should be provided:

\code
    QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
\endcode

\li w the parent widget
\li cumbia: a pointer to a Cumbia instance, typically the constructor will be called with a reference to CumbiaTango or CumbiaEpics
\li CuControlsReaderFactoryI: a const reference to a factory that instantiates the reader according to the engine in use: Tango
(CuTReaderFactory) or Epics (CuEpReaderFactory).
\li CumbiaPool: a pointer to an object designed to configure the QuLabel at runtime according to the source provided for the reader. 
In other words, if CumbiaPool parametrizes QuLabel, Tango and Epics sources can be used, even together in the same application.
\li CuControlsFactoryPool: a const reference to a factory that allocates the appropriate reader depending on the source specified.

### CuDataListener implementation

Implementing CuDataListener implies writing the *onUpdate* method:

\code
    void onUpdate(const CuData &d);
\endcode

The method will update the contents of the label.

### CuContextI implementation

A method returning a pointer to the CuContext in use must be written:

\code
    CuContext *getContext() const;
\endcode

### Optional: store private class attributes into a private pointer (*bridge* pattern)
To make a class binary compatible across several versions in the future, we choose to store the private 
attributes of the class into a *QuLabelPrivate* object.

\code

// after #include directives

class QuLabelPrivate; // will be defined in .cpp

class QuLabel : public QLabel, public CuDataListener, public CuContextI
{
    Q_OBJECT
    
    /* ... */
    
private:
    QuLabelPrivate *d;
};
        
\endcode

### Reader configuration: source and setSource methods

Two methods are provided to tell QuLabel the name of the connection end point. Methods names and usage will be 
familiar to QTango users:

\code

public:
    QString source() const;
    
public slots:
    void setSource(const QString& s);


\endcode

### Get link statistics and health through a right click menu on the widget
The QuLabel can be designed to provide an option to show a dialog window with statistics on the readings,
a plot of the trend of the variable (if scalar) and other piecese of information about the health of the link.
To enable this feature, a special *Qt signal* must be defined and a contextMenuEvent implementation must be given,
from where the signal is emitted. Add this to the header file:

\code
signals:
    void linkStatsRequest(QWidget *myself, CuContextI *myself_as_cwi);

protected:
    void contextMenuEvent(QContextMenuEvent* e);
\endcode

### Class destructor

Add the class destructor and the header file is now complete:

\code
public:
    virtual ~QuLabel();
\endcode

## cpp implementation

The QuLabel interface has been designed; focus now on its implementation.

### Needed include files
The following include files are needed to write the code that will follow.

\code 
#include <cucontrolsreader_abs.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <culinkstats.h>
#include <cucontextmenu.h>
#include <cucontext.h>

#include <QContextMenuEvent>
#include <cumacros.h> // for prints: perr, qstoc, printf...
\endcode

### Private class attributes
As discussed above, we decide to place the class attributes into a *private* class, applying the *bridge* design pattern 
to save future issues with binary compatibility across updates.

\code
class QuLabelPrivate
{
public:
    bool read_ok;
    CuContext *context;
};
\endcode

### Constructor implementation

QLabel's constructor is invoked with the parent widget.
The other two parameters are forwarded to the CuContext to delegate the connection setup
depending on the chosen engine.

\code
QuLabel::QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    QLabel(w), CuDataListener()
{
    d = new QuLabelPrivate;
    d->read_ok = false;
    d->context = new CuContext(cumbia, r_factory);
}

QuLabel::QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    QLabel(w), CuDataListener()
{
    d = new QuLabelPrivate;
    d->read_ok = false;
    d->context = new CuContext(cumbia_pool, fpool);
}
\endcode

### Destructor implementation

\code
QuLabel::~QuLabel()
{
    delete d->context;
    delete d;
}
\endcode

### source and setSource methods

The setSource function will rely on the CuContext method to replace the existing source.
Don't forget to call setSource on the CuControlsReader returned by the CuContext replace_reader call:

\code
void QuLabel::setSource(const QString &s)
{
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}
\endcode

The source method gets a pointer to the reader from the CuContext and returns its source 

\code
QString QuLabel::source() const
{
    if(CuControlsReaderA* r = d->context->getReader())
        return r->source();
    return "";
}
\endcode

If no readers have been set up with setSource, then NULL is returned by CuContext::getReader and an empty string
is in turn returned by source.

### Context menu event for statistics dialog

Reimplement QWidget's virtual method *contextMenuEvent* and use the CuContextMenu menu as a means  to request statistics,
connecting its *linkStatsTriggered* signal to QuLabel's *linkStatsRequest* signal:

\code 
void QuLabel::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = new CuContextMenu(this, this);
    connect(m, SIGNAL(linkStatsTriggered(QWidget*, CuContextI *)),
            this, SIGNAL(linkStatsRequest(QWidget*, CuContextI *)));
    m->popup(e->globalPos());
}
\endcode

### onUpdate: get data from the engine, format and display it on the label

The last necessary piece of code receives the read data in input, extracts necessary information about errors, a
message describing the result of the operation, the value and some other relevant fields.
This is a simplified version of the *QuLabel::onUpdate* version, for educational purposes.
A label displays data as text, so we declare a *QString* named *txt* for the purpose.<br/>
The *read_ok* class attribute is updated early in the function, and the widget is disabled if an error occurred,
to strike that the value is not available.<br/>
Link statistics are then updated and, in case of error, the *addError* method is invoked to record the error within
the CuContext link stats. In case of error, the string *"####"* is set on the label.<br/>
The message is set as *QWidget tooltip* and the value is extracted by the CuVariant paired with the *"value"* key in the 
CuData bundle.


\code
void QuLabel::onUpdate(const CuData &da)
{
    QString txt;
    d->read_ok = !da["err"].toBool();
    setEnabled(d->read_ok);

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da["msg"].toString());

    setToolTip(da["msg"].toString().c_str());

    if(da["err"].toBool() )
        setText("####");
    else if(da.containsKey("value"))
    {
        CuVariant val = da["value"];
        txt = QString::fromStdString(val.toString());
        setText(txt);
    }
}
\endcode

The written code is enough to make a *Qt QLabel* read from an engine like Tango and Epics.

Let's now see how a client uses the new label. If you create a new cumbia project with the *qumbiaprojectwizard* application 
(see <a href="../html/md_src_tutorial_qumbiatango_app.html">Setting up a graphical application with cumbia and tango</a>), 
you will have the necessary variables automatically declared for you and ready to use.

If you choose *tango* as engine in the *qumbiaprojectwizard*, your main class declaration will contain something like this in the *private* section:

\code
private:
    Ui::MycumbiatangoWidget *ui;

    CumbiaTango *cu_t;
    CuTReaderFactory cu_tango_r_fac;
    CuTWriterFactory cu_tango_w_fac;
\endcode

The *cu_t* and *cu_tango_r_fac* class attributes are what you need to instantiate the new *label*.
Supposing to instantiate the label in the MycumbiatangoWidget constructor:

\code 
Mycumbiatangoapp::Mycumbiatangoapp(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MycumbiatangoWidget)
{
    ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
    
    QuLabel *mylabel = new QuLabel(this, cu_t, cu_tango_r_fac);
    mylabel->setSource("$1/double_scalar"); // set Tango source
    
    // add mylabel to layout and so on...
}
\endcode

### See also

\li <a href="../html/md_src_tutorial_qumbiatango_app.html">Setting up a graphical application with cumbia and tango</a>.
\li <a href="../../cuuimake/html/md_src_cuuimake.html">Using <em>cuuimake</em></a>.

\li <a href="../../cumbia-qtcontrols/html/index.html">cumbia-qtcontrols module</a>.
\li <a href="../html/index.html">qumbia-tango-controls module</a>.
\li <a href="../../cumbia-epics/index.html">qumbia-epics module</a>.
\li <a href="../../qumbia-epics-controls/html/index.html">qumbia-epics-controls module</a>.





