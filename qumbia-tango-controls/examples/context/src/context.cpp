#include "context.h"
#include "ui_options.h"

#include <cumbiapool.h>
#include <cumbiatango.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quinputoutput.h>
#include <cucontext.h>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <quapplication.h>
#include <QMessageBox>
#include <QPushButton>
#include <cudata.h>
#include <quapps.h>
#include <cutangoopt_builder.h>

#include <QtDebug>

Context::Context(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;
    ui = new Ui::Options;
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);


    qDebug() << __FUNCTION__ << qApp << qApp->arguments();
    if(qApp->arguments().count() < 2)
    {
        QMessageBox::information(this,
                                 "Usage",
                                 QString("%1 tango/device/name/attribute [or command]").arg(qApp->arguments().first()));
        exit(EXIT_SUCCESS);
    }

    QGridLayout *lo = new QGridLayout(this);
    // row 1: source label, start, stop pause buttons
    QLineEdit *leSrc = new QLineEdit(qApp->arguments().at(1));
    leSrc->setObjectName("leSrc");
    lo->addWidget(leSrc, 0, 0, 1, 5);
    leSrc->setText(qApp->arguments().at(1));
    QPushButton *pbStart = new QPushButton("Start", this);
    lo->addWidget(pbStart, 0, 5, 1, 1);
    QPushButton *pbStop = new QPushButton("Stop", this);
    lo->addWidget(pbStop, 0, 6, 1, 1);
    foreach(QPushButton *pb, findChildren<QPushButton *>())
        connect(pb, SIGNAL(clicked()), this, SLOT(runModified()));
    QFont f = leSrc->font();
    f.setBold(true);
    leSrc->setFont(f);
    // row 2 QuInputOutput, information Line edit and reload values button
    QuInputOutput *qio = new QuInputOutput(this, cu_pool, m_ctrl_factory_pool);
    lo->addWidget(qio, 1, 0, 1, 5);
    connect(qio->outputWidget(), SIGNAL(newData(const CuData& )), this, SLOT(onNewData(const CuData&)));
    QLineEdit *refresh_info = new QLineEdit(this);
    refresh_info->setReadOnly(true);
    lo->addWidget(refresh_info, 1, 5, 1, 1);
    refresh_info->setObjectName("leRefreshInfo");
    QPushButton *pbReload = new QPushButton("Reload values property", this);
    pbReload->setToolTip("Calls setOptions to store the options locally.\n"
                         "The options are then used by the setSource\n"
                         "that is invoked right after.");
    connect(pbReload, SIGNAL(clicked()), this, SLOT(reloadProps()));
    lo->addWidget(pbReload, 1, 6, 1, 1);
    QLabel *lref = new QLabel("Refresh Mode", this);
    lref->setToolTip("Change the refresh mode to the selected one.\n"
                     "sendData is called on the context, in conjunction with\n"
                     "setOptions to save the settings locally on the object.");
    lo->addWidget(lref, 2, 2, 1, 2);
    lref->setAlignment(Qt::AlignRight);
    QComboBox *cbMode = new QComboBox(this);
    cbMode->setObjectName("cbMode");
    lo->addWidget(cbMode, 2, 4, 1, 2);
    cbMode->insertItems(0, QStringList() << "event [change]" << "P" << "manual" << "archive event" << "periodic event");
    QPushButton *pbApplyMode = new QPushButton("Set Mode", this);
    pbApplyMode->setToolTip("Apply the mode chosen on the left box.");
    connect(pbApplyMode, SIGNAL(clicked()), this, SLOT(setMode()));
    lo->addWidget(pbApplyMode, 2, 6, 1, 1);
    QLabel *lPeriod = new QLabel("Period", this);
    QSpinBox *sbper = new QSpinBox(this);
    lo->addWidget(lPeriod, 3, 3, 1, 1);
    lo->addWidget(sbper, 3, 4, 1, 2);
    sbper->setToolTip("Set the period in ms. This applies if the mode is \"polled\"");
    QPushButton *pbApplyPeriod = new QPushButton("Set Period", this);
    connect(pbApplyPeriod, SIGNAL(clicked()), this, SLOT(setPeriod()));
    lo->addWidget(pbApplyPeriod, 3, 6, 1, 1);
    pbApplyPeriod->setToolTip("Apply the polling period, if the refresh mode is \"polled\"");

    QLabel *lsend = new QLabel("Get data", this);
    lo->addWidget(lsend, 4 ,0 , 1, 2);
    QComboBox *cbGetData = new QComboBox(this);
    cbGetData->setObjectName("cbGetData");
    cbGetData->setEditable(true);
    cbGetData->insertItems(0, QStringList() << "mode" << "period" << "refresh_mode" << "read");
    cbGetData->setToolTip("getData will be invoked on the context and the key here\n"
                       "will be sent to the link. The result will be displayed on\n"
                       "the text area on the rigth.");
    lo->addWidget(cbGetData, 4, 2, 1, 2 );
    QLineEdit *lereceive = new QLineEdit(this);
    lereceive->setReadOnly(true);
    lereceive->setObjectName("leReceive");
    lereceive->setText("-");
    lereceive->setToolTip("Received data will be displayed here");
    lo->addWidget(lereceive, 4, 4, 1, 2);
    QPushButton *pbGetData = new QPushButton("Get Data", this);
    connect(pbGetData, SIGNAL(clicked()), this, SLOT(getData()));
    lo->addWidget(pbGetData, 4, 6, 1, 1);

    QComboBox *cbSendData = new QComboBox(this);
    cbSendData->setObjectName("cbSend");
    cbSendData->setEditable(true);
    cbSendData->insertItems(0, QStringList() << "period:1200");
    cbSendData->setToolTip("sendData will be invoked on the context");
    lo->addWidget(cbSendData, 5, 2, 1, 2 );
    QLineEdit *le_in_send = new QLineEdit(this);
    le_in_send->setReadOnly(false);
    le_in_send->setObjectName("le_in_send");
    le_in_send->setText(cbSendData->currentText());
    le_in_send->setToolTip("Input arguments to build data to send through the link\n"
                           "Syntax:\n" "key1:value1 key2:value2 key3:value3\n"
                           "For example, to change the period in polling mode, type\n"
                           "\"period:1200\"");
    lo->addWidget(le_in_send, 5, 4, 1, 2);
    connect(cbSendData, SIGNAL(currentTextChanged(QString)), le_in_send, SLOT(setText(QString)));
    QPushButton *pbSendData = new QPushButton("Send Data", this);
    connect(pbSendData, SIGNAL(clicked()), this, SLOT(sendData()));
    lo->addWidget(pbSendData, 5, 6, 1, 1);

    sbper->setMinimum(10);
    sbper->setMaximum(10000);
    sbper->setSuffix("ms");
    sbper->setValue(1000);
}

