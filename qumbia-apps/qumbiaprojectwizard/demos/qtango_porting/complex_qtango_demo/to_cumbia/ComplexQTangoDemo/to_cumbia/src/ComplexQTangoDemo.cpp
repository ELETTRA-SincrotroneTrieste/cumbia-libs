#include "ComplexQTangoDemo.h"
// cumbia-tango
#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cuserviceprovider.h>
#include <cucontextactionbridge.h>
#include <cumacros.h>
// cumbia-tango

#include "ui_ComplexQTangoDemo.h"
#include "demodialog.h"

// #include <QTWriter>
#include <quwriter.h>//	QTWriter is mapped into QuWriter	//	 ## added by qumbiaprojectwizard
// #include <QTWatcher>
#include <quwatcher.h>//	QTWatcher is mapped into QuWatcher	//	 ## added by qumbiaprojectwizard

ComplexQTangoDemo::ComplexQTangoDemo(CumbiaTango *cut, QWidget *parent) :
    QWidget(parent)	/* ,    ui(new Ui::ComplexQTangoDemo)	  // ## qumbiaprojectwizard: instantiated in constructor body */
{

// cumbia-tango
    cu_t = cut;
    m_log = new CuLog(&m_log_impl);
    cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);

    ui = new Ui::ComplexQTangoDemo;
    ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
    // cumbia-tango


    //ui->setupUi(this);

//     QTWriter *sliderWriter = new QTWriter(this);
    QuWriter *sliderWriter = new QuWriter(this, cu_t, cu_tango_w_fac);//	QTWriter is mapped into QuWriter	//	 ## added by qumbiaprojectwizard
    sliderWriter->attach(ui->horizontalSlider, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));
//     sliderWriter->setTargets("$1/double_scalar");
    sliderWriter->setTarget("$1/double_scalar");//	setTargets method is mapped into setTarget (->)	//	 ## added by qumbiaprojectwizard

//     QTWatcher *progressBarWatcher = new QTWatcher(this);
    QuWatcher *progressBarWatcher = new QuWatcher(this, cu_t, cu_tango_r_fac);//	QTWatcher is mapped into QuWatcher	//	 ## added by qumbiaprojectwizard
    progressBarWatcher->attach(ui->progressBar, SLOT(setValue(int)));
//     progressBarWatcher->setAutoConfSlot(QTWatcher::Min, SLOT(setMinimum(int)));
    progressBarWatcher->setAutoConfSlot(QuWatcher::Min, SLOT(setMinimum(int)));//	QTWatcher is mapped into QuWatcher	//	 ## added by qumbiaprojectwizard
//     progressBarWatcher->setAutoConfSlot(QTWatcher::Max, SLOT(setMaximum(int)));
    progressBarWatcher->setAutoConfSlot(QuWatcher::Max, SLOT(setMaximum(int)));//	QTWatcher is mapped into QuWatcher	//	 ## added by qumbiaprojectwizard

    progressBarWatcher->setSource("$1/double_scalar");

    connect(ui->readDial, SIGNAL(description(QString)), ui->leDialRead, SLOT(setText(QString)));
    connect(ui->writeDial, SIGNAL(description(QString)), ui->leDialWrite, SLOT(setText(QString)));

    ui->readDial->setSource("$1/double_scalar");
//     ui->writeDial->setTargets("$1/double_scalar");
    ui->writeDial->setTarget("$1/double_scalar");//	setTargets method is mapped into setTarget (->)	//	 ## added by qumbiaprojectwizard

    connect(ui->pbMore, SIGNAL(clicked()), this, SLOT(execDemoDialog()));
    new CuContextActionBridge(this, cu_t, cu_tango_r_fac);

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

