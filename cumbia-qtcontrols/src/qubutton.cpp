#include "qubutton.h"
#include <cumacros.h>
#include <cudata.h>
#include <cumbia.h>
#include <cuserviceprovider.h>

#include "cucontrolswriter_abs.h"
#include "cucontrolsfactories_i.h"
#include "cucontrolsutils.h"
#include "qupalette.h"
#include "qulogimpl.h"

class QuButtonPrivate
{
public:
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
    d = new QuButtonPrivate;
    connect(this, SIGNAL(clicked()), this, SLOT(execute()));
    d->writer = w_fac.create(cumbia, this);
    d->auto_configure = true;
    d->write_ok = false;
    d->log = static_cast<CuLog *>(cumbia->getServiceProvider()->get(CuServices::Log));
    static_cast<QuLogImpl *>(d->log->getImpl("QuLogImpl"))->showPopupOnMessage(CuLog::Write, true);
}

QuButton::~QuButton()
{
    delete d->writer;
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
    d->writer->setTargets(targets);
}

QString QuButton::targets() const
{
    return d->writer->targets();
}

void QuButton::onUpdate(const CuData &data)
{
    if(data["err"].toBool())
        d->log->write(QString("QuButton [" + objectName() + "]").toStdString(), data["msg"].toString(), CuLog::Error, CuLog::Write);
}
