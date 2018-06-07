#include "qucirculargauge.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <cudatatypes_ex.h>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPaintEvent>

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
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuCircularGauge::QuCircularGauge(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    ECircularGauge(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuCircularGauge::QuCircularGauge(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    ECircularGauge(w), CuDataListener()
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
    CuContextMenu* m = new CuContextMenu(this, this);
    connect(m, SIGNAL(linkStatsTriggered(QWidget*, CuContextI *)),
            this, SIGNAL(linkStatsRequest(QWidget*, CuContextI *)));
    m->popup(e->globalPos());
}

void QuCircularGauge::m_configure(const CuData& da)
{
    QMap<int, const char*> threshs;
    threshs[CuXDType::Min] = "minValue";
    threshs[CuXDType::Max] = "maxValue";
    threshs[CuXDType::LowerWarningLimit] = "lowWarning";
    threshs[CuXDType::UpperWarningLimit] = "highWarning";
    threshs[CuXDType::UpperAlarmLimit] = "highError";
    threshs[CuXDType::LowerAlarmLimit] = "lowError";
    // map keys are not ordered!
    QList<int> props = QList<int>() << CuXDType::Min << CuXDType::Max << CuXDType::UpperAlarmLimit << CuXDType::LowerAlarmLimit
                                      << CuXDType::LowerWarningLimit << CuXDType::UpperWarningLimit;
    foreach(int thidx, props) {
        const char *name = threshs[thidx];
        try {
            if(da.containsKey(thidx)) {
                setProperty(threshs[thidx], std::stod(da[thidx].toString()));
            }
        }
        catch(const std::invalid_argument& ) {
            char bound[16] = "";
            memset(bound, 0, 16);
            strncpy(bound, name, 3);
            strncat(bound, "Value", 5);
            setProperty(threshs[thidx], property(bound).toDouble());
        }
    }
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
    printf("\e[1;33m QuCircularGauge::onUpdate da %s\e[0m\n", da.toString().c_str());
    d->read_ok = !da[CuDType::Err].toBool();
    if(!d->read_ok)
        setLabel("####");
    setToolTip(da[CuDType::Message].toString().c_str());

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da[CuDType::Message].toString());

    if(d->read_ok && d->auto_configure && da[CuDType::Type].toString() == "property") {
        m_configure(da);
    }
    if(d->read_ok && da[CuDType::Value].isValid()) {
        m_set_value(da[CuDType::Value]);
    }
    emit newData(da);
}
