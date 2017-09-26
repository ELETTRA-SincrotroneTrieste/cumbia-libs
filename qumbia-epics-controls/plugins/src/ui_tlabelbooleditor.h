/********************************************************************************
** Form generated from reading UI file 'tlabelbooleditor.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TLABELBOOLEDITOR_H
#define UI_TLABELBOOLEDITOR_H

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
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_TLabelBoolEditor
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    QSpacerItem *spacerItem1;
    QGroupBox *groupBoxFalse;
    QGridLayout *gridLayout1;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *eLabelTrue;
    QLineEdit *eLabelFalse;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout2;
    QLineEdit *lineEditFalse;
    QLabel *label_4;
    QSpacerItem *spacerItem2;
    QPushButton *pushColorFalse;
    QLabel *label_2;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem3;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox;
    QGridLayout *gridLayout3;
    QPushButton *pushColorTrue;
    QSpacerItem *spacerItem4;
    QLineEdit *lineEditTrue;
    QLabel *label_3;
    QLabel *label;

    void setupUi(QDialog *TLabelBoolEditor)
    {
        if (TLabelBoolEditor->objectName().isEmpty())
            TLabelBoolEditor->setObjectName(QStringLiteral("TLabelBoolEditor"));
        TLabelBoolEditor->resize(336, 256);
        gridLayout = new QGridLayout(TLabelBoolEditor);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(9, 9, 9, 9);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 1, 3, 1, 1);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem1, 1, 0, 1, 1);

        groupBoxFalse = new QGroupBox(TLabelBoolEditor);
        groupBoxFalse->setObjectName(QStringLiteral("groupBoxFalse"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(groupBoxFalse->sizePolicy().hasHeightForWidth());
        groupBoxFalse->setSizePolicy(sizePolicy);
        gridLayout1 = new QGridLayout(groupBoxFalse);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(9, 9, 9, 9);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        label_5 = new QLabel(groupBoxFalse);
        label_5->setObjectName(QStringLiteral("label_5"));
        QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);

        gridLayout1->addWidget(label_5, 0, 0, 1, 1);

        label_6 = new QLabel(groupBoxFalse);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout1->addWidget(label_6, 1, 0, 1, 1);

        eLabelTrue = new QLineEdit(groupBoxFalse);
        eLabelTrue->setObjectName(QStringLiteral("eLabelTrue"));
        eLabelTrue->setFocusPolicy(Qt::NoFocus);
        eLabelTrue->setAlignment(Qt::AlignHCenter);
        eLabelTrue->setReadOnly(true);

        gridLayout1->addWidget(eLabelTrue, 0, 1, 1, 1);

        eLabelFalse = new QLineEdit(groupBoxFalse);
        eLabelFalse->setObjectName(QStringLiteral("eLabelFalse"));
        eLabelFalse->setFocusPolicy(Qt::NoFocus);
        eLabelFalse->setFrame(true);
        eLabelFalse->setAlignment(Qt::AlignHCenter);
        eLabelFalse->setReadOnly(true);

        gridLayout1->addWidget(eLabelFalse, 1, 1, 1, 1);


        gridLayout->addWidget(groupBoxFalse, 1, 1, 1, 2);

        groupBox_2 = new QGroupBox(TLabelBoolEditor);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        gridLayout2 = new QGridLayout(groupBox_2);
        gridLayout2->setSpacing(6);
        gridLayout2->setContentsMargins(9, 9, 9, 9);
        gridLayout2->setObjectName(QStringLiteral("gridLayout2"));
        lineEditFalse = new QLineEdit(groupBox_2);
        lineEditFalse->setObjectName(QStringLiteral("lineEditFalse"));

        gridLayout2->addWidget(lineEditFalse, 0, 1, 1, 2);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout2->addWidget(label_4, 1, 0, 1, 1);

        spacerItem2 = new QSpacerItem(50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem2, 1, 2, 1, 1);

        pushColorFalse = new QPushButton(groupBox_2);
        pushColorFalse->setObjectName(QStringLiteral("pushColorFalse"));
        QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(0));
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushColorFalse->sizePolicy().hasHeightForWidth());
        pushColorFalse->setSizePolicy(sizePolicy2);
        pushColorFalse->setMaximumSize(QSize(40, 16777215));

        gridLayout2->addWidget(pushColorFalse, 1, 1, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout2->addWidget(label_2, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox_2, 0, 2, 1, 2);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        spacerItem3 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem3);

        okButton = new QPushButton(TLabelBoolEditor);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout->addWidget(okButton);

        cancelButton = new QPushButton(TLabelBoolEditor);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout->addWidget(cancelButton);


        gridLayout->addLayout(hboxLayout, 2, 0, 1, 4);

        groupBox = new QGroupBox(TLabelBoolEditor);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        gridLayout3 = new QGridLayout(groupBox);
        gridLayout3->setSpacing(6);
        gridLayout3->setContentsMargins(9, 9, 9, 9);
        gridLayout3->setObjectName(QStringLiteral("gridLayout3"));
        pushColorTrue = new QPushButton(groupBox);
        pushColorTrue->setObjectName(QStringLiteral("pushColorTrue"));
        sizePolicy2.setHeightForWidth(pushColorTrue->sizePolicy().hasHeightForWidth());
        pushColorTrue->setSizePolicy(sizePolicy2);
        pushColorTrue->setMaximumSize(QSize(40, 16777215));

        gridLayout3->addWidget(pushColorTrue, 1, 1, 1, 1);

        spacerItem4 = new QSpacerItem(50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout3->addItem(spacerItem4, 1, 2, 1, 1);

        lineEditTrue = new QLineEdit(groupBox);
        lineEditTrue->setObjectName(QStringLiteral("lineEditTrue"));

        gridLayout3->addWidget(lineEditTrue, 0, 1, 1, 2);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout3->addWidget(label_3, 1, 0, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout3->addWidget(label, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 2);

        QWidget::setTabOrder(lineEditTrue, lineEditFalse);
        QWidget::setTabOrder(lineEditFalse, pushColorTrue);
        QWidget::setTabOrder(pushColorTrue, pushColorFalse);
        QWidget::setTabOrder(pushColorFalse, cancelButton);
        QWidget::setTabOrder(cancelButton, okButton);

        retranslateUi(TLabelBoolEditor);
        QObject::connect(okButton, SIGNAL(clicked()), TLabelBoolEditor, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), TLabelBoolEditor, SLOT(reject()));

        QMetaObject::connectSlotsByName(TLabelBoolEditor);
    } // setupUi

    void retranslateUi(QDialog *TLabelBoolEditor)
    {
        TLabelBoolEditor->setWindowTitle(QApplication::translate("TLabelBoolEditor", "Boolean Behaviour", Q_NULLPTR));
        groupBoxFalse->setTitle(QApplication::translate("TLabelBoolEditor", "Example", Q_NULLPTR));
        label_5->setText(QApplication::translate("TLabelBoolEditor", "True:", Q_NULLPTR));
        label_6->setText(QApplication::translate("TLabelBoolEditor", "False:", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("TLabelBoolEditor", "Displaying a False value", Q_NULLPTR));
        label_4->setText(QApplication::translate("TLabelBoolEditor", "Color:", Q_NULLPTR));
        pushColorFalse->setText(QString());
        label_2->setText(QApplication::translate("TLabelBoolEditor", "String:", Q_NULLPTR));
        okButton->setText(QApplication::translate("TLabelBoolEditor", "OK", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("TLabelBoolEditor", "Cancel", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("TLabelBoolEditor", "Displaying a True value", Q_NULLPTR));
        pushColorTrue->setText(QString());
        label_3->setText(QApplication::translate("TLabelBoolEditor", "Color:", Q_NULLPTR));
        label->setText(QApplication::translate("TLabelBoolEditor", "String:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TLabelBoolEditor: public Ui_TLabelBoolEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TLABELBOOLEDITOR_H
