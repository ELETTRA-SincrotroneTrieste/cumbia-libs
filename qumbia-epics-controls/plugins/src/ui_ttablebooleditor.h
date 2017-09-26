/********************************************************************************
** Form generated from reading UI file 'ttablebooleditor.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TTABLEBOOLEDITOR_H
#define UI_TTABLEBOOLEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include "eflag.h"

QT_BEGIN_NAMESPACE

class Ui_TTableBoolEditor
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox_4;
    QHBoxLayout *hboxLayout;
    QLabel *label_10;
    QSpinBox *spinNumRows;
    QLabel *label_9;
    QSpinBox *spinNumColumns;
    QHBoxLayout *hboxLayout1;
    QLabel *label_11;
    QLineEdit *lineMask;
    QHBoxLayout *hboxLayout2;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout;
    QLabel *label_8;
    QSpinBox *spinColumn;
    QCheckBox *checkApplyAll;
    QSpinBox *spinRow;
    QLabel *label_7;
    QPushButton *pushApply;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QPushButton *pushColorTrue;
    QLabel *label_3;
    QLabel *label;
    QLineEdit *lineEditTrue;
    QSpacerItem *spacerItem1;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout2;
    QLineEdit *lineEditFalse;
    QLabel *label_4;
    QSpacerItem *spacerItem2;
    QPushButton *pushColorFalse;
    QLabel *label_2;
    QGroupBox *groupBoxFalse;
    QGridLayout *gridLayout3;
    QLabel *label_6;
    EFlag *eFlagFalse;
    EFlag *eFlagTrue;
    QLabel *label_5;
    QHBoxLayout *hboxLayout4;
    QSpacerItem *spacerItem3;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *TTableBoolEditor)
    {
        if (TTableBoolEditor->objectName().isEmpty())
            TTableBoolEditor->setObjectName(QStringLiteral("TTableBoolEditor"));
        TTableBoolEditor->resize(472, 720);
        vboxLayout = new QVBoxLayout(TTableBoolEditor);
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(3, 3, 3, 3);
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        vboxLayout->setContentsMargins(9, 9, 9, 9);
        groupBox_4 = new QGroupBox(TTableBoolEditor);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy);
        hboxLayout = new QHBoxLayout(groupBox_4);
        hboxLayout->setSpacing(6);
        hboxLayout->setContentsMargins(3, 3, 3, 3);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        hboxLayout->setContentsMargins(9, 9, 9, 9);
        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        hboxLayout->addWidget(label_10);

        spinNumRows = new QSpinBox(groupBox_4);
        spinNumRows->setObjectName(QStringLiteral("spinNumRows"));
        spinNumRows->setMinimum(1);
        spinNumRows->setMaximum(16);

        hboxLayout->addWidget(spinNumRows);

        label_9 = new QLabel(groupBox_4);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        hboxLayout->addWidget(label_9);

        spinNumColumns = new QSpinBox(groupBox_4);
        spinNumColumns->setObjectName(QStringLiteral("spinNumColumns"));
        spinNumColumns->setMinimum(1);
        spinNumColumns->setMaximum(16);

        hboxLayout->addWidget(spinNumColumns);


        vboxLayout->addWidget(groupBox_4);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(TTableBoolEditor);
        label_11->setObjectName(QStringLiteral("label_11"));

        hboxLayout1->addWidget(label_11);

        lineMask = new QLineEdit(TTableBoolEditor);
        lineMask->setObjectName(QStringLiteral("lineMask"));

        hboxLayout1->addWidget(lineMask);


        vboxLayout->addLayout(hboxLayout1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setObjectName(QStringLiteral("hboxLayout2"));
        hboxLayout2->setContentsMargins(0, 0, 0, 0);
        groupBox_3 = new QGroupBox(TTableBoolEditor);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(groupBox_3);
        gridLayout->setSpacing(3);
        gridLayout->setContentsMargins(3, 3, 3, 3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(6);
        gridLayout->setVerticalSpacing(6);
        gridLayout->setContentsMargins(9, 9, 9, 9);
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_8, 1, 0, 1, 1);

        spinColumn = new QSpinBox(groupBox_3);
        spinColumn->setObjectName(QStringLiteral("spinColumn"));
        spinColumn->setEnabled(true);

        gridLayout->addWidget(spinColumn, 1, 1, 1, 1);

        checkApplyAll = new QCheckBox(groupBox_3);
        checkApplyAll->setObjectName(QStringLiteral("checkApplyAll"));
        checkApplyAll->setChecked(false);

        gridLayout->addWidget(checkApplyAll, 0, 2, 2, 1);

        spinRow = new QSpinBox(groupBox_3);
        spinRow->setObjectName(QStringLiteral("spinRow"));
        spinRow->setEnabled(true);

        gridLayout->addWidget(spinRow, 0, 1, 1, 1);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_7, 0, 0, 1, 1);


        hboxLayout2->addWidget(groupBox_3);

        pushApply = new QPushButton(TTableBoolEditor);
        pushApply->setObjectName(QStringLiteral("pushApply"));

        hboxLayout2->addWidget(pushApply);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem);


        vboxLayout->addLayout(hboxLayout2);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setSpacing(6);
        hboxLayout3->setObjectName(QStringLiteral("hboxLayout3"));
        hboxLayout3->setContentsMargins(0, 0, 0, 0);
        groupBox = new QGroupBox(TTableBoolEditor);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(1);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        gridLayout1 = new QGridLayout(groupBox);
        gridLayout1->setSpacing(3);
        gridLayout1->setContentsMargins(3, 3, 3, 3);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        gridLayout1->setHorizontalSpacing(6);
        gridLayout1->setVerticalSpacing(6);
        gridLayout1->setContentsMargins(9, 9, 9, 9);
        pushColorTrue = new QPushButton(groupBox);
        pushColorTrue->setObjectName(QStringLiteral("pushColorTrue"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(pushColorTrue->sizePolicy().hasHeightForWidth());
        pushColorTrue->setSizePolicy(sizePolicy3);
        pushColorTrue->setMaximumSize(QSize(40, 16777215));

        gridLayout1->addWidget(pushColorTrue, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout1->addWidget(label_3, 1, 0, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout1->addWidget(label, 0, 0, 1, 1);

        lineEditTrue = new QLineEdit(groupBox);
        lineEditTrue->setObjectName(QStringLiteral("lineEditTrue"));

        gridLayout1->addWidget(lineEditTrue, 0, 1, 1, 2);

        spacerItem1 = new QSpacerItem(91, 27, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem1, 1, 2, 1, 1);


        hboxLayout3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(TTableBoolEditor);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        sizePolicy2.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy2);
        gridLayout2 = new QGridLayout(groupBox_2);
        gridLayout2->setSpacing(3);
        gridLayout2->setContentsMargins(3, 3, 3, 3);
        gridLayout2->setObjectName(QStringLiteral("gridLayout2"));
        gridLayout2->setHorizontalSpacing(6);
        gridLayout2->setVerticalSpacing(6);
        gridLayout2->setContentsMargins(9, 9, 9, 9);
        lineEditFalse = new QLineEdit(groupBox_2);
        lineEditFalse->setObjectName(QStringLiteral("lineEditFalse"));

        gridLayout2->addWidget(lineEditFalse, 0, 1, 1, 2);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout2->addWidget(label_4, 1, 0, 1, 1);

        spacerItem2 = new QSpacerItem(50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem2, 1, 2, 1, 1);

        pushColorFalse = new QPushButton(groupBox_2);
        pushColorFalse->setObjectName(QStringLiteral("pushColorFalse"));
        sizePolicy3.setHeightForWidth(pushColorFalse->sizePolicy().hasHeightForWidth());
        pushColorFalse->setSizePolicy(sizePolicy3);
        pushColorFalse->setMaximumSize(QSize(40, 16777215));

        gridLayout2->addWidget(pushColorFalse, 1, 1, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout2->addWidget(label_2, 0, 0, 1, 1);


        hboxLayout3->addWidget(groupBox_2);


        vboxLayout->addLayout(hboxLayout3);

        groupBoxFalse = new QGroupBox(TTableBoolEditor);
        groupBoxFalse->setObjectName(QStringLiteral("groupBoxFalse"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(2);
        sizePolicy4.setVerticalStretch(2);
        sizePolicy4.setHeightForWidth(groupBoxFalse->sizePolicy().hasHeightForWidth());
        groupBoxFalse->setSizePolicy(sizePolicy4);
        gridLayout3 = new QGridLayout(groupBoxFalse);
        gridLayout3->setSpacing(3);
        gridLayout3->setContentsMargins(3, 3, 3, 3);
        gridLayout3->setObjectName(QStringLiteral("gridLayout3"));
        gridLayout3->setHorizontalSpacing(6);
        gridLayout3->setVerticalSpacing(6);
        gridLayout3->setContentsMargins(9, 9, 9, 9);
        label_6 = new QLabel(groupBoxFalse);
        label_6->setObjectName(QStringLiteral("label_6"));
        sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy);

        gridLayout3->addWidget(label_6, 0, 1, 1, 1);

        eFlagFalse = new EFlag(groupBoxFalse);
        eFlagFalse->setObjectName(QStringLiteral("eFlagFalse"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(1);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(eFlagFalse->sizePolicy().hasHeightForWidth());
        eFlagFalse->setSizePolicy(sizePolicy5);

        gridLayout3->addWidget(eFlagFalse, 1, 1, 1, 1);

        eFlagTrue = new EFlag(groupBoxFalse);
        eFlagTrue->setObjectName(QStringLiteral("eFlagTrue"));
        sizePolicy5.setHeightForWidth(eFlagTrue->sizePolicy().hasHeightForWidth());
        eFlagTrue->setSizePolicy(sizePolicy5);

        gridLayout3->addWidget(eFlagTrue, 1, 0, 1, 1);

        label_5 = new QLabel(groupBoxFalse);
        label_5->setObjectName(QStringLiteral("label_5"));
        sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy);

        gridLayout3->addWidget(label_5, 0, 0, 1, 1);


        vboxLayout->addWidget(groupBoxFalse);

        hboxLayout4 = new QHBoxLayout();
        hboxLayout4->setSpacing(6);
        hboxLayout4->setObjectName(QStringLiteral("hboxLayout4"));
        hboxLayout4->setContentsMargins(0, 0, 0, 0);
        spacerItem3 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout4->addItem(spacerItem3);

        okButton = new QPushButton(TTableBoolEditor);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout4->addWidget(okButton);

        cancelButton = new QPushButton(TTableBoolEditor);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout4->addWidget(cancelButton);


        vboxLayout->addLayout(hboxLayout4);


        retranslateUi(TTableBoolEditor);
        QObject::connect(checkApplyAll, SIGNAL(toggled(bool)), spinRow, SLOT(setDisabled(bool)));
        QObject::connect(checkApplyAll, SIGNAL(toggled(bool)), spinColumn, SLOT(setDisabled(bool)));
        QObject::connect(cancelButton, SIGNAL(clicked()), TTableBoolEditor, SLOT(reject()));
        QObject::connect(okButton, SIGNAL(clicked()), TTableBoolEditor, SLOT(accept()));

        QMetaObject::connectSlotsByName(TTableBoolEditor);
    } // setupUi

    void retranslateUi(QDialog *TTableBoolEditor)
    {
        TTableBoolEditor->setWindowTitle(QApplication::translate("TTableBoolEditor", "Boolean Behaviour", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("TTableBoolEditor", "Global Properties", Q_NULLPTR));
        label_10->setText(QApplication::translate("TTableBoolEditor", "Number of Rows:", Q_NULLPTR));
        label_9->setText(QApplication::translate("TTableBoolEditor", "Number of Columns:", Q_NULLPTR));
        label_11->setText(QApplication::translate("TTableBoolEditor", "Bits to display:", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        lineMask->setToolTip(QApplication::translate("TTableBoolEditor", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">comma-separated list of bits (e.g. 4,6,8,10)</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        groupBox_3->setTitle(QApplication::translate("TTableBoolEditor", "Cell to Edit", Q_NULLPTR));
        label_8->setText(QApplication::translate("TTableBoolEditor", "Column:", Q_NULLPTR));
        checkApplyAll->setText(QApplication::translate("TTableBoolEditor", "Apply to all", Q_NULLPTR));
        label_7->setText(QApplication::translate("TTableBoolEditor", "Row:", Q_NULLPTR));
        pushApply->setText(QApplication::translate("TTableBoolEditor", "Apply to Cell", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("TTableBoolEditor", "Displaying a True value", Q_NULLPTR));
        pushColorTrue->setText(QString());
        label_3->setText(QApplication::translate("TTableBoolEditor", "Color:", Q_NULLPTR));
        label->setText(QApplication::translate("TTableBoolEditor", "String:", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("TTableBoolEditor", "Displaying a False value", Q_NULLPTR));
        label_4->setText(QApplication::translate("TTableBoolEditor", "Color:", Q_NULLPTR));
        pushColorFalse->setText(QString());
        label_2->setText(QApplication::translate("TTableBoolEditor", "String:", Q_NULLPTR));
        groupBoxFalse->setTitle(QApplication::translate("TTableBoolEditor", "Example", Q_NULLPTR));
        label_6->setText(QApplication::translate("TTableBoolEditor", "False:", Q_NULLPTR));
        label_5->setText(QApplication::translate("TTableBoolEditor", "True:", Q_NULLPTR));
        okButton->setText(QApplication::translate("TTableBoolEditor", "OK", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("TTableBoolEditor", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TTableBoolEditor: public Ui_TTableBoolEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TTABLEBOOLEDITOR_H
