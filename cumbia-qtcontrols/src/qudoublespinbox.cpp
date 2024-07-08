#include "qudoublespinbox.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cumbiapool.h"
#include "cucontext.h"
#include "qulogimpl.h"
#include "cuengine_swap.h"

class QuDoubleSpinBoxPrivate
{
public:
    CuContext *context;
    bool auto_configure;
};

QuDoubleSpinBox::QuDoubleSpinBox(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
    : QDoubleSpinBox(parent)
{
    d = new QuDoubleSpinBoxPrivate;
    d->context = new CuContext(cumbia, w_fac);
    m_init();
}

QuDoubleSpinBox::QuDoubleSpinBox(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QDoubleSpinBox(parent)
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
    if(da[TTT::Err].toBool())  // da["err"]
    {
        perr("QuDoubleSpinBox [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[TTT::Src].toString().c_str(), da[TTT::Message].toString().c_str(),  // da["src"], da["msg"]
                da[TTT::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[TTT::Src].toString());  // da["src"]
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::CategoryWrite, true);
            log->write(QString("QuDoubleSpinBox [" + objectName() + "]").toStdString(), da[TTT::Message].toString(), CuLog::LevelError, CuLog::CategoryWrite);  // da["msg"]
        }
    }
    else if(d->auto_configure && da[TTT::Type].toString() == "property")  // da["type"]
    {
        QString desc = "";
        if(da[TTT::DataFormatStr] == "scalar" && da["writable"].toInt() > 0)  // da["dfs"]
        {
            /* first apply format, if - correctly - specified */
            CuVariant m, M;
            m = da[TTT::Min];  // da["min"]
            M = da[TTT::Max];  // da["max"]
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
            bool can_be_double = da[TTT::WriteValue].to<double>(val);  // da["w_value"]
            if (can_be_double)
                setValue(val);
            if(!da[TTT::Description].isNull()) {  // da["description"]
                desc.prepend(QString::fromStdString(da[TTT::Description].toString()));  // da["description"]
            }
            setWhatsThis(desc);
        }
        else
            perr("QuDoubleSpinBox [%s]: invalid data format \"%s\" or read only source (writable: %d)", qstoc(objectName()),
                 da[TTT::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

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
void QuDoubleSpinBox::setTarget(const QString &targets, CuContext *ctx) {
    if(ctx) {
        delete d->context;
        d->context = ctx;
    }
    CuControlsWriterA* w = d->context->replace_writer(targets.toStdString(), this);
    if(w) w->setTarget(targets);
}

void QuDoubleSpinBox::clearTarget() {
    d->context->disposeWriter();
}

bool QuDoubleSpinBox::ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool) {
    CuEngineSwap csw;
    d->context = csw.replace(this, d->context, cu_p, fpool);
    return csw.ok();
}

void QuDoubleSpinBox::m_init()
{
    d->auto_configure = true;

}
