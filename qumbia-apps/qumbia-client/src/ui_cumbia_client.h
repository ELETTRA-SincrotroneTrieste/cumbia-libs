/********************************************************************************
** Form generated from reading UI file 'cumbia_client.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUMBIA_CLIENT_H
#define UI_CUMBIA_CLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CumbiaClient
{
public:
    QGridLayout *gridLayout;
    QComboBox *cbRefMode;
    QLabel *label;
    QSpinBox *sbPeriod;
    QPushButton *pbSetSources;
    QPushButton *pbUnsetSrc;
    QLineEdit *leSrcs;
    QPushButton *pbApplyRefresh;
    QWidget *widget;
    QLabel *labelTitle;

    void setupUi(QWidget *CumbiaClient)
    {
        if (CumbiaClient->objectName().isEmpty())
            CumbiaClient->setObjectName(QStringLiteral("CumbiaClient"));
        CumbiaClient->resize(544, 300);
        gridLayout = new QGridLayout(CumbiaClient);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        cbRefMode = new QComboBox(CumbiaClient);
        cbRefMode->setObjectName(QStringLiteral("cbRefMode"));

        gridLayout->addWidget(cbRefMode, 2, 2, 1, 1);

        label = new QLabel(CumbiaClient);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        sbPeriod = new QSpinBox(CumbiaClient);
        sbPeriod->setObjectName(QStringLiteral("sbPeriod"));
        sbPeriod->setMinimum(10);
        sbPeriod->setMaximum(10000);
        sbPeriod->setSingleStep(100);
        sbPeriod->setValue(1000);

        gridLayout->addWidget(sbPeriod, 2, 1, 1, 1);

        pbSetSources = new QPushButton(CumbiaClient);
        pbSetSources->setObjectName(QStringLiteral("pbSetSources"));

        gridLayout->addWidget(pbSetSources, 3, 2, 1, 1);

        pbUnsetSrc = new QPushButton(CumbiaClient);
        pbUnsetSrc->setObjectName(QStringLiteral("pbUnsetSrc"));

        gridLayout->addWidget(pbUnsetSrc, 3, 3, 1, 1);

        leSrcs = new QLineEdit(CumbiaClient);
        leSrcs->setObjectName(QStringLiteral("leSrcs"));

        gridLayout->addWidget(leSrcs, 3, 0, 1, 2);

        pbApplyRefresh = new QPushButton(CumbiaClient);
        pbApplyRefresh->setObjectName(QStringLiteral("pbApplyRefresh"));

        gridLayout->addWidget(pbApplyRefresh, 2, 3, 1, 1);

        widget = new QWidget(CumbiaClient);
        widget->setObjectName(QStringLiteral("widget"));

        gridLayout->addWidget(widget, 1, 0, 1, 4);

        labelTitle = new QLabel(CumbiaClient);
        labelTitle->setObjectName(QStringLiteral("labelTitle"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelTitle->sizePolicy().hasHeightForWidth());
        labelTitle->setSizePolicy(sizePolicy);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        labelTitle->setFont(font);
        labelTitle->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(labelTitle, 0, 0, 1, 4);


        retranslateUi(CumbiaClient);

        QMetaObject::connectSlotsByName(CumbiaClient);
    } // setupUi

    void retranslateUi(QWidget *CumbiaClient)
    {
        CumbiaClient->setWindowTitle(QApplication::translate("CumbiaClient", "CumbiaClient", Q_NULLPTR));
        cbRefMode->clear();
        cbRefMode->insertItems(0, QStringList()
         << QApplication::translate("CumbiaClient", "PolledRefresh", Q_NULLPTR)
         << QApplication::translate("CumbiaClient", "ChangeEventRefresh", Q_NULLPTR)
        );
        label->setText(QApplication::translate("CumbiaClient", "Period", Q_NULLPTR));
        pbSetSources->setText(QApplication::translate("CumbiaClient", "Set sources", Q_NULLPTR));
        pbUnsetSrc->setText(QApplication::translate("CumbiaClient", "Unset Sources", Q_NULLPTR));
        pbApplyRefresh->setText(QApplication::translate("CumbiaClient", "Apply", Q_NULLPTR));
        labelTitle->setText(QApplication::translate("CumbiaClient", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CumbiaClient: public Ui_CumbiaClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUMBIA_CLIENT_H
