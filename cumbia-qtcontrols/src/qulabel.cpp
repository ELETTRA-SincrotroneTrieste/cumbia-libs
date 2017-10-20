#include "qulabel.h"
#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <QPainter>
#include <QPaintEvent>

#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsfactorypool.h"
#include "culinkcontrol.h"

class QuLabelPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    CuControlsReaderA *reader;
    QuPalette palette;
    int max_len;
    CuLinkControl *link_ctrl;
};

QuLabel::QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    ESimpleLabel(w), CuDataListener()
{
    m_init();
    d->link_ctrl = new CuLinkControl(cumbia, r_factory);
}

QuLabel::QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    ESimpleLabel(w), CuDataListener()
{
    m_init();
    d->link_ctrl = new CuLinkControl(cumbia_pool, fpool);
}

void QuLabel::m_init()
{
    d = new QuLabelPrivate;
    d->link_ctrl = NULL;
    d->reader = NULL;
    d->auto_configure = true;
    d->read_ok = false;
    d->max_len = -1;
    setWordWrap(true);
    setProperty("trueString", "TRUE");
    setProperty("falseString", "FALSE");
    setProperty("trueColor", QColor(Qt::green));
    setProperty("falseColor", QColor(Qt::red));
    QColor background = d->palette["white"];
    QColor border = d->palette["gray"];
    decorate(background, border);
}

QuLabel::~QuLabel()
{
    pdelete("~QuLabel %p", this);
    delete d->link_ctrl;
    if(d->reader)
        delete d->reader;
    delete d;
}

QString QuLabel::source() const
{
    if(d->reader)
        return d->reader->source();
    return "";
}

int QuLabel::maximumLength() const
{
    return d->max_len;
}

void QuLabel::setSource(const QString &s)
{
    if(d->reader && d->reader->source() != s)
        delete d->reader;

    d->reader = d->link_ctrl->make_reader(s.toStdString(), this);
    if(d->reader)
        d->reader->setSource(s);
}

void QuLabel::unsetSource()
{
    if(d->reader)
        d->reader->unsetSource();
}

void QuLabel::setMaximumLength(int len)
{
    d->max_len = len;
}

void QuLabel::onUpdate(const CuData &da)
{
    QString txt;
    QColor background, border;
    d->read_ok = !da["err"].toBool();
    setEnabled(d->read_ok);

    if(da.containsKey("quality_color"))
        background = d->palette[QString::fromStdString(da["quality_color"].toString())];
    if(da.containsKey("success_color"))
        border = d->palette[QString::fromStdString(da["success_color"].toString())];

    setToolTip(da["msg"].toString().c_str());

    if(da["err"].toBool() )
        setText("####");
    else if(da.containsKey("value"))
    {
        CuVariant val = da["value"];
        if(val.getType() == CuVariant::Boolean)
        {
            txt = (val.toBool() ? property("trueString").toBool() : property("falseString").toBool());
            background = val.toBool() ? property("trueColor").value<QColor>() : property("falseColor").value<QColor>();
        }
        else
        {
            txt = QString::fromStdString(da["value"].toString());
            if(d->max_len > -1 && txt.length() > d->max_len)
            {
                setToolTip(toolTip() + "\n\n" + txt);
                txt.truncate(d->max_len);
            }
        }
        setText(txt);
    }

    if(da.containsKey("state_color"))
    {
        CuVariant v = da["state_color"];
        QuPalette p;
        background = p[QString::fromStdString(v.toString())];
    }
    decorate(background, border);
    emit newData(da);
}
