#include "qucirculargauge.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QtDebug>
#include <cucontrolsutils.h>
#include <QToolTip>

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
    CuControlsUtils u;
    char msg[MSGLEN];
    CuData last_d;
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
    d->msg[0] = '\0';
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

void QuCircularGauge::contextMenuEvent(QContextMenuEvent *e) {
    CuContextMenu* m = new CuContextMenu(this);
    m->popup(e->globalPos(), this); // menu auto deletes after exec
}

void QuCircularGauge::m_configure(const CuData& da)
{
    QMap<QString, const char*> threshs;
    threshs["min_warning"] = "lowWarning";
    threshs["max_warning"] = "highWarning";
    threshs["max_alarm"] = "highError";
    threshs["min_alarm"] = "lowError";
    char *endptr;
    const char *str;
    double val;
    // avoid cache regeneration at every property change
    setCacheRegenerationDisabled(true);

    double m;
    const CuVariant& min = da.containsKey(TTT::Min) ? da[TTT::Min] : CuVariant();
    const CuVariant& max = da.containsKey(TTT::Max) ? da[TTT::Max] : CuVariant();
    if(min.isValid() && min.to<double>(m))
        setProperty("minValue", m);
    if(max.isValid() && max.to<double>(m))
        setProperty("maxValue", m);


    // map keys are not ordered!
    QStringList props = QStringList() << "max_alarm" << "min_alarm" << "min_warning" << "max_warning";
    foreach(QString thnam, props) {
        const std::string name = thnam.toStdString();
        if(da.containsKey(name)) {
            str = da[name].toString().c_str();
            val = strtod(da[name].toString().c_str(), &endptr);
            if(endptr != str) { // no conversion performed
                setProperty(threshs[thnam], val);
            }
        }
    }
    if(da["display_unit"].toString().length() > 0)
        setUnit(QString::fromStdString(da["display_unit"].toString()));
    if(da[TTT::NumberFormat].toString().length() > 0)  // da["format"]
        setFormatProperty(QString::fromStdString(da[TTT::NumberFormat].toString()));  // da["format"]

    setCacheRegenerationDisabled(false);
    regenerateCache();
}

void QuCircularGauge::m_set_value(const CuVariant &val)
{
    if(val.isInteger()) {
        int i = 0;
        val.to<int>(i);
        setValue(i);
    }
    else if(val.isFloatingPoint()) {
        double d = 0.0;
        val.to<double>(d);
        setValue(d);
    }
    else {
        setToolTip("wrong data type " + QString::fromStdString(val.dataTypeStr(val.getType())));
    }

}

void QuCircularGauge::onUpdate(const CuData &da)
{
    d->read_ok = !da[TTT::Err].toBool();

    const char *mode = da[TTT::Mode].c_str();
    bool event = mode != nullptr && strcmp(mode, "E") == 0;
    bool update = event; // if data is delivered by an event, always refresh
    if(!event) { // data generated periodically by a poller or by something else
        // update label if value changed
        update = !d->read_ok || d->last_d[TTT::Value] != da[TTT::Value];
        // onUpdate measures better with d->last_d = da; than with d->last_d = da.clone()
        // even though measured alone the clone version performs better
        //        d->last_d = da.clone(); // clone does a copy, then contents moved into last_d
        d->last_d = da;
        if(strlen(d->msg) > 0)
            d->msg[0] = 0; // clear msg
    }
    else { // "event" mode
        d->u.msg_short(da, d->msg);
    }

    setReadError(!d->read_ok);
    d->read_ok ? setLabel("") : setLabel(labelErrorText());

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da.c_str(TTT::Message));

    if(d->read_ok && d->auto_configure && da[TTT::Type].toString() == "property") {
        m_configure(da);
    }
    if(/*update && */d->read_ok && da[TTT::Value].isValid()) {  // da["value"]
        m_set_value(da[TTT::Value]);  // da["value"]
    }
    emit newData(da);
}

bool QuCircularGauge::event(QEvent *e) {
    if(e->type() == QEvent::ToolTip) {
        if(strlen(d->msg) == 0) {
            d->u.msg_short(d->last_d, d->msg);
            pretty_pri("buildging msg_short '%s' last data %s", d->msg, datos(d->last_d));
        }
        QToolTip::showText(static_cast<QHelpEvent *>(e)->globalPos(), d->msg);
        return true;
    }
    return QuCircularGaugeBase::event(e);
}

