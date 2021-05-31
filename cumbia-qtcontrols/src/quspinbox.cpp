#include "quspinbox.h"
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

class QuSpinBoxPrivate
{
public:
    CuContext *context;
    bool auto_configure;
};

QuSpinBox::QuSpinBox(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
    : QSpinBox(parent)
{
    d = new QuSpinBoxPrivate;
    d->context = new CuContext(cumbia, w_fac);
    d->auto_configure = true;
}

QuSpinBox::QuSpinBox(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QSpinBox(parent)
{
    d = new QuSpinBoxPrivate;
    d->context = new CuContext(cumbia_pool, fpool);
    d->auto_configure = true;
}

QuSpinBox::~QuSpinBox()
{
    delete d->context;
    delete d;
}

CuContext *QuSpinBox::getContext() const
{
    return d->context;
}

void QuSpinBox::onUpdate(const CuData &da)
{
    if(da["err"].toBool())
    {
        perr("QuSpinBox [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da["src"].toString().c_str(), da["msg"].toString().c_str(),
                da["data_format_str"].toString().c_str(), da["writable"].toInt());

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da["src"].toString());
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::CategoryWrite, true);
            log->write(QString("QuSpinBox [" + objectName() + "]").toStdString(), da["msg"].toString(), CuLog::LevelError, CuLog::CategoryWrite);
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
            int min, max;
            bool ok;
            ok = m.to<int>(min);
            if(ok)
                ok = M.to<int>(max);
            if(ok) {
                setMaximum(max);
                setMinimum(min);desc = "\n(min: "+ QString("%1").arg(min) + " max: "+ QString("%1").arg(max) + ")";
            }
            else
                pinfo("QuSpinBox: maximum and minimum values not set on the tango attribute \"%s\", object \"%s\": "
                      "not setting format nor maximum/minimum", qstoc(target()), qstoc(objectName()));

            /* can set current values instead */
            int val;
            bool can_be_int = da["w_value"].to<int>(val);
            if (can_be_int)
                setValue(val);
            if(!da["description"].isNull()) {
                desc.prepend(QString::fromStdString(da["description"].toString()));
            }
            setWhatsThis(desc);
        }
        else
            perr("QuSpinBox [%s]: invalid data format \"%s\" or read only source (writable: %d)", qstoc(objectName()),
                 da["data_format_str"].toString().c_str(), da["writable"].toInt());

    }
}

/*! \brief returns the target name.
 *
 * @return the target name, or an empty string if setTarget hasn't been called yet.
 *
 * Refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QString QuSpinBox::target() const
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
void QuSpinBox::setTarget(const QString &targets)
{
    CuControlsWriterA* w = d->context->replace_writer(targets.toStdString(), this);
    if(w) w->setTarget(targets);
}

void QuSpinBox::m_init()
{
    d->auto_configure = true;
}
