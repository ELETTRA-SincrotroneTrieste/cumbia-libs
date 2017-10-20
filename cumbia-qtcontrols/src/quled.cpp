#include "quled.h"

#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cudata.h>
#include <QPainter>
#include <QPaintEvent>

#include "culinkcontrol.h"
#include "qupalette.h"
#include "cucontrolsfactories_i.h"

class QuLedPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    CuControlsReaderA *reader;
    QuPalette palette;
    CuLinkControl *link_ctrl;
};

QuLed::QuLed(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac) : ELed(w)
{
    m_init();
    d->link_ctrl = new CuLinkControl(cumbia, r_fac);
}

QuLed::QuLed(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) : ELed(w)
{
    m_init();
    d->link_ctrl = new CuLinkControl(cumbia_pool, fpool);
}

void QuLed::m_init()
{
    d = new QuLedPrivate;
    d->reader = NULL;
    d->link_ctrl = NULL;
    d->auto_configure = true;
    d->read_ok = false;
    setProperty("trueColor", QColor(Qt::green));
    setProperty("falseColor", QColor(Qt::red));
}

QuLed::~QuLed()
{
    if(d->reader) delete d->reader;
    delete d;
}

QString QuLed::source() const
{
    if(d->reader)
        return d->reader->source();
    return "";
}

void QuLed::setSource(const QString &s)
{
    if(d->reader && d->reader->source() != s)
        delete d->reader;

    d->reader = d->link_ctrl->make_reader(s.toStdString(), this);
    if(d->reader)
        d->reader->setSource(s);
}

void QuLed::unsetSource()
{
    if(d->reader)
        d->reader->unsetSource();
}

void QuLed::onUpdate(const CuData &da)
{
    QColor background, border;
    d->read_ok = !da["err"].toBool();
    setEnabled(d->read_ok);

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
