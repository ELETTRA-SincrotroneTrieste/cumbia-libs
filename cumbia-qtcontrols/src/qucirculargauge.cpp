#include "qucirculargauge.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QtDebug>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"

/** @private */
class QuCircularGaugePrivate
{
public:
    bool auto_configure;
    bool read_ok;
    CuContext *context;
    CuVariant prev_val;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuCircularGauge::QuCircularGauge(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    QuCircularGaugeBase(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuCircularGauge::QuCircularGauge(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    QuCircularGaugeBase(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

void QuCircularGauge::m_init()
{
    d = new QuCircularGaugePrivate;
    d->context = NULL;
    d->auto_configure = true;
    d->read_ok = false;
}

QuCircularGauge::~QuCircularGauge()
{
    pdelete("~QuCircularGauge %p", this);
    delete d->context;
    delete d;
}

QString QuCircularGauge::source() const
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
CuContext *QuCircularGauge::getContext() const
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
void QuCircularGauge::setSource(const QString &s)
{
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void QuCircularGauge::unsetSource()
{
    d->context->disposeReader();
}

void QuCircularGauge::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = findChild<CuContextMenu *>();
    if(!m)
        m = new CuContextMenu(this, d->context);
    m->popup(e->globalPos());
}

void QuCircularGauge::m_configure(const CuData& da)
{
    QMap<QString, const char*> threshs;
    threshs["min"] = "minValue";
    threshs["max"] = "maxValue";
    threshs["min_warning"] = "lowWarning";
    threshs["max_warning"] = "highWarning";
    threshs["max_alarm"] = "highError";
    threshs["min_alarm"] = "lowError";
    // avoid cache regeneration at every property change
    setCacheRegenerationDisabled(true);
    // map keys are not ordered!
    QStringList props = QStringList() << "min" << "max" << "max_alarm" << "min_alarm"
                                      << "min_warning" << "max_warning";
    foreach(QString thnam, props) {
        const char *name = thnam.toStdString().c_str();
        try {
            if(da.containsKey(name)) {
                setProperty(threshs[thnam], std::stod(da[name].toString().c_str()));
            }
        }
        catch(const std::invalid_argument& ) {
            // do not change the property
            //            char bound[16] = "";
            //            memset(bound, 0, 16);
            //            strncpy(bound, name, 3);
            //            strncat(bound, "Value", 6);
            //            qDebug() << __PRETTY_FUNCTION__ <<" setting prop " << thnam << "bound" << bound
            //                     << " to " << property(bound).toDouble();
            //            setProperty(threshs[thnam], property(bound).toDouble());
        }
    }
    if(da["display_unit"].toString().length() > 0)
        setUnit(QString::fromStdString(da["display_unit"].toString()));
    if(da["format"].toString().length() > 0)
        setFormatProperty(QString::fromStdString(da["format"].toString()));
    setCacheRegenerationDisabled(false);
    regenerateCache();
}

void QuCircularGauge::m_set_value(const CuVariant &val)
{
    if(val.isInteger()) {
        int i;
        val.to<int>(i);
        setValue(i);
    }
    else if(val.isFloatingPoint()) {
        double d;
        val.to<double>(d);
        setValue(d);
    }
    else {
        setToolTip("wrong data type " + QString::fromStdString(val.dataTypeStr(val.getType())));
    }

}

void QuCircularGauge::onUpdate(const CuData &da)
{
    d->read_ok = !da["err"].toBool();
    setReadError(!d->read_ok);
    if(!d->read_ok)
        setLabel("####");
    setToolTip(da["msg"].toString().c_str());

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da["msg"].toString());

    if(d->read_ok && d->auto_configure && da["type"].toString() == "property") {
        m_configure(da);
    }
    if(d->read_ok && da["value"].isValid()) {
        m_set_value(da["value"]);
    }
    emit newData(da);
}
