#include "quapplynumeric.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "cumbiapool.h"
#include "cucontext.h"
#include "culinkstats.h"
#include "qulogimpl.h"
#include "quanimation.h"
#include "cuengine_swap.h"
#include "quapplication.h"

#include <QPainter>
#include <QPaintEvent>
#include <QtDebug>
#include <cumbiapool.h>

/// @private
class QuApplyNumericPrivate
{
public:
    CuContext *context;
    bool auto_configure;
    bool write_ok;
    CuLog *log;
    QuAnimation animation;
    CuControlsUtils u;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsWriterFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuApplyNumeric::QuApplyNumeric(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
    : EApplyNumeric(parent) {
    m_init();
    d->context = new CuContext(cumbia, w_fac);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuApplyNumeric::QuApplyNumeric(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : EApplyNumeric(parent) {
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
}

QuApplyNumeric::QuApplyNumeric(QWidget *w)
   : EApplyNumeric(w) {
    m_init();
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    d->context = new CuContext(a->cumbiaPool(), *a->fpool());
}

/*! \brief class destructor
 *
 * Deletes the context used to link the widget to the writer.
 */
QuApplyNumeric::~QuApplyNumeric()
{
    delete d->context;
    delete d;
}

/*! \brief returns the target name.
 *
 * @return the target name, or an empty string if setTarget hasn't been called yet.
 *
 * Refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QString QuApplyNumeric::target() const
{
    CuControlsWriterA *w = d->context->getWriter();
    if(w != NULL)
        return w->target();
    return "";
}

/** \brief Set the name of the target that will be executed.
 *
 * Refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
void QuApplyNumeric::setTarget(const QString &target)
{
    CuControlsWriterA* w = d->context->replace_writer(target.toStdString(), this);
    if(w) w->setTarget(target);
}

/*! \brief executes the target specified with setTarget, writing the value set on the widget.
 *
 * The value set on the widget is written on the target when the *apply* button
 * is clicked.
 * This virtual method can be reimplemented on a subclass.
 */
void QuApplyNumeric::execute(double val)
{
    d->animation.setPenColor(QColor(Qt::gray));
    d->animation.loop(QuAnimation::ElasticBottomLine, 1000);
    CuVariant args(val);
    CuControlsWriterA *w = d->context->getWriter();
    if(w) {
        w->setArgs(args);
        w->execute();
    }
}

bool QuApplyNumeric::ctxSwap(CumbiaPool *cp, const CuControlsFactoryPool &fpool) {
    CuEngineSwap ehs;
    d->context = ehs.replace(this, d->context, cp, fpool);
    return ehs.ok();
}

/// @private
void QuApplyNumeric::m_init()
{
    d = new QuApplyNumericPrivate;
    connect(this, SIGNAL(clicked(double)), this, SLOT(execute(double)));
    d->auto_configure = true;
    d->write_ok = false;
}

/*! \brief configures the widget as soon as it is connected and records write errors.
 *
 * @param da CuData containing configuration options upon link creation or
 *        write operation results.
 */
void QuApplyNumeric::onUpdate(const CuData &da)
{
    bool is_config = da[TTT::Type].toString() == std::string("property");  // da["type"]
    if(!da["is_result"].toBool() && !is_config)
        return;

    d->write_ok = !da[TTT::Err].toBool();  // da["err"]
    // update link statistics
    d->context->getLinkStats()->addOperation();
    const QString& msg = d->u.msg(da);
    if(!d->write_ok)
    {
        perr("QuApplyNumeric [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[TTT::Src].toString().c_str(), msg.toStdString().c_str(),  // da["src"]
                da[TTT::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[TTT::Src].toString());  // da["src"]
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
            log->write(QString("QuApplyNumeric [" + objectName() + "]").toStdString(), msg.toStdString(), CuLog::LevelError, CuLog::CategoryWrite);
        else if(!cumbia) {
            perr("QuApplyNumeric.onUpdate: cannot get a reference to cumbia either from context or CumbiaPool with target \"%s\"", da[TTT::Src].toString().c_str());  // da["src"]
        }

        d->context->getLinkStats()->addError(msg.toStdString());
    }
    else if(d->auto_configure && is_config)
    {
        QString desc = "";
        if(da[TTT::DataFormatStr] == "scalar" && da["writable"].toInt() > 0)  // da["dfs"]
        {
            /* first apply format, if - correctly - specified */
            CuVariant m, M;
            m = da[TTT::Min];  // da["min"]
            M = da[TTT::Max];  // da["max"]
            std::string print_format = da[TTT::NumberFormat].toString();  // da["format"]
            double min, max;
            bool ok;
            ok = m.to<double>(min);
            if(ok)
                ok = M.to<double>(max);

            if(!ok)
                min = max = 0.0f; // configureNumber will use format only
            configureNumber(min, max, QString::fromStdString(print_format));
            /* integerDigits() and decimalDigits() from NumberDelegate */
            setIntDigits(integerDigits());
            setDecDigits(decimalDigits());
            if(ok) {
                setMaximum(max);
                setMinimum(min);
            }
            else
                updateMaxMinAuto();

            desc = "\n(min: "+ QString("%1").arg(min) + " max: "+ QString("%1").arg(max) + ")";

            if(!ok)
                pinfo("QuApplyNumeric: maximum and minimum values not available for \"%s\", object \"%s\": "
                      "setting number of int and dec digits from format \"%s\"", qstoc(target()), qstoc(objectName()), print_format.c_str());

            /* can set current values instead */
            double val;
            bool can_be_double = da[TTT::WriteValue].to<double>(val);  // da["w_value"]
            if (can_be_double)
            {
                setValue(val);
                clearModified();
            }
            if(!da[TTT::Description].isNull()) {  // da["description"]
                desc.prepend(QString::fromStdString(da[TTT::Description].toString()));  // da["description"]
            }
            setWhatsThis(desc);
            // save fetching configuration at every execute
            CuControlsWriterA *w = d->context->getWriter();
            if(w)
                w->saveConfiguration(da);
            d->animation.installOn(this);
        }
        else
            perr("QuApplyNumeric [%s]: invalid data format \"%s\" or read only source (writable: %d)", qstoc(objectName()),
                 da[TTT::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

    }
    setToolTip(msg);
    // animation
    d->write_ok ? d->animation.setPenColor(QColor(Qt::green)) : d->animation.setPenColor(QColor(Qt::red));
    d->write_ok ? d->animation.setDuration(1500) : d->animation.setDuration(5000);
    d->animation.setType(QuAnimation::ShrinkingBottomLine);
    d->animation.start();
}

/*! \brief returns a pointer to the context
 *
 * @return a pointer to the CuContext
 */
CuContext *QuApplyNumeric::getContext() const
{
    return d->context;
}

void QuApplyNumeric::onAnimationValueChanged(const QVariant &)
{
    // optimize animation paint area with the clipped rect provided by QuAnimation::clippedRect
    QRect br = getButton()->geometry();
    QRect updateRect(br.left() - 2, br.bottom() + 2, br.width() + 2, 4);
    update(updateRect);
}

void QuApplyNumeric::paintEvent(QPaintEvent *pe)
{
    EApplyNumeric::paintEvent(pe);
    if(d->animation.currentValue() != 0.0) {
        QRect bre = getButton()->geometry();
        QRect r = QRect(bre.left(), bre.bottom() + 2, bre.width(), 4);
        d->animation.draw(this, r);
    }
}
