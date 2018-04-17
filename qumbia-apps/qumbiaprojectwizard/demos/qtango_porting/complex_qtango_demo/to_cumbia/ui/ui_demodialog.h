/********************************************************************************
** Form generated from reading UI file 'demodialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEMODIALOG_H
#define UI_DEMODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include "elabel.h"
#include "eled.h"
#include "qubutton.h"
#include "qucheckbox.h"
#include "qudoublespinbox.h"
#include "qulabel.h"
#include "quled.h"
#include "qulineedit.h"
#include "quplot_base.h"
#include "qutrendplot.h"

QT_BEGIN_NAMESPACE

class Ui_DemoDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *tSimpleLabel_2;
    QLabel *tSimpleLabel_3;
    QuButton *tPushButton_2;
    QLabel *label;
    QuTrendPlot *tPlotLightMarker;
    QuLabel *tLabel_2;
    QuButton *tPushButton;
    QuLineEdit *tle;
    QuDoubleSpinBox *tdsb;
    QuLabel *tLabel;
    QuLed *tLed;
    QLabel *tSimpleLabel;
    QuCheckBox *tCheckBox;

//     void setupUi(QDialog *DemoDialog) // expanded by cuuimake v.0.1
    void setupUi(QDialog *DemoDialog, CumbiaTango *cu_t, const CuTReaderFactory& cu_tango_r_fac, const CuTWriterFactory& cu_tango_w_fac)
    {
        if (DemoDialog->objectName().isEmpty())
            DemoDialog->setObjectName(QStringLiteral("DemoDialog"));
        DemoDialog->resize(529, 436);
        gridLayout = new QGridLayout(DemoDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tSimpleLabel_2 = new QLabel(DemoDialog);
        tSimpleLabel_2->setObjectName(QStringLiteral("tSimpleLabel_2"));

        gridLayout->addWidget(tSimpleLabel_2, 2, 0, 1, 1);

        tSimpleLabel_3 = new QLabel(DemoDialog);
        tSimpleLabel_3->setObjectName(QStringLiteral("tSimpleLabel_3"));

        gridLayout->addWidget(tSimpleLabel_3, 3, 0, 1, 1);

//         tPushButton_2 = new QuButton(DemoDialog); // expanded by cuuimake v.0.1
        tPushButton_2 = new QuButton(DemoDialog, cu_t, cu_tango_w_fac);
        tPushButton_2->setObjectName(QStringLiteral("tPushButton_2"));

        gridLayout->addWidget(tPushButton_2, 3, 4, 1, 1);

        label = new QLabel(DemoDialog);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 5);

//         tPlotLightMarker = new QuTrendPlot(DemoDialog); // expanded by cuuimake v.0.1
        tPlotLightMarker = new QuTrendPlot(DemoDialog, cu_t, cu_tango_r_fac);
        tPlotLightMarker->setObjectName(QStringLiteral("tPlotLightMarker"));

        gridLayout->addWidget(tPlotLightMarker, 4, 0, 1, 5);

//         tLabel_2 = new QuLabel(DemoDialog); // expanded by cuuimake v.0.1
        tLabel_2 = new QuLabel(DemoDialog, cu_t, cu_tango_r_fac);
        tLabel_2->setObjectName(QStringLiteral("tLabel_2"));

        gridLayout->addWidget(tLabel_2, 3, 1, 1, 2);

//         tPushButton = new QuButton(DemoDialog); // expanded by cuuimake v.0.1
        tPushButton = new QuButton(DemoDialog, cu_t, cu_tango_w_fac);
        tPushButton->setObjectName(QStringLiteral("tPushButton"));

        gridLayout->addWidget(tPushButton, 2, 4, 1, 1);

//         tle = new QuLineEdit(DemoDialog); // expanded by cuuimake v.0.1
        tle = new QuLineEdit(DemoDialog, cu_t, cu_tango_w_fac);
        tle->setObjectName(QStringLiteral("tle"));

        gridLayout->addWidget(tle, 3, 3, 1, 1);

//         tdsb = new QuDoubleSpinBox(DemoDialog); // expanded by cuuimake v.0.1
        tdsb = new QuDoubleSpinBox(DemoDialog, cu_t, cu_tango_w_fac);
        tdsb->setObjectName(QStringLiteral("tdsb"));

        gridLayout->addWidget(tdsb, 2, 3, 1, 1);

//         tLabel = new QuLabel(DemoDialog); // expanded by cuuimake v.0.1
        tLabel = new QuLabel(DemoDialog, cu_t, cu_tango_r_fac);
        tLabel->setObjectName(QStringLiteral("tLabel"));

        gridLayout->addWidget(tLabel, 2, 1, 1, 2);

//         tLed = new QuLed(DemoDialog); // expanded by cuuimake v.0.1
        tLed = new QuLed(DemoDialog, cu_t, cu_tango_r_fac);
        tLed->setObjectName(QStringLiteral("tLed"));

        gridLayout->addWidget(tLed, 1, 4, 1, 1);

        tSimpleLabel = new QLabel(DemoDialog);
        tSimpleLabel->setObjectName(QStringLiteral("tSimpleLabel"));

        gridLayout->addWidget(tSimpleLabel, 1, 1, 1, 2);

//         tCheckBox = new QuCheckBox(DemoDialog); // expanded by cuuimake v.0.1
        tCheckBox = new QuCheckBox(DemoDialog, cu_t, cu_tango_r_fac, cu_tango_w_fac);
        tCheckBox->setObjectName(QStringLiteral("tCheckBox"));

        gridLayout->addWidget(tCheckBox, 1, 3, 1, 1);


        retranslateUi(DemoDialog);

        QMetaObject::connectSlotsByName(DemoDialog);
    } // setupUi

    void retranslateUi(QDialog *DemoDialog)
    {
        DemoDialog->setWindowTitle(QApplication::translate("DemoDialog", "Dialog", Q_NULLPTR));
        tSimpleLabel_2->setText(QApplication::translate("DemoDialog", "double_scalar", Q_NULLPTR));
        tSimpleLabel_3->setText(QApplication::translate("DemoDialog", "string_scalar", Q_NULLPTR));
        tPushButton_2->setTarget(QApplication::translate("DemoDialog", "$1/string_scalar(&tle)", Q_NULLPTR));
        label->setText(QApplication::translate("DemoDialog", "DEMO DIALOG WITH QTango READERS/WRITERS", Q_NULLPTR));
        tPlotLightMarker->setProperty("source", QVariant(QApplication::translate("DemoDialog", "$1/double_scalar", Q_NULLPTR)));
        tLabel_2->setSource(QApplication::translate("DemoDialog", "$1/string_scalar", Q_NULLPTR));
        tPushButton->setTarget(QApplication::translate("DemoDialog", "$1/double_scalar(&tdsb)", Q_NULLPTR));
        tle->setTarget(QApplication::translate("DemoDialog", "$1/string_scalar", Q_NULLPTR));
        tdsb->setTarget(QApplication::translate("DemoDialog", "$1/double_scalar", Q_NULLPTR));
        tLabel->setSource(QApplication::translate("DemoDialog", "$1/double_scalar", Q_NULLPTR));
        tLed->setSource(QApplication::translate("DemoDialog", "$1/boolean_scalar", Q_NULLPTR));
        tSimpleLabel->setText(QApplication::translate("DemoDialog", "boolean_scalar", Q_NULLPTR));
        tCheckBox->setSource(QApplication::translate("DemoDialog", "$1/boolean_scalar", Q_NULLPTR));
        tCheckBox->setTarget(QApplication::translate("DemoDialog", "$1/boolean_scalar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DemoDialog: public Ui_DemoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEMODIALOG_H
