#include "widget.h"
#include "ui_widget.h"

#include <cudata.h>
#include <cuactivity.h>
#include <cuisolatedactivity.h>
#include <QThread>
#include <cumbia.h>
#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridge.h>
#include <cuserviceprovider.h>

#include "myactivity.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
//    CuData data("value", 10);

//    Cumbia *cu = new Cumbia();
//    MyActivity * a = new MyActivity();
//    CuThreadFactoryImpl fi;
//    CuThreadsEventBridgeFactory tfii;

//  // cu->registerActivity(a, this, CuData("thread", "mythread"), fi, tfii);


    ui->comboBox->insertItems(0, QStringList() << "A" << "B" << "C");
    ui->lineEdit->setText("duck");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onProgress(int step, int total, const CuData &data)
{

}

void Widget::onResult(const CuData &data)
{
    ui->lineEdit->setText(data["value"].toString().c_str());
}

void Widget::onResult(const std::vector<CuData> &datalist)
{

}

CuData Widget::getToken() const
{
    return CuData("thread", "mythread");
}

