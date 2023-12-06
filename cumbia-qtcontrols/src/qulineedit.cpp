#include "qulineedit.h"
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

class QuLineEditPrivate
{
public:
    CuContext *context;
    bool auto_configure;
};

QuLineEdit::QuLineEdit(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
    : QLineEdit(parent)
{
    d = new QuLineEditPrivate;
    d->context = new CuContext(cumbia, w_fac);
    d->auto_configure = true;
}

QuLineEdit::QuLineEdit(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QLineEdit(parent)
{
    d = new QuLineEditPrivate;
    d->context = new CuContext(cumbia_pool, fpool);
    d->auto_configure = true;
}

QuLineEdit::~QuLineEdit()
{
    delete d->context;
    delete d;
}

CuContext *QuLineEdit::getContext() const
{
    return d->context;
}

void QuLineEdit::onUpdate(const CuData &da)
{
    if(da[CuDType::Err].toBool())  // da["err"]
    {
        perr("QuLineEdit [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[CuDType::Src].toString().c_str(), da[CuDType::Message].toString().c_str(),  // da["src"], da["msg"]
                da[CuDType::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[CuDType::Src].toString());  // da["src"]
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::CategoryWrite, true);
            log->write(QString("QuLineEdit [" + objectName() + "]").toStdString(), da[CuDType::Message].toString(), CuLog::LevelError, CuLog::CategoryWrite);  // da["msg"]
        }
    }
    else if(d->auto_configure && da[CuDType::Type].toString() == "property")  // da["type"]
    {
        QString desc = "";
        if(da["writable"].toInt() > 0)  {
            setText(da[CuDType::WriteValue].toString().c_str());  // da["w_value"]
            if(!da[CuDType::Description].isNull()) {  // da["description"]
                desc.prepend(QString::fromStdString(da[CuDType::Description].toString()));  // da["description"]
            }
            setWhatsThis(desc);
        }
        else
            perr("QuLineEdit [%s]: (data format \"%s\") is read only (writable: %d)", qstoc(objectName()),
                 da[CuDType::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]
    }
}

/*! \brief returns the target name.
 *
 * @return the target name, or an empty string if setTarget hasn't been called yet.
 *
 * Refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QString QuLineEdit::target() const
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
void QuLineEdit::setTarget(const QString &targets)
{
    CuControlsWriterA* w = d->context->replace_writer(targets.toStdString(), this);
    if(w) w->setTarget(targets);
}

bool QuLineEdit::ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool) {
    CuEngineSwap csw;
    d->context = csw.replace(this, d->context, cu_p, fpool);
    return csw.ok();
}

void QuLineEdit::m_init()
{
    d->auto_configure = true;
}
