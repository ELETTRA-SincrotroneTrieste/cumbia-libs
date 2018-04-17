#include "demodialog.h"
#include "ui_demodialog.h"

DemoDialog::DemoDialog(CumbiaTango *cut, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DemoDialog)
{
    ui->setupUi(this, cut, cu_tango_r_fac, cu_tango_w_fac);
}

DemoDialog::~DemoDialog()
{
    delete ui;
}

