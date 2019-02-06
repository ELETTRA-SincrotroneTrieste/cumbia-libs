#include "conversiondialog.h"
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QtDebug>
#include <QApplication>
#include <QLineEdit>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>

#include "definitions.h"
#include "fileprocessor_a.h"

ConversionDialog::ConversionDialog(QWidget *parent, const QString& outpath, bool overwrite) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    QVBoxLayout *vblo = new QVBoxLayout(this);
    QGridLayout *lo = new QGridLayout(this);
    vblo->addLayout(lo, 3);
    QTreeWidget *tree = new QTreeWidget(this);
    tree->setObjectName("maintree");
    QPushButton *pbOk = new QPushButton("Convert", this);
    pbOk->setObjectName("pbConvert");
    pbOk->setDisabled(true);
    QPushButton *pbCanc = new QPushButton("Close", this);
    connect(pbOk, SIGNAL(clicked()), this, SLOT(slotOkClicked()));
    connect(pbCanc, SIGNAL(clicked()), this, SLOT(reject()));
    tree->setColumnCount(6);
    /*
     * type = ty;
        old_e = old;
        new_e = ne;
        quality = q;
        comment = comm;
        lineno = line;
    */
    tree->setHeaderLabels(QStringList() << "Type" << "from" << "to" << "file" << "quality" << "comment" << "line");
    tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    lo->addWidget(tree, 0, 0, 6, 7);
    lo->addWidget(pbOk, 6, 0, 1, 1);
    lo->addWidget(pbCanc, 6, 5, 1, 1);

    QLabel *label = new QLabel("Output directory:");
    label->setAlignment(Qt::AlignHCenter|Qt::AlignRight);
    lo->addWidget(label, 6, 1, 1, 1);

    QLineEdit *outpathLe = new QLineEdit(this);
    overwrite ? outpathLe->setText(outpath) :  outpathLe->setText(outpath + "/to_cumbia");
    outpathLe->setObjectName("outpathLe");
    lo->addWidget(outpathLe, 6, 2, 1, 4);

    QPushButton *pbOutPath = new QPushButton("Change", this);
    pbOutPath->setObjectName("pbOutPath");
    connect(pbOutPath, SIGNAL(clicked()), this, SLOT(changeOutPath()));
    lo->addWidget(pbOutPath, 6, 6, 1, 1);

    QLabel *problemsLabel = new QLabel("Conversion cannot be performed safely this time", this);
    problemsLabel->setObjectName("problemsLabel");
    problemsLabel->setAlignment(Qt::AlignHCenter|Qt::AlignRight);
    lo->addWidget(problemsLabel, 7, 0, 1, 4);

    QCheckBox *problemsCb = new QCheckBox("OK, try your best, I'll correct manually later", this);
    problemsCb->setObjectName("problemsCb");
    lo->addWidget(problemsCb, 7, 4, 1, 3);
    connect(problemsCb, SIGNAL(toggled(bool)), pbOk, SLOT(setEnabled(bool)));

    m_setProblemWidgetsVisible(false);

    QTreeWidget *twout = new QTreeWidget(this);
    twout->setColumnCount(2);
    twout->setHeaderLabels(QStringList() << "file name" << "relative path" << "success");
    twout->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    twout->setVisible(false);
    twout->setObjectName("treeOut");
    vblo->addWidget(twout, 1);

    resize(1000, 600);
}

ConversionDialog::~ConversionDialog()
{
    printf("~ConversionDialog %p\n", this);
}

QString ConversionDialog::outputPath() const
{
    return findChild<QLineEdit *>("outpathLe")->text();
}

void ConversionDialog::addLogs(const QList<OpQuality> &log)
{
    foreach(const OpQuality &q, log) {
        QMap<Quality::Level, QString> quamap;
        quamap[Quality::Ok] = "ok";
        quamap[Quality::Warn] = "warn";
        quamap[Quality::Critical] = "critical";
        QString in = q.old_e;
        QString out = q.new_e;
        if(in.length() > 60) {
            in = in.section("\n", 0, 0);
            in.truncate(60);
            in += "...";
        }
        if(out.length() > 60) {
            out = out.section("\n", 0, 0);
            out.truncate(60);
            out += "...";
        }
        QTreeWidgetItem *it = new QTreeWidgetItem(findChild<QTreeWidget*>(),
                                                  QStringList() << q.type << in << out << q.file <<
                                                  quamap[q.quality] << q.comment  << QString::number(q.lineno));
        it->setToolTip(1, q.old_e);
        it->setToolTip(2, q.new_e);
        for(int i = 0; i < it->columnCount(); i++) {
            if(q.quality == Quality::Ok)
                it->setBackground(i, QColor(Qt::green));
            else if(q.quality == Quality::Warn)
                it->setBackground(i, QColor(Qt::yellow));
            else
                it->setBackground(i, QColor(Qt::red));
        }
        QApplication::processEvents();
    }
}

void ConversionDialog::conversionFinished(bool ok)
{
    findChild<QPushButton *>("pbConvert")->setEnabled(ok);
    foreach(QTreeWidgetItem *it, findChild<QTreeWidget*>()->findItems("*", Qt::MatchWildcard)) {
        if(it->text(4) == "critical")
        {
            findChild<QTreeWidget*>()->scrollToItem(it);
            m_setProblemWidgetsVisible(true);
            findChild<QPushButton *>("pbConvert")->setDisabled(true);
            break;
        }
    }
}

void ConversionDialog::outputFileWritten(const QString &name, const QString &filerelpath, bool ok)
{
    findChild<QTreeWidget *>("treeOut")->setVisible(true);
    QTreeWidgetItem *it = new QTreeWidgetItem(findChild<QTreeWidget *>("treeOut"), QStringList() << name << filerelpath << (ok ? "ok" : "failed"));
    for(int i = 0; i < it->columnCount() && !ok; i++)
        it->setBackground(i, QColor(Qt::red));
}

void ConversionDialog::changeOutPath()
{
    QString f = QFileDialog::getExistingDirectory(this, "Select output folder",
                                                  findChild<QLineEdit *>("outpathLe")->text(),
                                                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!f.isEmpty())
        findChild<QLineEdit *>("outpathLe")->setText(f);
}

void ConversionDialog::slotOkClicked()
{
    findChild<QTreeWidget *>("treeOut")->setVisible(false);
    emit okClicked();
}

bool ConversionDialog::m_setProblemWidgetsVisible(bool v)
{
    foreach(QWidget* w, findChildren<QWidget*>(QRegExp("problems.*")))
        w->setVisible(v);
    return true;
}
