#include "quled.h"

#include "cucontrolsreader_abs.h"
#include <cumacros.h>
#include <cudata.h>
#include <QPainter>
#include <QPaintEvent>
#include <quapplication.h>
#include <cucontrolsutils.h>

#include "cucontext.h"
#include "culinkstats.h"
#include "qupalette.h"
#include "cucontrolsfactories_i.h"
#include "cucontextmenu.h"
#include "cuengine_swap.h"

/** @private */
class QuLedPrivate
{
public:
    bool auto_configure;
    bool read_ok, value;
    QuPalette palette;
    CuContext *context;
    CuControlsUtils u;
    char msg[MSGLEN];
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

/*!
 * \brief Classical, single parent-widget constructor. *QuApplication* properly initialized with
 *        cumbia engine objects is compulsory.
 *
 * \param parent widget
 * \par Important note: cumbia engine references are obtained from the QuApplication instance.
 *      For best performance, static cast of QCoreApplication::instance() to QuApplication is
 *      used.
 * \since cumbia 2.1
 */
QuLed::QuLed(QWidget *w) : QuLedBase(w) {
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    m_init();
    d->context = new CuContext(a->cumbiaPool(), *a->fpool());
}

/*! @private */
void QuLed::m_init()
{
    d = new QuLedPrivate;
    d->auto_configure = true;
    d->read_ok = d->value = false;
    setProperty("trueColor", QColor(Qt::green));
    setProperty("falseColor", QColor(Qt::red));
    d->msg[0] = '\0';
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

bool QuLed::value() {
    return d->value;
}

void QuLed::setValue(bool v) {
    d->value = v;
    setColor(v ? property("trueColor").value<QColor>()
               : property("falseColor").value<QColor>());
    emit valueChanged(v);
}

bool QuLed::ctxSwap(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) {
    CuEngineSwap csw;
    d->context = csw.replace(this, d->context, cumbia_pool, fpool);
    return csw.ok();
}

void QuLed::onUpdate(const CuData &da)
{
    QColor background, border;
    d->read_ok = !da[TTT::Err].toBool();  // da["err"]
    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->read_ok)
        d->context->getLinkStats()->addError(da[TTT::Message].toString());  // da["msg"]

    if(da.containsKey(TTT::QualityColor))  // da.containsKey("qc")
        background = d->palette[QString::fromStdString(da[TTT::QualityColor].toString())];  // da["qc"]
    if(da.containsKey(TTT::Color))  // da.containsKey("color")
        border = d->palette[QString::fromStdString(da[TTT::Color].toString())];  // da["color"]

    setDisabled(da[TTT::Err].toBool() );  // da["err"]
    if(d->read_ok && da.containsKey(TTT::StateColor))  // da.containsKey("sc")
    {
        CuVariant v = da[TTT::StateColor];  // da["sc"]
        setColor(d->palette[QString::fromStdString(v.toString())]);
    }
    else if(d->read_ok && da.containsKey(TTT::Value))  // da.containsKey("value")
    {
        CuVariant v = da[TTT::Value];  // da["value"]
        switch (v.getType()) {
        case CuVariant::Boolean:
            setValue(v.b());
            break;
        default:
            break;
        }
    }
    else if(!d->read_ok) {
        setColor(QColor(Qt::gray));
        setToolTip(da.s(TTT::Message).c_str());
    }
    if(d->read_ok) {
        d->u.msg_short(da, d->msg);
        setToolTip(d->msg);
    }

    if(da.containsKey(TTT::Color))  // da.containsKey("color")
        setBorderColor(d->palette[QString::fromStdString(da[TTT::Color].toString())]);  // da["color"]

    emit newData(da);
}

CuContext *QuLed::getContext() const
{
    return d->context;
}

/** \brief change the association between color names and color values
 *
 * The QuLed behavior is equivalent to QuLabel's
 *
 * @see QuLabel::setQuPalette
 * @see quPalette
 */
void QuLed::setQuPalette(const QuPalette &colors) {
    d->palette = colors;
}

/** \brief returns the QuPalette in use
 *
 * @see QuLabel::quPalette
 * @see setQuPalette
 */
QuPalette QuLed::quPalette() const {
    return d->palette;
}

void QuLed::contextMenuEvent(QContextMenuEvent *e) {
    CuContextMenu* m = new CuContextMenu(this);
    m->popup(e->globalPos(), this); // menu auto deletes after exec
}

