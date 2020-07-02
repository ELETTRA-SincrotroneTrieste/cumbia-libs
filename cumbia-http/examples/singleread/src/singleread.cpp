#include "singleread.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
// cumbia

#include <qulabel.h>
#include <QVBoxLayout>

Singleread::Singleread(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;

    // mloader.modules() to get the list of loaded modules
    // cumbia

    QVBoxLayout *lo = new QVBoxLayout(this);
    QuLabel * la = new QuLabel(this, cu_pool, m_ctrl_factory_pool);
    qApp->arguments().size() > 1 ? la->setSource(qApp->arguments().last()) : la->setText("Usage: " + qApp->arguments().first() +
                                                                                        " [-u http://host:port/chan] source ");
    lo->addWidget(la);
    if(!la->source().isEmpty()) {
        QLabel *l = new  QLabel(this);
        l->setText(la->source());
        lo->insertWidget(0, l);
    }
}

Singleread::~Singleread()
{
}