Context::~Context()
{
    delete ui;
}

void Context::setMode() {
    CuData options("refresh_mode", static_cast<int>(m_getRefreshMode()));
    if(options["refresh_mode"].toInt() == CuTReader::PolledRefresh)
        options["period"] = findChild<QSpinBox *>()->value();
    findChild<QuInputOutput *>()->getOutputContext()->sendData(options);
}

void Context::setPeriod() {
    findChild<QuInputOutput *>()->getOutputContext()->sendData(CuData("period", findChild<QSpinBox *>()->value()));
}

void Context::reloadProps()
{
    QuInputOutput *qio = findChild<QuInputOutput *>();
    CuTangoOptBuilder tob(qio->getOutputContext()->options());
    std::vector<std::string> props;
    props.push_back("values");
    tob.setFetchAttProps(props);
    // set options before setSource
    qio->getOutputContext()->setOptions(tob.options());
    qio->setSource(qio->source());
}

void Context::runModified()
{
    QuInputOutput *qio = findChild<QuInputOutput *>();
    QPushButton *b = qobject_cast<QPushButton *>(sender());
    QSpinBox *sbper = findChild<QSpinBox *>();
    CuTangoOptBuilder tob;
    tob.setPeriod(sbper->value());
    tob.setRefreshMode(m_getRefreshMode());
    qio->getOutputContext()->setOptions(tob.options());
    if(b->text() == "Start") {
        qio->setSource(findChild<QLineEdit *>("leSrc")->text());
    }
    else if(b->text() == "Stop")
        qio->unsetSource();
}

void Context::onNewData(const CuData &d)
{
    QLineEdit *le = findChild<QLineEdit *>("leRefreshInfo");
    if(d["mode"].isValid()) {
        le->setText(QString::fromStdString(d["mode"].toString()));
        findChild<QSpinBox *>()->setEnabled(le->text().compare("EVENT", Qt::CaseInsensitive) != 0);
    }
    if(d["period"].isValid())
        le->setText(le->text() + ": " + QString::fromStdString(d["period"].toString()) + "ms");
}

void Context::getData()
{
    CuData in;
    std::string key = findChild<QComboBox *>("cbGetData")->currentText().toStdString();
    in[key] = "-"; // need only the key
    findChild<QuInputOutput *>()->getOutputContext()->getData(in);
    // if the reader recognizes the desired key, it will be paired to a value.
    findChild<QLineEdit *>("leReceive")->setText(in[key].toString().c_str());
}

void Context::sendData()
{
    QString key, val, in = findChild<QLineEdit *>("le_in_send")->text();
    QStringList l = in.split(QRegExp("\\s+"));
    CuData din;
    foreach(QString expr, l)
    {
        QStringList pts = expr.split(":");
        if(pts.size() > 0)
        {
            key = pts.first();
            if(pts.size() == 2)
                val = pts.last();
            din[key.toStdString()] = val.toStdString();
        }
    }
    findChild<QuInputOutput *>()->getOutputContext()->sendData(din);
}

void Context::onLinkStatsRequest(QWidget *w)
{
    qDebug() << __FUNCTION__ << w;
}

CuTReader::RefreshMode Context::m_getRefreshMode()
{
    QComboBox *c = findChild<QComboBox *>("cbMode");
    if(c->currentText() == "event [change]")
        return (CuTReader::ChangeEventRefresh);
    else if(c->currentText() == "archive event")
        return CuTReader::ArchiveEventRefresh;
    else if(c->currentText() == "periodic event")
        return CuTReader::PeriodicEventRefresh;
    else if(c->currentText() == "manual")
        return (CuTReader::Manual);

    return CuTReader::PolledRefresh;
}

