#include "manual_refresh.h"
#include "ui_manual_refresh.h"

// cumbia-tango
#include <cuserviceprovider.h>
#include <cumacros.h>
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

Manual_refresh::Manual_refresh(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent)
{
    // cumbia-tango
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);
    // cumbia-tango

    QVBoxLayout *vlo = new QVBoxLayout(this);
    QLabel *title = new QLabel("Manual Refresh Mode Example", this);
    QLabel *src = new QLabel(this);
    QFont f = title->font();
    f.setBold(true);
    title->setFont(f);
    QuLabel *l = new QuLabel(this, cu_t, cu_tango_r_fac);
    QuCircularGauge *g = new QuCircularGauge(this, cu_t, cu_tango_r_fac);
    QPushButton *b = new QPushButton("Click to Refresh!", this);

    // vertically fixed size for labels
    title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    src->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    l->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    if(qApp->arguments().size() > 1) {
        l->getContext()->setOptions(CuData("refresh_mode", CuTReader::Manual));
        l->setSource(qApp->arguments().at(1));
        g->setSource(l->source());
        src->setText(l->source());
    }
    else {
        QMessageBox::information(this, "Usage", QString("%1 a/device/name/attribute").arg(qApp->arguments().first()));
        b->setDisabled(true);
    }

    vlo->addWidget(title);
    vlo->addWidget(src);
    vlo->addWidget(l);
    vlo->addWidget(g);
    vlo->addWidget(b);

    connect(b, SIGNAL(clicked()), this, SLOT(read()));
    resize(300, 400);
}

Manual_refresh::~Manual_refresh()
{
    delete ui;
}

void Manual_refresh::read()
{
    QuLabel *l = findChild<QuLabel *>();
    l->getContext()->sendData(CuData("read", ""));
}
