/********************************************************************************
** Form generated from reading UI file 'editsourcedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITSOURCEDIALOG_H
#define UI_EDITSOURCEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EditSourceDialog
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QHBoxLayout *hboxLayout;
    QLineEdit *lineEdit;
    QToolButton *toolDevice;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *EditSourceDialog)
    {
        if (EditSourceDialog->objectName().isEmpty())
            EditSourceDialog->setObjectName(QStringLiteral("EditSourceDialog"));
        EditSourceDialog->resize(219, 122);
        vboxLayout = new QVBoxLayout(EditSourceDialog);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        groupBox = new QGroupBox(EditSourceDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        hboxLayout = new QHBoxLayout(groupBox);
        hboxLayout->setSpacing(0);
        hboxLayout->setContentsMargins(6, 6, 6, 6);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        hboxLayout->addWidget(lineEdit);

        toolDevice = new QToolButton(groupBox);
        toolDevice->setObjectName(QStringLiteral("toolDevice"));

        hboxLayout->addWidget(toolDevice);


        vboxLayout->addWidget(groupBox);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem);

        okButton = new QPushButton(EditSourceDialog);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout1->addWidget(okButton);

        cancelButton = new QPushButton(EditSourceDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout1->addWidget(cancelButton);


        vboxLayout->addLayout(hboxLayout1);


        retranslateUi(EditSourceDialog);
        QObject::connect(okButton, SIGNAL(clicked()), EditSourceDialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), EditSourceDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(EditSourceDialog);
    } // setupUi

    void retranslateUi(QDialog *EditSourceDialog)
    {
        EditSourceDialog->setWindowTitle(QApplication::translate("EditSourceDialog", "Edit Source", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("EditSourceDialog", "Set Tango Source Point", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        lineEdit->setToolTip(QApplication::translate("EditSourceDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Valid formats are:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for attributes: <span style=\" font-weight:600;\">tango/device/server/attribute</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for commands: <span style=\" font-weight:600;\">tango/device/server-&gt;command</span></p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent"
                        ":0px;\">You can also specify a Tango Database:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for attributes: <span style=\" font-weight:600;\">host:port/tango/device/server/attribute</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for commands: <span style=\" font-weight:600;\">host:port/tango/device/server-&gt;command</span></p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        toolDevice->setText(QApplication::translate("EditSourceDialog", "...", Q_NULLPTR));
        okButton->setText(QApplication::translate("EditSourceDialog", "OK", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("EditSourceDialog", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EditSourceDialog: public Ui_EditSourceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITSOURCEDIALOG_H
