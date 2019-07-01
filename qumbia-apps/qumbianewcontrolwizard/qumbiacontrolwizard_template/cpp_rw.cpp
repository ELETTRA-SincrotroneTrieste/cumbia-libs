#include "$INCLUDE$"
#include "cucontrolswriter_abs.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QContextMenuEvent>
#include <QMetaProperty>
#include <QStringList>
#include <vector>

#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <culinkstats.h>
#include <cucontextmenu.h>
#include <cucontext.h>
#include <culog.h>
#include <cuserviceprovider.h>
#include <cuservices.h>
#include <qulogimpl.h>

/** @private */
class $MAINCLASS$Private
{
public:
    bool auto_configure;
    bool ok;
    CuContext *in_ctx, *out_ctx;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
$MAINCLASS$::$MAINCLASS$(QWidget *parent, Cumbia *cumbia,
                         const CuControlsReaderFactoryI &r_fac,
                         const CuControlsWriterFactoryI &w_fac) :
    $SUPERCLASS$(parent), CuDataListener()
{
    m_init();
    d->out_ctx = new CuContext(cumbia, r_fac);
    d->in_ctx = new CuContext(cumbia, w_fac);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
$MAINCLASS$::$MAINCLASS$(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    $SUPERCLASS$(parent), CuDataListener()
{
    m_init();
    d->out_ctx = new CuContext(cumbia_pool, fpool);
    d->in_ctx = new CuContext(cumbia_pool, fpool);
}

void $MAINCLASS$::m_init()
{
    d = new $MAINCLASS$Private;
    d->out_ctx = d->in_ctx = NULL;
    d->auto_configure = true;
    d->ok = false;
}

$MAINCLASS$::~$MAINCLASS$()
{
    pdelete("~$MAINCLASS$ %p", this);
    delete d->out_ctx;
    delete d->in_ctx;
    delete d;
}

// call this method at the end of m_configure so as to be sure that no write is
// triggered after the m_configure sets the current write value on the object
//
void $MAINCLASS$::m_create_connections() {
    // enable one of these connections or write a custom one to trigger a write operation
    //
    // ---------------------------------------------------------------------------------------------------
    //
//    connect(this, SIGNAL(valueChanged(int)), this, SLOT(write(int)));
//    connect(this, SIGNAL(valueChanged(double)), this, SLOT(write(double)));
//    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(write(const QString& )));
//    connect(this, SIGNAL(listChanged(const QStringList&)), this, SLOT(write(const QStringList&)));
//    connect(this, SIGNAL(clicked()), this, SLOT(write()));
//    connect(this, SIGNAL(apply()), this, SLOT(write()));
    //
    // ---------------------------------------------------------------------------------------------------
}

QString $MAINCLASS$::source() const {
    if(CuControlsReaderA* r = d->out_ctx->getReader())
        return r->source();
    return "";
}

/** \brief returns the pointer to the output CuContext
 *
 * Implements CuContextI interface, returns getOutputContext
 *
 * @see CuContext
 */
CuContext *$MAINCLASS$::getContext() const
{
    return d->out_ctx;
}

/** \brief Returns the reader's context
 *
 * @return a pointer to the output CuContext used to read.
 */
CuContext *$MAINCLASS$::getOutputContext() const
{
    return d->out_ctx;

}

/** \brief Returns the writer's context
 *
 * @return a pointer to the input CuContext used to write.
 */
CuContext *$MAINCLASS$::getInputContext() const
{
    return d->in_ctx;
}

/** \brief Connect the reader to the specified source.
 *
 * If a reader with a different source is configured, it is deleted.
 * If options have been set with QuContext::setOptions, they are used to set up the reader as desired.
 *
 * @see QuContext::setOptions
 * @see source
 */
void $MAINCLASS$::setSource(const QString &s)
{
    CuControlsReaderA * r = d->out_ctx->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
    CuControlsWriterA *w = d->in_ctx->replace_writer(s.toStdString(), this);
    if(w)
        w->setTarget(s);
}

void $MAINCLASS$::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = findChild<CuContextMenu *>();
    if(!m) m = new CuContextMenu(this, d->context);
    m->popup(e->globalPos());
}

void $MAINCLASS$::m_configure(const CuData& da)
{
    QString description, unit, label;
    CuVariant m, M;

    m = da["min"];  // min value
    M = da["max"];  // max value

    unit = QString::fromStdString(da["display_unit"].toString());
    label = QString::fromStdString(da["label"].toString());

    // 1.
    // if this class has minimum / maximum properties:
    m_try_write_property("minimum", m);
    m_try_write_property("maximum", M);

    // 2. the manual way, more efficient
    // 2a. double
    /*
        setMinimum(m.toDouble());
        setMaximum(M.toDouble());
     */

    // 2b. int
    /*
        setMinimum(m.toInt());
        setMaximum(M.toInt());
    */

    // convert minimum and maximum to custom types (define MYTYPE with the desired type)
    // ---------------------------------
    bool ok;
#define MYTYPE short int
    MYTYPE mymin, mymax;
    ok = m.to<MYTYPE>(mymin);
    if(ok)
        ok = M.to<MYTYPE>(mymax);

    // if(ok) {
    // setMinimum(mymin);
    // setMaximum(mymax);
    // }

#undef MYTYPE
    // ---------------------------------


    // need min and max as string?
    // ---------------------------------
    // comment if not needed
    std::string smin = m.toString();
    std::string smax = M.toString();
    printf("MyLineEdit.m_configure: minimum and maximum as strings: m: %s M:%s\n", smin.c_str(), smax.c_str());
    // ---------------------------------


    description = QString::fromStdString(da["description"].toString());
    setProperty("description", description);
    setProperty("unit", unit);

    // need these?
    /*
    CuVariant mw, Mw, ma, Ma;
    mw = da["min_warning"];
    Mw = da["max_warning"];
    ma = da["min_alarm"];
    Ma = da["max_alarm"];
    */

    //
    // refer to documentation in
    // file:///usr/local/cumbia-libs/share/doc/cumbia-tango/html/cudata_for_tango.html
    // for a detailed CuData bundle description for data exchange with the Tango world.
    //


    // initialise the object with the "write" value (also called "set point"), if available:
    //
    if(d->ok && da.containsKey("w_value")) {
        printf("$MAINCLASS$.m_configure: set point value for %s is %s\n", da["src"].toString().c_str(), da["w_value"].toString().c_str());
    }

    // if this method is also used to initialise a write value (also called "set point")
    // setup signal/slot connections right here at the end of the function body
    m_create_connections();
}


void $MAINCLASS$::onUpdate(const CuData &da)
{
    bool write_op = da["activity"].toString() == "writer";
    d->ok = !da["err"].toBool();
    setEnabled(d->ok);
    setToolTip(QString::fromStdString(da["msg"].toString()));

    // update link statistics
    CuContext *ctx;
    write_op ? ctx = d->in_ctx : ctx = d->out_ctx;
    ctx->getLinkStats()->addOperation();
    if(!d->ok)
        ctx->getLinkStats()->addError(da["msg"].toString());

    // if not ok, show the popup only if the failed operation is a write operation
    if(!d->ok && write_op) {
        perr("$MAINCLASS$ [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da["src"].toString().c_str(), da["msg"].toString().c_str(),
                da["data_format_str"].toString().c_str(), da["writable"].toInt());

        Cumbia* cumbia = d->in_ctx->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->in_ctx->cumbiaPool()->getBySrc(da["src"].toString());
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
            log->write(QString("$MAINCLASS$ [" + objectName() + "]").toStdString(), da["msg"].toString(), CuLog::Error, CuLog::Write);
        }
    }
    else if(d->auto_configure && da.has("type", "property")) {
        //
        // --------------------------------------------------------------------------------------------
        // You may want to check data format and write type and issue a warning or avoid configuration
        // at all if they are not as expected
        // if(da["data_format_str"] == "scalar" && da["writable"].toInt() > 0)
        //
        // --------------------------------------------------------------------------------------------
        m_configure(da);
    }

    if(da.containsKey("value")) {
        m_set_value(da["value"]);
    }
    emit newData(da);
}

/** \brief write a double value to the target
 *
 * @param d the value to be written on the target
 */
void $MAINCLASS$::write(double val) {
    m_write(CuVariant(val));
}

/** \brief write an integer to the target
 *
 * @param i the value to be written on the target
 */
void $MAINCLASS$::write(int ival) {
    m_write(CuVariant(ival));
}

/** \brief write a boolean to the target
 *
 * @param i the boolean value to be written on the target
 */
void $MAINCLASS$::write(bool bval) {
    m_write(CuVariant(bval));
}

/** \brief write a list of strings to the target
 *
 * @param s the QStringList to be written on the target
 */
void $MAINCLASS$::write(const QStringList& sl) {
    std::vector<std::string> vs;
    foreach(QString s, sl)
        vs.push_back(s.toStdString());
    m_write(CuVariant(vs));
}

/** \brief write a string to the target
 *
 * @param s the string to be written on the target
 */
void $MAINCLASS$::write(const QString& s) {
    m_write(CuVariant(s.toStdString()));
}

/** \brief write an value that is displayed and edited in this widget
 *
 */
void $MAINCLASS$::write() {
    double d = 1.0; // get the value somewhere from this widget
    m_write(CuVariant(d));
}

// perform the write operation on the target
//
void $MAINCLASS$::m_write(const CuVariant& v){
    CuControlsWriterA *w = d->in_ctx->getWriter();
    if(w) {
        w->setArgs(v);
        w->execute();
    }
}

// val is the content of "w_value" key in the data bundle.
// Initialize the displayed value of the object with it.
void $MAINCLASS$::m_set_value(const CuVariant& val) {
    int idx = -1;
    // 1. setProperty method
    // ---------------------------------------------------------------------------------
    //    the automatic way, less efficient, works if the object has the
    //    following properties
    //
    // NOTE: remove this if you use method 2
    QStringList properties = QStringList() << "value" << "text" << "currentText" << "currentIndex";
    for(int i = 0; i < properties.size() && idx < 0; i++)
        idx = m_try_write_property(properties.at(i), val);
    // ----------------------------------- method 1 ends ----------------------------------------------


    // 2. Data type specific method
    // ---------------------------------------------------------------------------------
    //
    // comment/uncomment as needed
    //
    // Remove the setProperty method above if you use the following code

    if(idx < 0) { // no property found
        if(val.isInteger()) {
            int i;
            val.to<int>(i);
            // setIntValue(i);
        }
        else if(val.isFloatingPoint()) {
            double d;
            val.to<double>(d);
            // setDoubleValue(d);
        }
        else {
            // std::string as_string = val.toString();
            // setText(QString::fromStdString(as_string));
            // or if string is wrong
            setToolTip("wrong data type " + QString::fromStdString(val.dataTypeStr(val.getType())));
        }
    }

    // ----------------------------------- method 2 ends ----------------------------------------------
}

int $MAINCLASS$::m_try_write_property(const QString& propnam, const CuVariant &val)
{
    bool success = false;
    int idx;
    idx = metaObject()->indexOfProperty(propnam.toStdString().c_str());
    if(idx > -1) {
        switch(metaObject()->property(idx).type()) {
        case QVariant::Double:
            success = setProperty(propnam.toStdString().c_str(), val.toDouble());
            break;
        case QVariant::Int:
            success = setProperty(propnam.toStdString().c_str(), val.toInt());
            break;
        case QVariant::String:
            success = setProperty(propnam.toStdString().c_str(), QString::fromStdString(val.toString()));
            break;
        default:
            idx = -1;
            break;
        }
    }
    if(!success)
        return -1;
    return idx;
}
