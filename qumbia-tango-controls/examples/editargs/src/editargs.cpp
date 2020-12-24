#include "editargs.h"
#include "ui_editargs.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
#include <cucontext.h>
// cumbia

Editargs::Editargs(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;
    ui = new Ui::Editargs;
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    // mloader.modules() to get the list of loaded modules
    // cumbia
    foreach(QDoubleSpinBox *sb, findChildren<QDoubleSpinBox*>()) {
        connect(sb, SIGNAL(valueChanged(double)), this, SLOT(onSbValueChanged(double)));
    }
    connect(ui->pbRead, SIGNAL(clicked()), this, SLOT(onReadClicked()));
    ui->label->setText(ui->plot->source());
}

Editargs::~Editargs()
{
    delete ui;
}

void Editargs::onSbValueChanged(double v) {
    std::vector<std::string> args;
    QStringList sbs = QStringList() << "dsb0" << "dsb1" << "dsb2" << "dsb3" <<"dsb4" << "dsb5" << "dsb6";
    foreach(const QString& sb, sbs)
        args.push_back(QString::number(findChild<QDoubleSpinBox *>(sb)->value()).toStdString());
    CuData a("args", args);
    printf("Editargs.onSbValueChanged: sending data %s on context %p\n", datos(a), ui->plot->getContext());
    ui->plot->getContext()->sendData(CuData("args", args));
    if(ui->cbRefreshOnChange->isChecked()) ui->plot->getContext()->sendData(CuData("read", ""));
}

void Editargs::onReadClicked() {
    ui->plot->getContext()->sendData(CuData("read", ""));
}
