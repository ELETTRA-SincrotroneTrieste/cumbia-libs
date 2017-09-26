/********************************************************************************
** Form generated from reading UI file 'editsourcetargetdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITSOURCETARGETDIALOG_H
#define UI_EDITSOURCETARGETDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EditSourceTarget
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout1;
    QGroupBox *groupBox_2;
    QVBoxLayout *vboxLayout;
    QLineEdit *lineEdit;
    QWidget *tab_2;
    QGridLayout *gridLayout2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout3;
    QToolButton *moveItemDownButton;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QLabel *textLabel;
    QLineEdit *itemTextLineEdit;
    QToolButton *moveItemUpButton;
    QHBoxLayout *hboxLayout1;
    QToolButton *newItemButton;
    QToolButton *deleteItemButton;
    QSpacerItem *spacerItem1;
    QListWidget *listWidget;

    void setupUi(QDialog *EditSourceTarget)
    {
        if (EditSourceTarget->objectName().isEmpty())
            EditSourceTarget->setObjectName(QStringLiteral("EditSourceTarget"));
        EditSourceTarget->resize(313, 300);
        gridLayout = new QGridLayout(EditSourceTarget);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        buttonBox = new QDialogButtonBox(EditSourceTarget);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);

        tabWidget = new QTabWidget(EditSourceTarget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout1 = new QGridLayout(tab);
#ifndef Q_OS_MAC
        gridLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout1->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        vboxLayout = new QVBoxLayout(groupBox_2);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        lineEdit = new QLineEdit(groupBox_2);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        vboxLayout->addWidget(lineEdit);


        gridLayout1->addWidget(groupBox_2, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout2 = new QGridLayout(tab_2);
#ifndef Q_OS_MAC
        gridLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout2->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout2->setObjectName(QStringLiteral("gridLayout2"));
        groupBox = new QGroupBox(tab_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout3 = new QGridLayout(groupBox);
#ifndef Q_OS_MAC
        gridLayout3->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout3->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout3->setObjectName(QStringLiteral("gridLayout3"));
        moveItemDownButton = new QToolButton(groupBox);
        moveItemDownButton->setObjectName(QStringLiteral("moveItemDownButton"));

        gridLayout3->addWidget(moveItemDownButton, 2, 1, 1, 1);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        textLabel = new QLabel(groupBox);
        textLabel->setObjectName(QStringLiteral("textLabel"));
        QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(5));
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textLabel->sizePolicy().hasHeightForWidth());
        textLabel->setSizePolicy(sizePolicy1);

        hboxLayout->addWidget(textLabel);

        itemTextLineEdit = new QLineEdit(groupBox);
        itemTextLineEdit->setObjectName(QStringLiteral("itemTextLineEdit"));
        QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(4));
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(itemTextLineEdit->sizePolicy().hasHeightForWidth());
        itemTextLineEdit->setSizePolicy(sizePolicy2);

        hboxLayout->addWidget(itemTextLineEdit);


        gridLayout3->addLayout(hboxLayout, 2, 0, 1, 1);

        moveItemUpButton = new QToolButton(groupBox);
        moveItemUpButton->setObjectName(QStringLiteral("moveItemUpButton"));

        gridLayout3->addWidget(moveItemUpButton, 1, 1, 1, 1);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
        newItemButton = new QToolButton(groupBox);
        newItemButton->setObjectName(QStringLiteral("newItemButton"));

        hboxLayout1->addWidget(newItemButton);

        deleteItemButton = new QToolButton(groupBox);
        deleteItemButton->setObjectName(QStringLiteral("deleteItemButton"));

        hboxLayout1->addWidget(deleteItemButton);

        spacerItem1 = new QSpacerItem(16, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);


        gridLayout3->addLayout(hboxLayout1, 1, 0, 1, 1);

        listWidget = new QListWidget(groupBox);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        gridLayout3->addWidget(listWidget, 0, 0, 1, 2);


        gridLayout2->addWidget(groupBox, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

#ifndef QT_NO_SHORTCUT
        textLabel->setBuddy(itemTextLineEdit);
#endif // QT_NO_SHORTCUT

        retranslateUi(EditSourceTarget);
        QObject::connect(buttonBox, SIGNAL(accepted()), EditSourceTarget, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), EditSourceTarget, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(EditSourceTarget);
    } // setupUi

    void retranslateUi(QDialog *EditSourceTarget)
    {
        EditSourceTarget->setWindowTitle(QApplication::translate("EditSourceTarget", "Edit Source and Target", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("EditSourceTarget", "Set Tango Source Point", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        lineEdit->setToolTip(QApplication::translate("EditSourceTarget", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Valid formats are:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for attributes: <span style=\" font-weight:600;\">tango/device/server/attribute</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for commands: <span style=\" font-weight:600;\">tango/device/server-&gt;command</span></p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent"
                        ":0px;\">You can also specify a Tango Database:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for attributes: <span style=\" font-weight:600;\">host:port/tango/device/server/attribute</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for commands: <span style=\" font-weight:600;\">host:port/tango/device/server-&gt;command</span></p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("EditSourceTarget", "Source", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("EditSourceTarget", "Set Tango Targets", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        moveItemDownButton->setToolTip(QApplication::translate("EditSourceTarget", "Move Item Down", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        moveItemDownButton->setText(QApplication::translate("EditSourceTarget", "D", Q_NULLPTR));
        textLabel->setText(QApplication::translate("EditSourceTarget", "&Text", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        itemTextLineEdit->setToolTip(QApplication::translate("EditSourceTarget", "Set Item Text", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        moveItemUpButton->setToolTip(QApplication::translate("EditSourceTarget", "Move Item Up", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        moveItemUpButton->setText(QApplication::translate("EditSourceTarget", "U", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        newItemButton->setToolTip(QApplication::translate("EditSourceTarget", "New Item", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        newItemButton->setText(QApplication::translate("EditSourceTarget", "&New Item", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        deleteItemButton->setToolTip(QApplication::translate("EditSourceTarget", "Delete Item", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        deleteItemButton->setText(QApplication::translate("EditSourceTarget", "&Delete Item", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        listWidget->setToolTip(QApplication::translate("EditSourceTarget", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Valid formats are:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for attributes: <span style=\" font-weight:600;\">tango/device/server/attribute</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for commands: <span style=\" font-weight:600;\">tango/device/server-&gt;command</span></p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent"
                        ":0px;\">You can also specify a Tango Database:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for attributes: <span style=\" font-weight:600;\">host:port/tango/device/server/attribute</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for commands: <span style=\" font-weight:600;\">host:port/tango/device/server-&gt;command</span></p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("EditSourceTarget", "Target", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EditSourceTarget: public Ui_EditSourceTarget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITSOURCETARGETDIALOG_H
