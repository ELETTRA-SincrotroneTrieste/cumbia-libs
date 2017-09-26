/********************************************************************************
** Form generated from reading UI file 'tledbooleditor.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TLEDBOOLEDITOR_H
#define UI_TLEDBOOLEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "eled.h"

QT_BEGIN_NAMESPACE

class Ui_TLedBoolEditor
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBoxFalse;
    QGridLayout *gridLayout;
    QPushButton *pushColorTrue;
    QSpacerItem *spacerItem;
    QSpacerItem *spacerItem1;
    QSpacerItem *spacerItem2;
    QSpacerItem *spacerItem3;
    QPushButton *pushColorFalse;
    QLabel *label_5;
    QLabel *label_6;
    ELed *eLed;
    ELed *eLed_2;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem4;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *TLedBoolEditor)
    {
        if (TLedBoolEditor->objectName().isEmpty())
            TLedBoolEditor->setObjectName(QStringLiteral("TLedBoolEditor"));
        TLedBoolEditor->resize(192, 200);
        vboxLayout = new QVBoxLayout(TLedBoolEditor);
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(3, 3, 3, 3);
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        vboxLayout->setContentsMargins(9, 9, 9, 9);
        groupBoxFalse = new QGroupBox(TLedBoolEditor);
        groupBoxFalse->setObjectName(QStringLiteral("groupBoxFalse"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(groupBoxFalse->sizePolicy().hasHeightForWidth());
        groupBoxFalse->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBoxFalse);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(3, 3, 3, 3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(9, 9, 9, 9);
        pushColorTrue = new QPushButton(groupBoxFalse);
        pushColorTrue->setObjectName(QStringLiteral("pushColorTrue"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushColorTrue->sizePolicy().hasHeightForWidth());
        pushColorTrue->setSizePolicy(sizePolicy1);
        pushColorTrue->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(pushColorTrue, 1, 3, 1, 1);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem, 2, 3, 1, 1);

        spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem1, 0, 3, 1, 1);

        spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem2, 5, 3, 1, 1);

        spacerItem3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem3, 3, 3, 1, 1);

        pushColorFalse = new QPushButton(groupBoxFalse);
        pushColorFalse->setObjectName(QStringLiteral("pushColorFalse"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushColorFalse->sizePolicy().hasHeightForWidth());
        pushColorFalse->setSizePolicy(sizePolicy2);
        pushColorFalse->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(pushColorFalse, 4, 3, 1, 1);

        label_5 = new QLabel(groupBoxFalse);
        label_5->setObjectName(QStringLiteral("label_5"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(label_5, 0, 0, 3, 1);

        label_6 = new QLabel(groupBoxFalse);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 3, 0, 3, 1);

        eLed = new ELed(groupBoxFalse);
        eLed->setObjectName(QStringLiteral("eLed"));

        gridLayout->addWidget(eLed, 1, 1, 1, 1);

        eLed_2 = new ELed(groupBoxFalse);
        eLed_2->setObjectName(QStringLiteral("eLed_2"));

        gridLayout->addWidget(eLed_2, 4, 1, 1, 1);


        vboxLayout->addWidget(groupBoxFalse);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        spacerItem4 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem4);

        okButton = new QPushButton(TLedBoolEditor);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout->addWidget(okButton);

        cancelButton = new QPushButton(TLedBoolEditor);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout->addWidget(cancelButton);


        vboxLayout->addLayout(hboxLayout);

        QWidget::setTabOrder(pushColorTrue, cancelButton);
        QWidget::setTabOrder(cancelButton, okButton);

        retranslateUi(TLedBoolEditor);
        QObject::connect(okButton, SIGNAL(clicked()), TLedBoolEditor, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), TLedBoolEditor, SLOT(reject()));

        QMetaObject::connectSlotsByName(TLedBoolEditor);
    } // setupUi

    void retranslateUi(QDialog *TLedBoolEditor)
    {
        TLedBoolEditor->setWindowTitle(QApplication::translate("TLedBoolEditor", "Boolean Behaviour", Q_NULLPTR));
        groupBoxFalse->setTitle(QApplication::translate("TLedBoolEditor", "Choose Colors", Q_NULLPTR));
        pushColorTrue->setText(QApplication::translate("TLedBoolEditor", "Edit", Q_NULLPTR));
        pushColorFalse->setText(QApplication::translate("TLedBoolEditor", "Edit", Q_NULLPTR));
        label_5->setText(QApplication::translate("TLedBoolEditor", "True:", Q_NULLPTR));
        label_6->setText(QApplication::translate("TLedBoolEditor", "False:", Q_NULLPTR));
        okButton->setText(QApplication::translate("TLedBoolEditor", "OK", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("TLedBoolEditor", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TLedBoolEditor: public Ui_TLedBoolEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TLEDBOOLEDITOR_H
