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
#include "cuengine_swap.h"

class QuSpinBoxPrivate
{
public:
    CuContext *context;
    bool auto_configure;
    CuControlsUtils u;
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
    const QString& msg = d->u.msg(da);
    if(da[CuDType::Err].toBool())  // da["err"]
    {
        perr("QuSpinBox [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[CuDType::Src].toString().c_str(), qstoc(msg),  // da["src"]
                da[CuDType::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[CuDType::Src].toString());  // da["src"]
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::CategoryWrite, true);
            log->write(QString("QuSpinBox [" + objectName() + "]").toStdString(), msg.toStdString(), CuLog::LevelError, CuLog::CategoryWrite);
        }
    }
    else if(d->auto_configure && da[CuDType::Type].toString() == "property")  // da["type"]
    {
        QString desc = "";
        if(da[CuDType::DataFormatStr] == "scalar" && da["writable"].toInt() > 0)  // da["dfs"]
        {
            /* first apply format, if - correctly - specified */
            CuVariant m, M;
            m = da[CuDType::Min];  // da["min"]
            M = da[CuDType::Max];  // da["max"]
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
            bool can_be_int = da[CuDType::WriteValue].to<int>(val);  // da["w_value"]
            if (can_be_int)
                setValue(val);
            if(!da[CuDType::Description].isNull()) {  // da["description"]
                desc.prepend(QString::fromStdString(da[CuDType::Description].toString()));  // da["description"]
            }
            setWhatsThis(desc);
        }
        else
            perr("QuSpinBox [%s]: invalid data format \"%s\" or read only source (writable: %d)", qstoc(objectName()),
                 da[CuDType::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

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
void QuSpinBox::setTarget(const QString &targets, CuContext *ctx) {
    if(ctx) {
        delete d->context;
        d->context = ctx;
    }
    CuControlsWriterA* w = d->context->replace_writer(targets.toStdString(), this);
    if(w) w->setTarget(targets);
}

void QuSpinBox::clearTarget() {
    d->context->disposeWriter();
}

bool QuSpinBox::ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool) {
    CuEngineSwap csw;
    d->context = csw.replace(this, d->context, cu_p, fpool);
    return csw.ok();
}

void QuSpinBox::m_init()
{
    d->auto_configure = true;
}
