#include "qubutton.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "cumbiapool.h"
#include "culinkcontrol.h"
#include "qupalette.h"
#include "qulogimpl.h"

class QuButtonPrivate
{
public:
    CuLinkControl *link_ctrl;
    CuControlsWriterA *writer;
    bool auto_configure;
    bool write_ok;
    QuPalette palette;
    CuLog *log;
};

QuButton::QuButton(QWidget *parent,
                   Cumbia *cumbia,
                   const CuControlsWriterFactoryI &w_fac) : QPushButton(parent)
{
    m_init();
    d->link_ctrl = new CuLinkControl(cumbia, w_fac);
}

QuButton::QuButton(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool) : QPushButton(w)
{
    m_init();
    d->link_ctrl = new CuLinkControl(cumbia_pool, fpool);
}

QuButton::~QuButton()
{
    delete d->link_ctrl;
    delete d;
}

void QuButton::execute()
{
    cuprintf("QuButton.execute\n");
    CuControlsUtils cu;
    CuVariant args = cu.getArgs(targets(), this);
    printf("QuButton.execute: got args %s type %d format %d\n", args.toString().c_str(), args.getType(),
           args.getFormat());
    d->writer->setArgs(args);
    d->writer->execute();
}

void QuButton::setTargets(const QString &targets)
{
    printf("\e[1;32mQuButton.setTargets!!!!! %s\e[0m\n", qstoc(targets));
    if(d->writer && d->writer->targets() != targets)
        delete d->writer;
    d->writer = d->link_ctrl->make_writer(targets.toStdString(), this);
    if(d->writer)
        d->writer->setTargets(targets);
}

void QuButton::m_init()
{
    printf("\e[1;32mQuButton> initializing\e[0m\n");
    d = new QuButtonPrivate;
    connect(this, SIGNAL(clicked()), this, SLOT(execute()));
    d->writer = NULL;
    d->auto_configure = true;
    d->write_ok = false;
}

QString QuButton::targets() const
{
    if(d->writer)
        return d->writer->targets();
    return "";
}

void QuButton::onUpdate(const CuData &data)
{
    if(data["err"].toBool())
    {
        Cumbia* cumbia = d->link_ctrl->cu;
        if(!cumbia) /* pick from the CumbiaPool */
            cumbia = d->link_ctrl->cu_pool->getBySrc(data["src"].toString());
        CuLog *log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log));
        if(log)
        {
            static_cast<QuLogImpl *>(log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
            log->write(QString("QuButton [" + objectName() + "]").toStdString(), data["msg"].toString(), CuLog::Error, CuLog::Write);
        }
    }
}
