#include "qucirculargauge.h"
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
    try{
        if(da.containsKey("min"))
            setMinValue(std::stod(da["min"].toString()));
        if(da.containsKey("max"))
            setMaxValue(std::stod(da["max"].toString()));

        if(da.containsKey("min_alarm")) setLowError( std::stod(da["min_alarm"].toString()));
        if(da.containsKey("max_alarm")) setHighError( std::stod(da["max_alarm"].toString()));
        if(da.containsKey("min_warning")) setLowWarning( std::stod(da["min_warning"].toString()));
        if(da.containsKey("max_warning")) setHighWarning(std::stod(da["max_warning"].toString()));

    }
    catch(const std::invalid_argument& ia) {
        perr("QuCircularGauge.m_configure: error converting threshold values: %s",
             ia.what());
    }
}

void QuCircularGauge::onUpdate(const CuData &da)
{
    d->read_ok = !da["err"].toBool();
    setEnabled(d->read_ok);
    if(!d->read_ok)
        setLabel("####");
    setToolTip(da["msg"].toString().c_str());

    if(d->read_ok && d->auto_configure && da["type"].toString() == "property") {
        m_configure(da);
    }
    else if(d->read_ok) {
        CuVariant val = da["value"];
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
        else
            setToolTip("wrong data type " + QString::fromStdString(val.dataTypeStr(val.getType())));

    }
    emit newData(da);
}
