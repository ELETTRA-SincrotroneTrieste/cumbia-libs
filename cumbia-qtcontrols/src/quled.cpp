#include "quled.h"

#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cudata.h>
#include <QPainter>
#include <QPaintEvent>

#include "cucontext.h"
#include "culinkstats.h"
#include "qupalette.h"
#include "cucontrolsfactories_i.h"

class QuLedPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    QuPalette palette;
    CuContext *context;
};

QuLed::QuLed(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac) : QuLedBase(w)
{
    m_init();
    d->context = new CuContext(cumbia, r_fac);
}

QuLed::QuLed(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) : QuLedBase(w)
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

void QuLed::m_init()
{
    d = new QuLedPrivate;
    d->auto_configure = true;
    d->read_ok = false;
    setProperty("trueColor", QColor(Qt::green));
    setProperty("falseColor", QColor(Qt::red));
}

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
    d->read_ok = !da["err"].toBool();
    setEnabled(d->read_ok);

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da["msg"].toString());

    if(da.containsKey("quality_color"))
        background = d->palette[QString::fromStdString(da["quality_color"].toString())];
    if(da.containsKey("success_color"))
        border = d->palette[QString::fromStdString(da["success_color"].toString())];

    setToolTip(da["msg"].toString().c_str());

    setDisabled(da["err"].toBool() );
    if(d->read_ok && da.containsKey("state_color"))
    {
        CuVariant v = da["state_color"];
        QuPalette p;
        setColor(p[QString::fromStdString(v.toString())]);
    }
    else if(d->read_ok && da.containsKey("value"))
    {
        CuVariant v = da["value"];
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

    if(da.containsKey("success_color"))
        setBorderColor(d->palette[QString::fromStdString(da["success_color"].toString())]);

    emit newData(da);
}

CuContext *QuLed::getContext() const
{
    return d->context;
}
