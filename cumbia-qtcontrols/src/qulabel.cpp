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

/** @private */
class QuLabelPrivate
{
public:
    bool auto_configure;
    bool read_ok;
    QuPalette palette;
    int max_len;
    CuContext *context;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsReaderFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuLabel::QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory) :
    QuLabelBase(w), CuDataListener()
{
    m_init();
    d->context = new CuContext(cumbia, r_factory);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuLabel::QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) :
    QuLabelBase(w), CuDataListener()
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
    setDecoration(background, border);
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

/** \brief returns the pointer to the CuContext
 *
 * CuContext sets up the connection and is used as a mediator to send and get data
 * to and from the reader.
 *
 * @see CuContext
 */
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

void QuLabel::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = findChild<CuContextMenu *>();
    if(!m)
        m = new CuContextMenu(this, d->context);
    m->popup(e->globalPos());
}

void QuLabel::onUpdate(const CuData &da)
{
    bool background_modified;
    QString txt;
    QColor background, border;
    d->read_ok = !da["err"].toBool();

    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da["msg"].toString());

    d->read_ok ? border = d->palette["dark_green"] : border = d->palette["dark_red"];

    setToolTip(da["msg"].toString().c_str());

    if(da["err"].toBool() )
        setText("####");
    else if(da.containsKey("value"))
    {
        CuVariant val = da["value"];
        QuLabelBase::setValue(val, &background_modified);
    }

    if(da.containsKey("state_color")) {
        CuVariant v = da["state_color"];
        QuPalette p;
        background = p[QString::fromStdString(v.toString())];
        if(background.isValid())
            setBackground(background);
    }
    else if(!background_modified) {
        // background has not already been set by QuLabelBase::setValue (this happens if either a
        // boolean display or enum display have been configured)
        // if so, use the "quality_color" as a background
        if(da.containsKey("quality_color"))
            background = d->palette[QString::fromStdString(da["quality_color"].toString())];
         setBackground(background); // checks if background is valid
    }

    emit newData(da);
}
