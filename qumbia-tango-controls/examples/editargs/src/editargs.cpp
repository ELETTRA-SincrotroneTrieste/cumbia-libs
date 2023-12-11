#include "editargs.h"
#include "ui_editargs.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
#include <cucontext.h>
#include <cucontrolsreader_abs.h>
// cumbia

#include <QDoubleSpinBox>
#include <QLabel>

Editargs::Editargs(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;
    ui = new Ui::Editargs;
    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    QStringList srcs;
    QGridLayout * lo = new QGridLayout(ui->gbEditA);
    int r = lo->rowCount();
    for(int i = 1; i < qApp->arguments().size(); i++) {
        const QString a = qApp->arguments().at(i);
        if(a.count("/") < 2)
            continue;
        QLabel *l = new QLabel(a, this);
        lo->addWidget(l, r, 0, 1, 1);
        for(int j = 0; j < 10; j++) {
            QDoubleSpinBox *sb = new QDoubleSpinBox(this);
            sb->setObjectName(QString("sb_%1_%2").arg(i).arg(j));
            lo->addWidget(sb, r, j + 1, 1 ,1 );
            sb->setMinimum(-1e12);
            sb->setMaximum(1e12);
        }
        srcs << a;
        r++;
    }
    ui->plot->setSources(srcs);
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
    const QString& n = sender()->objectName();
    QString idx = n.split('_').at(1); // find spinboxes in row idx
    QList<QDoubleSpinBox *> sbs;
    for(int j = 0; j < 10; j++)
        sbs << findChild<QDoubleSpinBox *>(QString("sb_%1_%2").arg(idx).arg(j));
    foreach(const QDoubleSpinBox* sb, sbs)
        args.push_back(QString::number(sb->value()).toStdString());
    CuData a(CuDType::Args, args);  // CuData a("args", args)
    printf("Editargs.onSbValueChanged: sending data %s on context %p\n", datos(a), ui->plot->getContext());

    QList<CuControlsReaderA *> readers = ui->plot->getContext()->readers();
    foreach(CuControlsReaderA *r, readers) {
        if(r->source() == qApp->arguments().at(idx.toInt())) {
            r->sendData(CuData("args", args));
            if(ui->cbRefreshOnChange->isChecked()) r->sendData(CuData("read", ""));
        }
    }
}

void Editargs::onReadClicked() {
    QList<CuControlsReaderA *> readers = ui->plot->getContext()->readers();
    foreach(CuControlsReaderA *r, readers)
        r->sendData(CuData("read", ""));
}
