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
void QuApplyNumeric::setTarget(const QString &targets)
{
    CuControlsWriterA* w = d->context->replace_writer(targets.toStdString(), this);
    if(w) w->setTarget(targets);
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
    printf("\e[1;32mQuApplyNumeric> initializing\e[0m\n");
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
    if(da["err"].toBool())
    {
        perr("QuApplyNumeric [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da["src"].toString().c_str(), da["msg"].toString().c_str(),
                da["data_format_str"].toString().c_str(), da["writable"].toInt());

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da["src"].toString());
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
            log->write(QString("QuApplyNumeric [" + objectName() + "]").toStdString(), da["msg"].toString(), CuLog::Error, CuLog::Write);
        }
    }
    else if(d->auto_configure && da["type"].toString() == "property")
    {
        QString desc = "";
        if(da["data_format_str"] == "scalar" && da["writable"].toInt() > 0)
        {
            /* first apply format, if - correctly - specified */
            CuVariant m, M;
            m = da["min"];
            M = da["max"];
            std::string print_format = da["format"].toString();
            double min, max;
            bool ok;
            ok = m.to<double>(min);
            if(ok)
                ok = M.to<double>(max);
            if(ok)
            {
                configureNumber(min, min, QString::fromStdString(print_format));
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
            bool can_be_double = da["w_value"].to<double>(val);
            if (can_be_double)
            {
                setValue(val);
                clearModified();
            }
            if(!da["description"].isNull()) {
                desc.prepend(QString::fromStdString(da["description"].toString()));
            }
            setWhatsThis(desc);
        }
        else
            perr("QuApplyNumeric [%s]: invalid data format \"%s\" or read only source (writable: %d)", qstoc(objectName()),
                 da["data_format_str"].toString().c_str(), da["writable"].toInt());

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
