#include "quled.h"

#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cudata.h>
#include <cudatatypes_ex.h>
#include <QPainter>
#include <QPaintEvent>

#include "cucontext.h"
#include "culinkstats.h"
#include "qupalette.h"
#include "cucontrolsfactories_i.h"

/** @private */
class QuLedPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    QuPalette palette;
    CuContext *context;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */

QuLed::QuLed(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac) : QuLedBase(w)
{
    m_init();
    d->context = new CuContext(cumbia, r_fac);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuLed::QuLed(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) : QuLedBase(w)
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

/*! @private */
void QuLed::m_init()
{
    d = new QuLedPrivate;
    d->auto_configure = true;
    d->read_ok = false;
    setProperty("trueColor", QColor(Qt::green));
    setProperty("falseColor", QColor(Qt::red));
}

/*! \brief the class destructor
 *
 * deletes the CuContext
 */
QuLed::~QuLed()
{
    delete d->context;
    delete d;
}

QString QuLed::source() const
{
    if(d->context->getReader())
        return d->context->getReader()->source();
    return "";
}

void QuLed::setSource(const QString &s)
{
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void QuLed::unsetSource()
{
    d->context->disposeReader();
}

void QuLed::onUpdate(const CuData &da)
{
    QColor background, border;
    d->read_ok = !da[CuDType::Err].toBool();

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da[CuDType::Message].toString());

    if(da.containsKey(CuXDType::QualityColor))
        background = d->palette[QString::fromStdString(da[CuXDType::QualityColor].toString())];
    if(da.containsKey(CuXDType::SuccessColor))
        border = d->palette[QString::fromStdString(da[CuXDType::SuccessColor].toString())];

    setToolTip(da[CuDType::Message].toString().c_str());

    setDisabled(da[CuDType::Err].toBool() );
    if(d->read_ok && da.containsKey(CuXDType::StateColor))
    {
        CuVariant v = da[CuXDType::StateColor];
        QuPalette p;
        setColor(p[QString::fromStdString(v.toString())]);
    }
    else if(d->read_ok && da.containsKey(CuDType::Value))
    {
        CuVariant v = da[CuDType::Value];
        switch (v.getType()) {
            case CuVariant::Boolean:
                setColor(v.toBool() ? property("trueColor").value<QColor>()
                                    : property("falseColor").value<QColor>());
            break;
        default:
            break;
        }
    }
    else if(!d->read_ok)
        setColor(QColor(Qt::gray));

    if(da.containsKey(CuXDType::SuccessColor))
        setBorderColor(d->palette[QString::fromStdString(da[CuXDType::SuccessColor].toString())]);

    emit newData(da);
}

CuContext *QuLed::getContext() const
{
    return d->context;
}
