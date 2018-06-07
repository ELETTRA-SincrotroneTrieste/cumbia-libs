#include "quconnectionsettingswidget.h"

#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <quplot_base.h>
#include <cumacros.h>
#include <cudatatypes_ex.h>
#include <QtDebug>

QuConnectionSettingsWidget::QuConnectionSettingsWidget(QWidget *w) : QWidget(w)
{
    QGridLayout *lo = new QGridLayout(this);
    int layout_row = 0;
    QLabel *lPeriod = new QLabel("Period", this);
    lPeriod->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    lo->addWidget(lPeriod, layout_row, 0, 1, 1);
    QSpinBox *sb = new QSpinBox(this);
    sb->setObjectName("sbPeriod");
    sb->setMinimum(10);
    sb->setMaximum(5000);
    sb->setValue(1000);
    sb->setSuffix("ms");
    lo->addWidget(sb, layout_row, 1, 1, 1);
    QComboBox *cb = new QComboBox(this);
    cb->setObjectName("cbRefreshMode");
    // CuTReader::PolledRefresh -> 0, CuTReader::ChangeEventRefresh -> 1
    cb->insertItems(0, QStringList() << "PolledRefresh" << "ChangeEventRefresh");
    lo->addWidget(cb, layout_row, 2, 1, 1);
    QPushButton *bApply = new QPushButton("Apply", this);
    connect(bApply, SIGNAL(clicked()), this, SLOT(changeRefresh()));
    lo->addWidget(bApply, layout_row, 3, 1, 1);
}

QuConnectionSettingsWidget::~QuConnectionSettingsWidget()
{
    pdelete("~QuConnectionSettingsWidget");
    qDebug() <<__FUNCTION__ << "parent was" << parentWidget();
}

void QuConnectionSettingsWidget::changeRefresh()
{
    int period = findChild<QSpinBox *>("sbPeriod")->value();
    int refmode = findChild<QComboBox *>("cbRefreshMode")->currentIndex();
    CuData options;
    options[CuXDType::Period] = period;
    options[CuXDType::RefreshMode] = refmode;
    emit settingsChanged(options);
}
