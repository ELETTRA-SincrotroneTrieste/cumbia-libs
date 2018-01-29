#include "qubutton.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "cumbiapool.h"
#include "cucontext.h"
#include "qupalette.h"
#include "qulogimpl.h"
#include <QtDebug>

class QuButtonPrivate
{
public:
    CuContext *context;
    bool auto_configure;
    bool write_ok;
    QuPalette palette;
    CuLog *log;
};

/** \brief Constructor with the parent widget, an *engine specific* Cumbia implementation and a CuControlsWriterFactoryI interface.
 *
 *  Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuButton::QuButton(QWidget *parent,
                   Cumbia *cumbia,
                   const CuControlsWriterFactoryI &w_fac, const QString &text) : QPushButton(parent)
{
    m_init(text);
    d->context = new CuContext(cumbia, w_fac);
}

/** \brief Constructor with the parent widget, *CumbiaPool*  and *CuControlsFactoryPool*
 *
 *   Please refer to \ref md_src_cumbia_qtcontrols_widget_constructors documentation.
 */
QuButton::QuButton(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, const QString &text)
    : QPushButton(w)
{
    m_init(text);
    d->context = new CuContext(cumbia_pool, fpool);
}

QuButton::~QuButton()
{
    delete d->context;
    delete d;
}

void QuButton::m_init(const QString& text)
{
    d = new QuButtonPrivate;
    connect(this, SIGNAL(clicked()), this, SLOT(execute()));
    d->auto_configure = true;
    d->write_ok = false;
    setText(text);
}

void QuButton::execute()
{
    cuprintf("QuButton.execute\n");
    CuControlsUtils cu;
    CuVariant args = cu.getArgs(targets(), this);
    printf("QuButton.execute: got args %s type %d format %d\n", args.toString().c_str(), args.getType(),
           args.getFormat());
    CuControlsWriterA *w = d->context->getWriter();
    qDebug() << __FUNCTION__ << "writer " << w->targets();
    if(w) {
        w->setArgs(args);
        w->execute();
    }
}

void QuButton::setTargets(const QString &targets)
{
    printf("\e[1;32mQuButton.setTargets!!!!! %s\e[0m\n", qstoc(targets));
    CuControlsWriterA * w = d->context->replace_writer(targets.toStdString(), this);
    if(w) w->setTargets(targets);
}

QString QuButton::targets() const
{
    if(d->context->getWriter())
        return d->context->getWriter()->targets();
    return "";
}

void QuButton::onUpdate(const CuData &data)
{
    if(data["err"].toBool())
    {
        Cumbia* cumbia = d->context->cumbia();
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->context->cumbiaPool()->getBySrc(data["src"].toString());
        CuLog *log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log));
        if(log)
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
            log->write(QString("QuButton [" + objectName() + "]").toStdString(), data["msg"].toString(), CuLog::Error, CuLog::Write);
        }
    }
}

CuContext *QuButton::getContext() const
{
    return d->context;
}
