#include "qulineedit.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>
#include <cudatatypes_ex.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "cumbiapool.h"
#include "cucontext.h"
#include "qulogimpl.h"

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
    if(da[CuDType::Err].toBool())
    {
        perr("QuLineEdit [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[CuDType::Src].toString().c_str(), da[CuDType::Message].toString().c_str(),
                da[CuXDType::DataFormatStr].toString().c_str(), da[CuXDType::Writable].toInt());

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[CuDType::Src].toString());
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
            log->write(QString("QuLineEdit [" + objectName() + "]").toStdString(), da[CuDType::Message].toString(), CuLog::Error, CuLog::Write);
        }
    }
    else if(d->auto_configure && da[CuDType::Type].toString() == "property")
    {
        QString desc = "";
        if(da[CuXDType::Writable].toInt() > 0)  {
            setText(da[CuXDType::WriteValue].toString().c_str());
            if(!da[CuXDType::Description].isNull()) {
                desc.prepend(QString::fromStdString(da[CuXDType::Description].toString()));
            }
            setWhatsThis(desc);
        }
        else
            perr("QuLineEdit [%s]: (data format \"%s\") is read only (writable: %d)", qstoc(objectName()),
                 da[CuXDType::DataFormatStr].toString().c_str(), da[CuXDType::Writable].toInt());
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

void QuLineEdit::m_init()
{
    d->auto_configure = true;
}
