/********************************************************************************
** Form generated from reading UI file 'generic_client.ui'
**
** Created by: Qt User Interface Compiler version 5.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENERIC_CLIENT_H
#define UI_GENERIC_CLIENT_H

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

class Ui_GenericClient
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

    void setupUi(QWidget *GenericClient)
    {
        if (GenericClient->objectName().isEmpty())
            GenericClient->setObjectName(QStringLiteral("GenericClient"));
        GenericClient->resize(544, 300);
        gridLayout = new QGridLayout(GenericClient);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        cbRefMode = new QComboBox(GenericClient);
        cbRefMode->setObjectName(QStringLiteral("cbRefMode"));

        gridLayout->addWidget(cbRefMode, 2, 2, 1, 1);

        label = new QLabel(GenericClient);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        sbPeriod = new QSpinBox(GenericClient);
        sbPeriod->setObjectName(QStringLiteral("sbPeriod"));
        sbPeriod->setMinimum(10);
        sbPeriod->setMaximum(10000);
        sbPeriod->setSingleStep(100);
        sbPeriod->setValue(1000);

        gridLayout->addWidget(sbPeriod, 2, 1, 1, 1);

        pbSetSources = new QPushButton(GenericClient);
        pbSetSources->setObjectName(QStringLiteral("pbSetSources"));

        gridLayout->addWidget(pbSetSources, 3, 2, 1, 1);

        pbUnsetSrc = new QPushButton(GenericClient);
        pbUnsetSrc->setObjectName(QStringLiteral("pbUnsetSrc"));

        gridLayout->addWidget(pbUnsetSrc, 3, 3, 1, 1);

        leSrcs = new QLineEdit(GenericClient);
        leSrcs->setObjectName(QStringLiteral("leSrcs"));

        gridLayout->addWidget(leSrcs, 3, 0, 1, 2);

        pbApplyRefresh = new QPushButton(GenericClient);
        pbApplyRefresh->setObjectName(QStringLiteral("pbApplyRefresh"));

        gridLayout->addWidget(pbApplyRefresh, 2, 3, 1, 1);

        widget = new QWidget(GenericClient);
        widget->setObjectName(QStringLiteral("widget"));

        gridLayout->addWidget(widget, 1, 0, 1, 4);

        labelTitle = new QLabel(GenericClient);
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


        retranslateUi(GenericClient);

        QMetaObject::connectSlotsByName(GenericClient);
    } // setupUi

    void retranslateUi(QWidget *GenericClient)
    {
        GenericClient->setWindowTitle(QApplication::translate("GenericClient", "GenericClient", Q_NULLPTR));
        cbRefMode->clear();
        cbRefMode->insertItems(0, QStringList()
         << QApplication::translate("GenericClient", "PolledRefresh", Q_NULLPTR)
         << QApplication::translate("GenericClient", "ChangeEventRefresh", Q_NULLPTR)
        );
        label->setText(QApplication::translate("GenericClient", "Period", Q_NULLPTR));
        pbSetSources->setText(QApplication::translate("GenericClient", "Set sources", Q_NULLPTR));
        pbUnsetSrc->setText(QApplication::translate("GenericClient", "Unset Sources", Q_NULLPTR));
        pbApplyRefresh->setText(QApplication::translate("GenericClient", "Apply", Q_NULLPTR));
        labelTitle->setText(QApplication::translate("GenericClient", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GenericClient: public Ui_GenericClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERIC_CLIENT_H
