#include "qulineedit.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>
#include <quapplication.h>

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
    QuLineEditPrivate() : context(nullptr), auto_configure(true) {}
    CuContext *context;
    bool auto_configure;
};

QuLineEdit::QuLineEdit(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac)
    : QLineEdit(parent)
{
    d = new QuLineEditPrivate;
    d->context = new CuContext(cumbia, w_fac);
}

QuLineEdit::QuLineEdit(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)
    : QLineEdit(parent)
{
    d = new QuLineEditPrivate;
    d->context = new CuContext(cumbia_pool, fpool);
}

/*!
 * \brief Classical, single parent-widget constructor. *QuApplication* properly initialized with
 *        cumbia engine objects is compulsory.
 *
 * \param parent widget
 * \par Important note: cumbia engine references are obtained from the QuApplication instance.
 *      For best performance, static cast of QCoreApplication::instance() to QuApplication is
 *      used.
 * \since cumbia 2.1
 */
QuLineEdit::QuLineEdit(QWidget *parent) : QLineEdit(parent) {
    d = new QuLineEditPrivate;
    QuApplication *a = static_cast<QuApplication *>(QCoreApplication::instance());
    d->context = new CuContext(a->cumbiaPool(), *a->fpool());
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
    if(da[TTT::Err].toBool())
    {
        perr("QuLineEdit [%s]: error %s target: \"%s\" format %s (writable: %d)", qstoc(objectName()),
             da[TTT::Src].toString().c_str(), da[TTT::Message].toString().c_str(),  // da["src"], da["msg"]
                da[TTT::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]

        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(da[TTT::Src].toString());  // da["src"]
        CuLog *log;
        if(cumbia && (log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log))))
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::CategoryWrite, true);
            log->write(QString("QuLineEdit [" + objectName() + "]").toStdString(), da[TTT::Message].toString(), CuLog::LevelError, CuLog::CategoryWrite);  // da["msg"]
        }
    }
    else if(d->auto_configure && da[TTT::Type].toString() == "property")  // da["type"]
    {
        QString desc = "";
        if(da[TTT::Writable].toInt() > 0)  {
            setText(da[TTT::WriteValue].toString().c_str());  // da["w_value"]
            if(!da[TTT::Description].isNull()) {  // da["description"]
                desc.prepend(QString::fromStdString(da[TTT::Description].toString()));  // da["description"]
            }
            setWhatsThis(desc);
        }
        else
            perr("QuLineEdit [%s]: (data format \"%s\") is read only (writable: %d)", qstoc(objectName()),
                 da[TTT::DataFormatStr].toString().c_str(), da["writable"].toInt());  // da["dfs"]
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

