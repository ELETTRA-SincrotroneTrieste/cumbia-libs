#include "quiowidget.h"
#include "ui_quinputoutput.h"

#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cuserviceprovider.h>
#include <cumacros.h>

#include "quinputoutput.h"
#include <QVBoxLayout>

Quinputoutput::Quinputoutput(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Quinputoutput)
{
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);

    ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);

    QVBoxLayout *vlo = new QVBoxLayout(this);
    QuInputOutput *qio = new QuInputOutput(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
    vlo->addWidget(qio);

    if(qApp->arguments().count() < 2)
    {
        printf("\nusage: %s tango/device/name/attribute or  tango/device/name->Command\n\n",
               qstoc(qApp->arguments().first()));
        exit(EXIT_FAILURE);
    }
    else
        qio->setSource(qApp->arguments().at(1));
}

Quinputoutput::~Quinputoutput()
{
    delete ui;
}
