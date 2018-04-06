#include "qudoublespinbox.h"
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

class QuDoubleSpinBoxPrivate
{
public:
    CuContext *context;
    bool auto_configure;
};

QuDoubleSpinBox::QuDoubleSpinBox(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
    : QSpinBox(parent)
{
    d = new QuDoubleSpinBoxPrivate;
    d->context = new CuContext(cumbia, w_fac);
    m_init();
}

QuDoubleSpinBox::QuDoubleSpinBox(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QSpinBox(parent)
{
    d = new QuDoubleSpinBoxPrivate;
    d->context = new CuContext(cumbia_pool, fpool);
    m_init();
}

QuDoubleSpinBox::~QuDoubleSpinBox()
{
    delete d->context;
    delete d;
}

CuContext *QuDoubleSpinBox::getContext() const
{
    return d->context;
}

void QuDoubleSpinBox::onUpdate(const CuData &da)
{
    if(da["err"].toBool())
    {
        perr("QuDoubleSpinBox [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da["src"].toString().c_str(), da["msg"].toString().c_str(),
                da["data_format_str"].toString().c_str(), da["writable"].toInt());

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da["src"].toString());
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
            log->write(QString("QuDoubleSpinBox [" + objectName() + "]").toStdString(), da["msg"].toString(), CuLog::Error, CuLog::Write);
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
            double min, max;
            bool ok;
            ok = m.to<double>(min);
            if(ok)
                ok = M.to<double>(max);
            if(ok) {
                setMaximum(max);
                setMinimum(min);desc = "\n(min: "+ QString("%1").arg(min) + " max: "+ QString("%1").arg(max) + ")";
            }
            else
                pinfo("QuDoubleSpinBox: maximum and minimum values not set on the tango attribute \"%s\", object \"%s\": "
                      "not setting format nor maximum/minimum", qstoc(target()), qstoc(objectName()));

            /* can set current values instead */
            double val;
            bool can_be_double = da["w_value"].to<double>(val);
            if (can_be_double)
                setValue(val);
            if(!da["description"].isNull()) {
                desc.prepend(QString::fromStdString(da["description"].toString()));
            }
            setWhatsThis(desc);
        }
        else
            perr("QuDoubleSpinBox [%s]: invalid data format \"%s\" or read only source (writable: %d)", qstoc(objectName()),
                 da["data_format_str"].toString().c_str(), da["writable"].toInt());

    }
}

/*! \brief returns the target name.
 *
 * @return the target name, or an empty string if setTarget hasn't been called yet.
 *
 * Refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QString QuDoubleSpinBox::target() const
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
void QuDoubleSpinBox::setTarget(const QString &targets)
{
    CuControlsWriterA* w = d->context->replace_writer(targets.toStdString(), this);
    if(w) w->setTarget(targets);
}

void QuDoubleSpinBox::m_init()
{
    d->auto_configure = true;

}
