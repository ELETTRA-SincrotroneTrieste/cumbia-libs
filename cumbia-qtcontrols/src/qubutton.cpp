#include "qubutton.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "cumbiapool.h"
#include "cucontext.h"
#include "qupalette.h"
#include "qulogimpl.h"
#include "quanimation.h"
#include "cucontextmenu.h"
#include "culinkstats.h"
#include <QtDebug>
#include <QPaintEvent>
#include <QPainter>

/// @private
class QuButtonPrivate
{
public:
    CuContext *context;
    bool auto_configure;
    bool write_ok;
    QuPalette palette;
    CuLog *log;
    float anim_draw_penwidthF;
    QuAnimation animation;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsWriterFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuButton::QuButton(QWidget *parent,
                   Cumbia *cumbia,
                   const CuControlsWriterFactoryI &w_fac, const QString &text) : QPushButton(parent)
{
    m_init(text);
    d->context = new CuContext(cumbia, w_fac);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuButton::QuButton(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, const QString &text)
    : QPushButton(w)
{
    m_init(text);
    d->context = new CuContext(cumbia_pool, fpool);
}

/** \brief the class destructor.
 *
 * Deletes the context.
 */
QuButton::~QuButton()
{
    delete d->context;
    delete d;
}

/// @private
void QuButton::m_init(const QString& text)
{
    d = new QuButtonPrivate;
    connect(this, SIGNAL(clicked()), this, SLOT(execute()));
    d->auto_configure = true;
    d->write_ok = false;
    d->anim_draw_penwidthF = 2.0;
    setText(text);
    d->animation.setType(QuAnimation::RayOfLight);
}

/*! \brief executes the target specified with setTarget
 *
 * If target contains an expression, this is evaluated and then execution is performed.
 *
 */
void QuButton::execute()
{
    d->animation.setPenColor(QColor(Qt::gray));
    d->animation.loop(QuAnimation::RayOfLight);
    CuControlsUtils cu;
    CuVariant args = cu.getArgs(target(), this);
    CuControlsWriterA *w = d->context->getWriter();
    if(w) {
        w->setArgs(args);
        w->execute();
    }
}

/** \brief Set the name of the target that will be executed.
 *
 * Refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
void QuButton::setTarget(const QString &target)
{
    CuControlsWriterA * w = d->context->replace_writer(target.toStdString(), this);
    if(w) w->setTarget(target);
}

/*! \brief This slot is invoked by QuAnimation when the animation value changes. Calls *update*
 *         to draw the animation.
 *
 *  @see QuAnimation::installOn
 */
void QuButton::onAnimationValueChanged(const QVariant &)
{
    // optimize animation paint area with the clipped rect provided by QuAnimation::clippedRect
    update(d->animation.clippedRect(rect()));
}

void QuButton::paintEvent(QPaintEvent *pe)
{
    QPushButton::paintEvent(pe);
    if(d->animation.currentValue() != 0.0) {
        d->animation.draw(this, pe->rect());
    }
}

/** \brief get the name of the target
 *
 * Refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 *
 * @return the target name, or an empty string if setTarget hasn't been called yet.
 */
QString QuButton::target() const
{
    if(d->context->getWriter())
        return d->context->getWriter()->target();
    return "";
}

/** \brief the onUpdate method implementation for QuButton that can be overridden
 *         by subclasses.
 *
 * This method simply writes a log message if an error occurs.
 *
 * Implements CuDataListener::onUpdate.
 */
void QuButton::onUpdate(const CuData &data)
{
    bool is_config = data.has("type", "property");
    if(!data["is_result"].toBool() && !is_config)
        return;

    d->write_ok = !data["err"].toBool();
    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->write_ok)
        d->context->getLinkStats()->addError(data["msg"].toString());

    if(!d->write_ok)
    {
        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(data["src"].toString());
        CuLog *log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log));
        if(log)
            log->write(QString("QuButton [" + objectName() + "]").toStdString(), data["msg"].toString(), CuLog::Error, CuLog::Write);
    }
    else if(is_config) {
        CuControlsUtils cu;
        cu.initObjects(target(), this, data, "w_value");
        CuControlsWriterA *w = d->context->getWriter();
        if(w)
            w->saveConfiguration(data);
        d->animation.installOn(this);
    }
    d->write_ok ? d->animation.setPenColor(QColor(Qt::green)) : d->animation.setPenColor(QColor(Qt::red));
    d->write_ok ? d->animation.setDuration(1500) : d->animation.setDuration(3000);
    d->animation.start();
    emit newData(data);
}

/** \brief Returns a pointer to the CuContext in use.
 *
 * @return a pointer to CuContext
 */
CuContext *QuButton::getContext() const
{
    return d->context;
}

void QuButton::contextMenuEvent(QContextMenuEvent *e)
{
    CuContextMenu* m = findChild<CuContextMenu *>();
    if(!m)
        m = new CuContextMenu(this, d->context);
    m->popup(e->globalPos());
}
