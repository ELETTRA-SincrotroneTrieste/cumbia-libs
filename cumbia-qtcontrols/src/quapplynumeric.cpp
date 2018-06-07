#include "quapplynumeric.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cudatatypes_ex.h>
#include <cuserviceprovider.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "cumbiapool.h"
#include "cucontext.h"
#include "culinkstats.h"
#include "qulogimpl.h"

/// @private
class QuApplyNumericPrivate
{
public:
    CuContext *context;
    bool auto_configure;
    bool write_ok;
    CuLog *log;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsWriterFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuApplyNumeric::QuApplyNumeric(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
    : EApplyNumeric(parent)
{
    m_init();
    d->context = new CuContext(cumbia, w_fac);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuApplyNumeric::QuApplyNumeric(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : EApplyNumeric(parent)
{
    m_init();
    d->context = new CuContext(cumbia_pool, fpool);
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
    CuVariant args(val);
    CuControlsWriterA *w = d->context->getWriter();
    if(w) {
        w->setArgs(args);
        w->execute();
    }
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
    d->write_ok = !da[CuDType::Err].toBool();
    // update link statistics
    d->context->getLinkStats()->addOperation();
    if(!d->write_ok)
    {
        perr("QuApplyNumeric [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[CuDType::Src].toString().c_str(), da[CuDType::Message].toString().c_str(),
                da[CuXDType::DataFormatStr].toString().c_str(), da[CuXDType::Writable].toInt());

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[CuDType::Src].toString());
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
            log->write(QString("QuApplyNumeric [" + objectName() + "]").toStdString(), da[CuDType::Message].toString(), CuLog::Error, CuLog::Write);
        }
        d->context->getLinkStats()->addError(da[CuDType::Message].toString());
    }
    else if(d->auto_configure && da[CuDType::Type].toString() == "property")
    {
        QString desc = "";
        if(da[CuXDType::DataFormatStr] == "scalar" && da[CuXDType::Writable].toInt() > 0)
        {
            /* first apply format, if - correctly - specified */
            CuVariant m, M;
            m = da[CuXDType::Min];
            M = da[CuXDType::Max];
            std::string print_format = da[CuXDType::DisplayFormat].toString();
            double min, max;
            bool ok;
            ok = m.to<double>(min);
            if(ok)
                ok = M.to<double>(max);
            if(ok)
            {
                configureNumber(min, max, QString::fromStdString(print_format));
                /* integerDigits() and decimalDigits() from NumberDelegate */
                setIntDigits(integerDigits());
                setDecDigits(decimalDigits());
                setMaximum(max);
                setMinimum(min);
                desc = "\n(min: "+ QString("%1").arg(min) + " max: "+ QString("%1").arg(max) + ")";
            }
            else
                pinfo("QuApplyNumeric: maximum and minimum values not set on the tango attribute \"%s\", object \"%s\": "
                      "not setting format nor maximum/minimum", qstoc(target()), qstoc(objectName()));

            /* can set current values instead */
            double val;
            bool can_be_double = da[CuXDType::WriteValue].to<double>(val);
            if (can_be_double)
            {
                setValue(val);
                clearModified();
            }
            if(!da[CuXDType::Description].isNull()) {
                desc.prepend(QString::fromStdString(da[CuXDType::Description].toString()));
            }
            setWhatsThis(desc);
        }
        else
            perr("QuApplyNumeric [%s]: invalid data format \"%s\" or read only source (writable: %d)", qstoc(objectName()),
                 da[CuXDType::DataFormatStr].toString().c_str(), da[CuXDType::Writable].toInt());

    }
}

/*! \brief returns a pointer to the context
 *
 * @return a pointer to the CuContext
 */
CuContext *QuApplyNumeric::getContext() const
{
    return d->context;
}
