#include "qulineargauge.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPaintEvent>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkstats.h"
#include "cucontextmenu.h"
#include "cucontext.h"
#include "cuengine_swap.h"

/** @private */
class QuLinearGaugePrivate
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
QuLinearGauge::QuLinearGauge(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    QuLinearGaugeBase(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuLinearGauge::QuLinearGauge(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    QuLinearGaugeBase(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

void QuLinearGauge::m_init()
{
    d = new QuLinearGaugePrivate;
    d->context = NULL;
    d->auto_configure = true;
    d->read_ok = false;
}

QuLinearGauge::~QuLinearGauge()
{
    pdelete("~QuLinearGauge %p", this);
    delete d->context;
    delete d;
}

QString QuLinearGauge::source() const
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
CuContext *QuLinearGauge::getContext() const
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
void QuLinearGauge::setSource(const QString &s)
{
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void QuLinearGauge::unsetSource()
{
    d->context->disposeReader();
}

void QuLinearGauge::ctxSwap(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) {
    d->context = CuEngineSwap().replace(this, d->context, cumbia_pool, fpool);
}

void QuLinearGauge::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = findChild<CuContextMenu *>();
    if(!m)
        m = new CuContextMenu(this);
    m->popup(e->globalPos(), this);
}

void QuLinearGauge::m_configure(const CuData& da)
{
    QMap<QString, const char*> threshs;
    threshs["min"] = "minValue"; // !cudata
    threshs["max"] = "maxValue"; // !cudata
    threshs["min_warning"] = "lowWarning";
    threshs["max_warning"] = "highWarning";
    threshs["max_alarm"] = "highError";
    threshs["min_alarm"] = "lowError";
    // map keys are not ordered!
    QStringList props = QStringList() << "min" << "max" << "max_alarm" << "min_alarm"
                                      << "min_warning" << "max_warning";
    foreach(QString thnam, props) {
        const char *name = thnam.toStdString().c_str();
        try {
            if(da.containsKey(name)) {
                setProperty(threshs[thnam], strtod(da[name].toString().c_str(), NULL));
            }
        }
        catch(const std::invalid_argument& ) {
            char bound[16] = "";
            memset(bound, 0, 16);
            strncpy(bound, name, 3);
            strncat(bound, "Value", 6);
            setProperty(threshs[thnam], property(bound).toDouble());
        }
    }
    if(da["display_unit"].toString().length() > 0)
        setUnit(QString::fromStdString(da["display_unit"].toString()));
}

void QuLinearGauge::m_set_value(const CuVariant &val)
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

void QuLinearGauge::onUpdate(const CuData &da)
{
    d->read_ok = !da[CuDType::Err].toBool();  // da["err"]
    setReadError(!d->read_ok);
    setToolTip(da[CuDType::Message].toString().c_str());  // da["msg"]

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da[CuDType::Message].toString());  // da["msg"]

    if(d->read_ok && d->auto_configure && da[CuDType::Type].toString() == "property") {  // da["type"]
        m_configure(da);
    }
    if(d->read_ok && da[CuDType::Value].isValid()) {  // da["value"]
        m_set_value(da[CuDType::Value]);  // da["value"]
    }
    emit newData(da);
}
