/********************************************************************************
** Form generated from reading UI file 'ComplexQTangoDemo.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPLEXQTANGODEMO_H
#define UI_COMPLEXQTANGODEMO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include "eapplynumeric.h"
#include "elabel.h"
#include "quapplynumeric.h"
#include "qubutton.h"
#include "qulabel.h"
#include "tdialread.h"
#include "tdialwrite.h"

QT_BEGIN_NAMESPACE

class Ui_ComplexQTangoDemo
{
public:
    QGridLayout *gridLayout_2;
    QuButton *tPushButton;
    QLabel *tSimpleLabel;
    QuLabel *tLabel_3;
    QuLabel *tLabel_2;
    QLabel *tSimpleLabel_2;
    QPushButton *pbMore;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    TDialRead *readDial;
    TDialWrite *writeDial;
    QLineEdit *leDialRead;
    QLineEdit *leDialWrite;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QSlider *horizontalSlider;
    QProgressBar *progressBar;
    QSpacerItem *horizontalSpacer;
    QuApplyNumeric *tApplyNumeric;
    QuLabel *tLabel;

//     void setupUi(QWidget *ComplexQTangoDemo) // expanded by cuuimake v.0.1
    void setupUi(QWidget *ComplexQTangoDemo, CumbiaTango *cu_t, const CuTReaderFactory& cu_tango_r_fac, const CuTWriterFactory& cu_tango_w_fac)
    {
        if (ComplexQTangoDemo->objectName().isEmpty())
            ComplexQTangoDemo->setObjectName(QStringLiteral("ComplexQTangoDemo"));
        ComplexQTangoDemo->resize(592, 548);
        gridLayout_2 = new QGridLayout(ComplexQTangoDemo);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
//         tPushButton = new QuButton(ComplexQTangoDemo); // expanded by cuuimake v.0.1
        tPushButton = new QuButton(ComplexQTangoDemo, cu_t, cu_tango_w_fac);
        tPushButton->setObjectName(QStringLiteral("tPushButton"));

        gridLayout_2->addWidget(tPushButton, 3, 4, 1, 1);

        tSimpleLabel = new QLabel(ComplexQTangoDemo);
        tSimpleLabel->setObjectName(QStringLiteral("tSimpleLabel"));

        gridLayout_2->addWidget(tSimpleLabel, 0, 0, 1, 1);

//         tLabel_3 = new QuLabel(ComplexQTangoDemo); // expanded by cuuimake v.0.1
        tLabel_3 = new QuLabel(ComplexQTangoDemo, cu_t, cu_tango_r_fac);
        tLabel_3->setObjectName(QStringLiteral("tLabel_3"));

        gridLayout_2->addWidget(tLabel_3, 3, 2, 1, 2);

//         tLabel_2 = new QuLabel(ComplexQTangoDemo); // expanded by cuuimake v.0.1
        tLabel_2 = new QuLabel(ComplexQTangoDemo, cu_t, cu_tango_r_fac);
        tLabel_2->setObjectName(QStringLiteral("tLabel_2"));

        gridLayout_2->addWidget(tLabel_2, 3, 1, 1, 1);

        tSimpleLabel_2 = new QLabel(ComplexQTangoDemo);
        tSimpleLabel_2->setObjectName(QStringLiteral("tSimpleLabel_2"));

        gridLayout_2->addWidget(tSimpleLabel_2, 3, 0, 1, 1);

        pbMore = new QPushButton(ComplexQTangoDemo);
        pbMore->setObjectName(QStringLiteral("pbMore"));

        gridLayout_2->addWidget(pbMore, 3, 5, 1, 1);

        groupBox = new QGroupBox(ComplexQTangoDemo);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
//         readDial = new TDialRead(groupBox); // expanded by cuuimake v.0.1
        readDial = new TDialRead(groupBox, cu_t, cu_tango_r_fac);
        readDial->setObjectName(QStringLiteral("readDial"));
        readDial->setWrapping(false);
        readDial->setNotchesVisible(true);

        gridLayout->addWidget(readDial, 0, 0, 1, 1);

//         writeDial = new TDialWrite(groupBox); // expanded by cuuimake v.0.1
        writeDial = new TDialWrite(groupBox, cu_t, cu_tango_w_fac);
        writeDial->setObjectName(QStringLiteral("writeDial"));
        writeDial->setWrapping(false);
        writeDial->setNotchesVisible(true);

        gridLayout->addWidget(writeDial, 0, 1, 1, 1);

        leDialRead = new QLineEdit(groupBox);
        leDialRead->setObjectName(QStringLiteral("leDialRead"));

        gridLayout->addWidget(leDialRead, 1, 0, 1, 1);

        leDialWrite = new QLineEdit(groupBox);
        leDialWrite->setObjectName(QStringLiteral("leDialWrite"));

        gridLayout->addWidget(leDialWrite, 1, 1, 1, 1);


        gridLayout_2->addWidget(groupBox, 2, 0, 1, 6);

        groupBox_2 = new QGroupBox(ComplexQTangoDemo);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        horizontalSlider = new QSlider(groupBox_2);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setOrientation(Qt::Horizontal);

        gridLayout_3->addWidget(horizontalSlider, 1, 0, 1, 1);

        progressBar = new QProgressBar(groupBox_2);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(24);

        gridLayout_3->addWidget(progressBar, 0, 0, 1, 1);


        gridLayout_2->addWidget(groupBox_2, 1, 0, 1, 6);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 5, 1, 1);

//         tApplyNumeric = new QuApplyNumeric(ComplexQTangoDemo); // expanded by cuuimake v.0.1
        tApplyNumeric = new QuApplyNumeric(ComplexQTangoDemo, cu_t, cu_tango_w_fac);
        tApplyNumeric->setObjectName(QStringLiteral("tApplyNumeric"));
        tApplyNumeric->setMaximumSize(QSize(140, 46));

        gridLayout_2->addWidget(tApplyNumeric, 0, 4, 1, 1);

//         tLabel = new QuLabel(ComplexQTangoDemo); // expanded by cuuimake v.0.1
        tLabel = new QuLabel(ComplexQTangoDemo, cu_t, cu_tango_r_fac);
        tLabel->setObjectName(QStringLiteral("tLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tLabel->sizePolicy().hasHeightForWidth());
        tLabel->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(tLabel, 0, 1, 1, 2);


        retranslateUi(ComplexQTangoDemo);

        QMetaObject::connectSlotsByName(ComplexQTangoDemo);
    } // setupUi

    void retranslateUi(QWidget *ComplexQTangoDemo)
    {
        ComplexQTangoDemo->setWindowTitle(QApplication::translate("ComplexQTangoDemo", "ComplexQTangoDemo", Q_NULLPTR));
        tPushButton->setText(QApplication::translate("ComplexQTangoDemo", "Switch States", Q_NULLPTR));
        tPushButton->setTarget(QApplication::translate("ComplexQTangoDemo", "$1->SwitchStates", Q_NULLPTR));
        tSimpleLabel->setText(QApplication::translate("ComplexQTangoDemo", "double_scalar", Q_NULLPTR));
        tLabel_3->setSource(QApplication::translate("ComplexQTangoDemo", "$1->Status", Q_NULLPTR));
        tLabel_2->setSource(QApplication::translate("ComplexQTangoDemo", "$1->State", Q_NULLPTR));
        tSimpleLabel_2->setText(QApplication::translate("ComplexQTangoDemo", "State:", Q_NULLPTR));
        pbMore->setText(QApplication::translate("ComplexQTangoDemo", "More...", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("ComplexQTangoDemo", "Inheriting QTangoComProxyReader / QTangoComProxyWriter: double_scalar", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        writeDial->setToolTip(QApplication::translate("ComplexQTangoDemo", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">This dial will change the <span style=\" font-weight:600;\">short_scalar</span> on the left</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        groupBox_2->setTitle(QApplication::translate("ComplexQTangoDemo", "Using QuWatcher / QTWriter: double_scalar", Q_NULLPTR));
        tApplyNumeric->setTarget(QApplication::translate("ComplexQTangoDemo", "$1/double_scalar", Q_NULLPTR));
        tLabel->setSource(QApplication::translate("ComplexQTangoDemo", "$1/double_scalar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ComplexQTangoDemo: public Ui_ComplexQTangoDemo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPLEXQTANGODEMO_H
