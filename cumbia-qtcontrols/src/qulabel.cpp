#include "qulabel.h"
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

class QuLabelPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    QuPalette palette;
    int max_len;
    CuContext *context;
};

QuLabel::QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    ESimpleLabel(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
}

QuLabel::QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    ESimpleLabel(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

void QuLabel::m_init()
{
    d = new QuLabelPrivate;
    d->context = NULL;
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
    delete d->context;
    delete d;
}

QString QuLabel::source() const
{
    if(CuControlsReaderA* r = d->context->getReader())
        return r->source();
    return "";
}

int QuLabel::maximumLength() const
{
    return d->max_len;
}

CuContext *QuLabel::getContext() const
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
void QuLabel::setSource(const QString &s)
{
    CuControlsReaderA * r = d->context->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

void QuLabel::unsetSource()
{
    d->context->disposeReader();
}

void QuLabel::setMaximumLength(int len)
{
    d->max_len = len;
}

void QuLabel::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = new CuContextMenu(this, this);
    connect(m, SIGNAL(linkStatsTriggered(QWidget*, CuContextWidgetI *)),
            this, SIGNAL(linkStatsRequest(QWidget*, CuContextWidgetI *)));
    m->popup(e->globalPos());
}

void QuLabel::onUpdate(const CuData &da)
{
    QString txt;
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
