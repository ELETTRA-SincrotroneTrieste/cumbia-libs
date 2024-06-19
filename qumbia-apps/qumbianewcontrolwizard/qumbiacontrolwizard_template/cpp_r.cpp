#include "$INCLUDE$"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QContextMenuEvent>
#include <QMetaProperty>

#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"

/** @private */
class $MAINCLASS$Private
{
public:
    bool auto_configure;
    bool read_ok;
    CuContext *context;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
$MAINCLASS$::$MAINCLASS$(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    $SUPERCLASS$(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
$MAINCLASS$::$MAINCLASS$(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    $SUPERCLASS$(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

void $MAINCLASS$::m_init()
{
    d = new $MAINCLASS$Private;
    d->context = NULL;
    d->auto_configure = true;
    d->read_ok = false;
}

$MAINCLASS$::~$MAINCLASS$()
{
    pdelete("~$MAINCLASS$ %p", this);
    delete d->context;
    delete d;
}

QString $MAINCLASS$::source() const
{
    if(CuControlsReaderA* r = d->context->getReader())
        return r->source();
    return "";
}

/** \brief returns the pointer to the CuContext
 *
 * CuContext sets up the connection and is used as a mediator to send and get data
 * to and from the reader.
 *
 * @see CuContext
 */
CuContext *$MAINCLASS$::getContext() const
{
    return d->context;
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
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void $MAINCLASS$::unsetSource()
{
    d->context->disposeReader();
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
    label = QString::fromStdString(da[TTT::Label].toString());  // da["label"]

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


    description = QString::fromStdString(da[TTT::Description].toString());  // da["description"]
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
}


void $MAINCLASS$::onUpdate(const CuData &da)
{
    QString message = QString::fromStdString(da[TTT::Message].toString());  // da["msg"]
    d->read_ok = !da[TTT::Err].toBool();  // da["err"]

    setEnabled(d->read_ok);
    setToolTip(message);

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da[TTT::Message].toString());  // da["msg"]

    // configure object if the type of received data is "property"
    if(d->read_ok && d->auto_configure && da[TTT::Type].toString() == "property") {  // da["type"]
        m_configure(da);
    }

    // set value, if available

    CuVariant val = da[TTT::Value];  // da["value"]


    if(d->read_ok && val.isValid()) {
        int idx = -1;

        // 1. setProperty method
        // ---------------------------------------------------------------------------------
        //    the automatic way, less efficient, works if the object has the
        //    following properties
        //
        // NOTE: remove this if you use method 2
        QStringList properties = QStringList() << "value" << "text" << "currentText";
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
    emit newData(da);

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
