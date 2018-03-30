#include "ComplexQTangoDemo.h"
#include "ui_ComplexQTangoDemo.h"
#include "demodialog.h"

#include <QTWriter>
#include <QTWatcher>

ComplexQTangoDemo::ComplexQTangoDemo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComplexQTangoDemo)
{
    ui->setupUi(this);
    m_setup();
}

ComplexQTangoDemo::~ComplexQTangoDemo()
{
    delete ui;
}

void ComplexQTangoDemo::execDemoDialog()
{
    DemoDialog dd(this);
    dd.exec();
}

void ComplexQTangoDemo::m_setup()
{
    QTWriter *sliderWriter = new QTWriter(this);
    /* NOTE: sliderMoved not valueChanged! */
    sliderWriter->attach(ui->horizontalSlider, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));
    sliderWriter->setTargets("$1/double_scalar");

    QTWatcher *progressBarWatcher = new QTWatcher(this);
    progressBarWatcher->attach(ui->progressBar, SLOT(setValue(int)));
    /* maximum and minimum */
    progressBarWatcher->setAutoConfSlot(QTWatcher::Min, SLOT(setMinimum(int)));
    progressBarWatcher->setAutoConfSlot(QTWatcher::Max, SLOT(setMaximum(int)));

    progressBarWatcher->setSource("$1/double_scalar");

    connect(ui->readDial, SIGNAL(description(QString)), ui->leDialRead, SLOT(setText(QString)));
    connect(ui->writeDial, SIGNAL(description(QString)), ui->leDialWrite, SLOT(setText(QString)));

    ui->readDial->setSource("$1/double_scalar");
    ui->writeDial->setTargets("$1/double_scalar");

    connect(ui->pbMore, SIGNAL(clicked()), this, SLOT(execDemoDialog()));
}
