#include "manual_refresh.h"

// cumbia-tango
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
// cumbia-tango

#include <cucontext.h>
#include <qulabel.h>
#include <cutreader.h>
#include <qucirculargauge.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QMessageBox>
#include <QLabel>

Manual_refresh::Manual_refresh(CumbiaPool *cup, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cup, &m_ctrl_factory_pool, &m_log_impl);
    cu_p = cup;

    QVBoxLayout *vlo = new QVBoxLayout(this);
    QLabel *title = new QLabel("Manual Refresh Mode Example", this);
    QLabel *src = new QLabel(this);
    QFont f = title->font();
    f.setBold(true);
    title->setFont(f);
    QuLabel *l = new QuLabel(this, cup, m_ctrl_factory_pool);
    QuCircularGauge *g = new QuCircularGauge(this, cup, m_ctrl_factory_pool);
    QPushButton *b = new QPushButton("Click to Refresh!", this);

    // vertically fixed size for labels
    foreach(QWidget *w, QList<QWidget *>()<<l << title << src)
        w->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);;

    if(qApp->arguments().size() > 1) {
        l->getContext()->setOptions(CuData("manual", true));
        g->getContext()->setOptions(CuData("manual", true));
        l->setSource(qApp->arguments().at(1));
        g->setSource(l->source());
        src->setText(l->source());
    }
    else {
        QMessageBox::information(this, "Usage", QString("%1 a/device/name/attribute").arg(qApp->arguments().first()));
        b->setDisabled(true);
    }

    foreach(QWidget *w, QList<QWidget *>() << title << src << l << g << b)
        vlo->addWidget(w);

    connect(b, SIGNAL(clicked()), this, SLOT(read()));
    resize(300, 400);
}

Manual_refresh::~Manual_refresh()
{
}

void Manual_refresh::read()
{
    QuLabel *l = findChild<QuLabel *>();
    l->getContext()->sendData(CuData("read", ""));
    QuCircularGauge *g = findChild<QuCircularGauge *>();
    g->getContext()->sendData(CuData("read", ""));
}
