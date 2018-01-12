/********************************************************************************
** Form generated from reading UI file 'qumbiaprojectwizard.ui'
**
** Created by: Qt User Interface Compiler version 5.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUMBIAPROJECTWIZARD_H
#define UI_QUMBIAPROJECTWIZARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QumbiaProjectWizard
{
public:
    QGridLayout *gridLayout_4;
    QRadioButton *rbWidget;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_5;
    QLineEdit *leLaunch1;
    QLabel *label_4;
    QLineEdit *leLaunch2;
    QCheckBox *cbLaunch2;
    QCheckBox *cbLaunch1;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbCancel;
    QCheckBox *cbCloseAfterCreate;
    QPushButton *pbCreate;
    QRadioButton *rbMainWidget;
    QLabel *label_6;
    QGroupBox *gbSupport;
    QHBoxLayout *horizontalLayout_5;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *leLocation;
    QPushButton *pbChooseLocation;
    QLabel *label_3;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLineEdit *leProjectName;
    QLineEdit *leAuthor;
    QLineEdit *leAuthorEmail;
    QLineEdit *leAuthorPhone;
    QLineEdit *leAuthorOffice;
    QGroupBox *grBoxFiles;
    QGridLayout *gridLayout;
    QLabel *label_5;
    QLineEdit *leMain;
    QLineEdit *leCppFile;
    QLineEdit *leCHFile;
    QLabel *label_10;
    QLineEdit *leFormFile;
    QLineEdit *leMainWidgetName;
    QLineEdit *leProFile;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *leApplicationName;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *leOrganizationName;
    QGroupBox *groupBox_5;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *leHardwareReferent;
    QGridLayout *gridLayout_3;
    QPushButton *pbAddProperty;
    QPushButton *pbRemoveProperty;
    QTreeWidget *twProperties;

    void setupUi(QWidget *QumbiaProjectWizard)
    {
        if (QumbiaProjectWizard->objectName().isEmpty())
            QumbiaProjectWizard->setObjectName(QStringLiteral("QumbiaProjectWizard"));
        QumbiaProjectWizard->resize(686, 806);
        gridLayout_4 = new QGridLayout(QumbiaProjectWizard);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        rbWidget = new QRadioButton(QumbiaProjectWizard);
        rbWidget->setObjectName(QStringLiteral("rbWidget"));
        rbWidget->setChecked(true);

        gridLayout_4->addWidget(rbWidget, 0, 2, 1, 1);

        groupBox = new QGroupBox(QumbiaProjectWizard);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_5 = new QGridLayout(groupBox);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        leLaunch1 = new QLineEdit(groupBox);
        leLaunch1->setObjectName(QStringLiteral("leLaunch1"));
        leLaunch1->setProperty("save", QVariant(true));

        gridLayout_5->addWidget(leLaunch1, 0, 4, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_5->addWidget(label_4, 0, 1, 1, 1);

        leLaunch2 = new QLineEdit(groupBox);
        leLaunch2->setObjectName(QStringLiteral("leLaunch2"));
        leLaunch2->setProperty("save", QVariant(true));

        gridLayout_5->addWidget(leLaunch2, 1, 4, 1, 1);

        cbLaunch2 = new QCheckBox(groupBox);
        cbLaunch2->setObjectName(QStringLiteral("cbLaunch2"));
        cbLaunch2->setChecked(true);
        cbLaunch2->setProperty("save", QVariant(true));

        gridLayout_5->addWidget(cbLaunch2, 1, 0, 1, 1);

        cbLaunch1 = new QCheckBox(groupBox);
        cbLaunch1->setObjectName(QStringLiteral("cbLaunch1"));
        cbLaunch1->setChecked(true);
        cbLaunch1->setProperty("save", QVariant(true));

        gridLayout_5->addWidget(cbLaunch1, 0, 0, 1, 1);


        gridLayout_4->addWidget(groupBox, 7, 0, 1, 5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pbCancel = new QPushButton(QumbiaProjectWizard);
        pbCancel->setObjectName(QStringLiteral("pbCancel"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pbCancel->sizePolicy().hasHeightForWidth());
        pbCancel->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pbCancel);

        cbCloseAfterCreate = new QCheckBox(QumbiaProjectWizard);
        cbCloseAfterCreate->setObjectName(QStringLiteral("cbCloseAfterCreate"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(2);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(cbCloseAfterCreate->sizePolicy().hasHeightForWidth());
        cbCloseAfterCreate->setSizePolicy(sizePolicy1);
        cbCloseAfterCreate->setLayoutDirection(Qt::RightToLeft);
        cbCloseAfterCreate->setChecked(true);
        cbCloseAfterCreate->setProperty("save", QVariant(true));

        horizontalLayout->addWidget(cbCloseAfterCreate);

        pbCreate = new QPushButton(QumbiaProjectWizard);
        pbCreate->setObjectName(QStringLiteral("pbCreate"));
        sizePolicy.setHeightForWidth(pbCreate->sizePolicy().hasHeightForWidth());
        pbCreate->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pbCreate);


        gridLayout_4->addLayout(horizontalLayout, 9, 0, 1, 5);

        rbMainWidget = new QRadioButton(QumbiaProjectWizard);
        rbMainWidget->setObjectName(QStringLiteral("rbMainWidget"));

        gridLayout_4->addWidget(rbMainWidget, 1, 2, 1, 1);

        label_6 = new QLabel(QumbiaProjectWizard);
        label_6->setObjectName(QStringLiteral("label_6"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_6->setFont(font);
        label_6->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_4->addWidget(label_6, 0, 0, 2, 2);

        gbSupport = new QGroupBox(QumbiaProjectWizard);
        gbSupport->setObjectName(QStringLiteral("gbSupport"));
        horizontalLayout_5 = new QHBoxLayout(gbSupport);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));

        gridLayout_4->addWidget(gbSupport, 0, 3, 2, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label = new QLabel(QumbiaProjectWizard);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(QumbiaProjectWizard);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        leLocation = new QLineEdit(QumbiaProjectWizard);
        leLocation->setObjectName(QStringLiteral("leLocation"));
        leLocation->setProperty("save", QVariant(true));

        gridLayout_2->addWidget(leLocation, 1, 1, 1, 1);

        pbChooseLocation = new QPushButton(QumbiaProjectWizard);
        pbChooseLocation->setObjectName(QStringLiteral("pbChooseLocation"));

        gridLayout_2->addWidget(pbChooseLocation, 1, 2, 1, 1);

        label_3 = new QLabel(QumbiaProjectWizard);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_3, 2, 0, 1, 1);

        label_7 = new QLabel(QumbiaProjectWizard);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_7, 3, 0, 1, 1);

        label_8 = new QLabel(QumbiaProjectWizard);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_8, 4, 0, 1, 1);

        label_9 = new QLabel(QumbiaProjectWizard);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_9, 5, 0, 1, 1);

        leProjectName = new QLineEdit(QumbiaProjectWizard);
        leProjectName->setObjectName(QStringLiteral("leProjectName"));

        gridLayout_2->addWidget(leProjectName, 0, 1, 1, 2);

        leAuthor = new QLineEdit(QumbiaProjectWizard);
        leAuthor->setObjectName(QStringLiteral("leAuthor"));
        leAuthor->setProperty("save", QVariant(true));

        gridLayout_2->addWidget(leAuthor, 2, 1, 1, 2);

        leAuthorEmail = new QLineEdit(QumbiaProjectWizard);
        leAuthorEmail->setObjectName(QStringLiteral("leAuthorEmail"));
        leAuthorEmail->setProperty("save", QVariant(true));

        gridLayout_2->addWidget(leAuthorEmail, 3, 1, 1, 2);

        leAuthorPhone = new QLineEdit(QumbiaProjectWizard);
        leAuthorPhone->setObjectName(QStringLiteral("leAuthorPhone"));
        leAuthorPhone->setProperty("save", QVariant(true));

        gridLayout_2->addWidget(leAuthorPhone, 4, 1, 1, 2);

        leAuthorOffice = new QLineEdit(QumbiaProjectWizard);
        leAuthorOffice->setObjectName(QStringLiteral("leAuthorOffice"));
        leAuthorOffice->setProperty("save", QVariant(true));

        gridLayout_2->addWidget(leAuthorOffice, 5, 1, 1, 2);


        gridLayout_4->addLayout(gridLayout_2, 2, 0, 1, 4);

        grBoxFiles = new QGroupBox(QumbiaProjectWizard);
        grBoxFiles->setObjectName(QStringLiteral("grBoxFiles"));
        gridLayout = new QGridLayout(grBoxFiles);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_5 = new QLabel(grBoxFiles);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        leMain = new QLineEdit(grBoxFiles);
        leMain->setObjectName(QStringLiteral("leMain"));

        gridLayout->addWidget(leMain, 1, 1, 1, 1);

        leCppFile = new QLineEdit(grBoxFiles);
        leCppFile->setObjectName(QStringLiteral("leCppFile"));

        gridLayout->addWidget(leCppFile, 1, 2, 1, 1);

        leCHFile = new QLineEdit(grBoxFiles);
        leCHFile->setObjectName(QStringLiteral("leCHFile"));

        gridLayout->addWidget(leCHFile, 1, 3, 1, 1);

        label_10 = new QLabel(grBoxFiles);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_10, 0, 0, 1, 1);

        leFormFile = new QLineEdit(grBoxFiles);
        leFormFile->setObjectName(QStringLiteral("leFormFile"));

        gridLayout->addWidget(leFormFile, 1, 4, 1, 1);

        leMainWidgetName = new QLineEdit(grBoxFiles);
        leMainWidgetName->setObjectName(QStringLiteral("leMainWidgetName"));

        gridLayout->addWidget(leMainWidgetName, 0, 1, 1, 2);

        leProFile = new QLineEdit(grBoxFiles);
        leProFile->setObjectName(QStringLiteral("leProFile"));

        gridLayout->addWidget(leProFile, 0, 4, 1, 1);


        gridLayout_4->addWidget(grBoxFiles, 3, 0, 1, 4);

        groupBox_4 = new QGroupBox(QumbiaProjectWizard);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setCheckable(true);
        groupBox_4->setProperty("save", QVariant(true));
        horizontalLayout_3 = new QHBoxLayout(groupBox_4);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        leApplicationName = new QLineEdit(groupBox_4);
        leApplicationName->setObjectName(QStringLiteral("leApplicationName"));

        horizontalLayout_3->addWidget(leApplicationName);


        gridLayout_4->addWidget(groupBox_4, 4, 0, 1, 3);

        groupBox_2 = new QGroupBox(QumbiaProjectWizard);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setCheckable(true);
        groupBox_2->setProperty("save", QVariant(true));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        leOrganizationName = new QLineEdit(groupBox_2);
        leOrganizationName->setObjectName(QStringLiteral("leOrganizationName"));
        leOrganizationName->setProperty("save", QVariant(true));

        horizontalLayout_2->addWidget(leOrganizationName);


        gridLayout_4->addWidget(groupBox_2, 5, 0, 1, 3);

        groupBox_5 = new QGroupBox(QumbiaProjectWizard);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        horizontalLayout_4 = new QHBoxLayout(groupBox_5);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        leHardwareReferent = new QLineEdit(groupBox_5);
        leHardwareReferent->setObjectName(QStringLiteral("leHardwareReferent"));

        horizontalLayout_4->addWidget(leHardwareReferent);


        gridLayout_4->addWidget(groupBox_5, 6, 0, 1, 3);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        pbAddProperty = new QPushButton(QumbiaProjectWizard);
        pbAddProperty->setObjectName(QStringLiteral("pbAddProperty"));

        gridLayout_3->addWidget(pbAddProperty, 0, 1, 1, 1);

        pbRemoveProperty = new QPushButton(QumbiaProjectWizard);
        pbRemoveProperty->setObjectName(QStringLiteral("pbRemoveProperty"));

        gridLayout_3->addWidget(pbRemoveProperty, 1, 1, 1, 1);

        twProperties = new QTreeWidget(QumbiaProjectWizard);
        twProperties->setObjectName(QStringLiteral("twProperties"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(twProperties->sizePolicy().hasHeightForWidth());
        twProperties->setSizePolicy(sizePolicy2);

        gridLayout_3->addWidget(twProperties, 0, 0, 3, 1);


        gridLayout_4->addLayout(gridLayout_3, 4, 3, 3, 1);


        retranslateUi(QumbiaProjectWizard);

        QMetaObject::connectSlotsByName(QumbiaProjectWizard);
    } // setupUi

    void retranslateUi(QWidget *QumbiaProjectWizard)
    {
        QumbiaProjectWizard->setWindowTitle(QApplication::translate("QumbiaProjectWizard", "New cumbia Qt Project", Q_NULLPTR));
        rbWidget->setText(QApplication::translate("QumbiaProjectWizard", "W&idget", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("QumbiaProjectWizard", "Launch applications after files are created", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        leLaunch1->setToolTip(QApplication::translate("QumbiaProjectWizard", "<html><head/><body><p>command [args] </p><p>Special file name arguments:</p><p><span style=\" font-style:italic;\">- $PRO_FILE  </span>is replaced by the .pro file name;</p><p>- <span style=\" font-style:italic;\">$UI_FILE</span>      is replaced by the .ui form file name.</p><p><br/></p><p>For example, if the <span style=\" font-style:italic;\">.pro</span> file is <span style=\" font-style:italic;\">myproject.pro</span>,<span style=\" font-style:italic;\"/> and the command is <span style=\" font-style:italic;\"> qtcreator</span>, then <span style=\" font-style:italic;\">qtcreator myproject.pro </span>is launched after the new project files are created.</p><p>If the <span style=\" font-style:italic;\">.ui</span> file is <span style=\" font-style:italic;\">myform.ui</span>, and the command is <span style=\" font-style:italic;\">designer</span>, then <span style=\" font-style:italic;\">designer myform.ui</span> is launched.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        leLaunch1->setText(QApplication::translate("QumbiaProjectWizard", "qtcreator $PRO_FILE", Q_NULLPTR));
        label_4->setText(QString());
#ifndef QT_NO_TOOLTIP
        leLaunch2->setToolTip(QApplication::translate("QumbiaProjectWizard", "<html><head/><body><p>command [args] </p><p>Special file name arguments:</p><p><span style=\" font-style:italic;\">- $PRO_FILE </span>is replaced by the .pro file name;</p><p>- <span style=\" font-style:italic;\">$UI_FILE</span> is replaced by the .ui form file name.</p><p><br/></p><p>For example, if the <span style=\" font-style:italic;\">.pro</span> file is <span style=\" font-style:italic;\">myproject.pro</span>, and the command is <span style=\" font-style:italic;\">qtcreator</span>, then <span style=\" font-style:italic;\">qtcreator myproject.pro </span>is launched after the new project files are created.</p><p>If the <span style=\" font-style:italic;\">.ui</span> file is <span style=\" font-style:italic;\">myform.ui</span>, and the command is <span style=\" font-style:italic;\">designer</span>, then <span style=\" font-style:italic;\">designer myform.ui</span> is launched.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        leLaunch2->setText(QApplication::translate("QumbiaProjectWizard", "designer $UI_FILE", Q_NULLPTR));
        cbLaunch2->setText(QString());
        cbLaunch1->setText(QString());
        pbCancel->setText(QApplication::translate("QumbiaProjectWizard", "Cancel", Q_NULLPTR));
        cbCloseAfterCreate->setText(QApplication::translate("QumbiaProjectWizard", "Close after project creation", Q_NULLPTR));
        pbCreate->setText(QApplication::translate("QumbiaProjectWizard", "Create", Q_NULLPTR));
        rbMainWidget->setText(QApplication::translate("QumbiaProjectWizard", "&Main Window", Q_NULLPTR));
        label_6->setText(QApplication::translate("QumbiaProjectWizard", "Configure a new cumbia Qt project", Q_NULLPTR));
        gbSupport->setTitle(QApplication::translate("QumbiaProjectWizard", "Support", Q_NULLPTR));
        label->setText(QApplication::translate("QumbiaProjectWizard", "Project Name:", Q_NULLPTR));
        label_2->setText(QApplication::translate("QumbiaProjectWizard", "Location:", Q_NULLPTR));
        leLocation->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "path", Q_NULLPTR)));
        pbChooseLocation->setText(QApplication::translate("QumbiaProjectWizard", "Choose...", Q_NULLPTR));
        label_3->setText(QApplication::translate("QumbiaProjectWizard", "Author:", Q_NULLPTR));
        label_7->setText(QApplication::translate("QumbiaProjectWizard", "Author email:", Q_NULLPTR));
        label_8->setText(QApplication::translate("QumbiaProjectWizard", "Author phone:", Q_NULLPTR));
        label_9->setText(QApplication::translate("QumbiaProjectWizard", "Author office:", Q_NULLPTR));
        leProjectName->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "project_name", Q_NULLPTR)));
        leProjectName->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$PROJECT_NAME$", Q_NULLPTR)));
        leAuthor->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$AUTHOR$", Q_NULLPTR)));
        leAuthor->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "not_empty", Q_NULLPTR)));
        leAuthorEmail->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$AU_EMAIL$", Q_NULLPTR)));
        leAuthorEmail->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "email", Q_NULLPTR)));
        leAuthorPhone->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$AU_PHONE$", Q_NULLPTR)));
        leAuthorOffice->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$AU_OFFICE$", Q_NULLPTR)));
        grBoxFiles->setTitle(QApplication::translate("QumbiaProjectWizard", "Files", Q_NULLPTR));
        label_5->setText(QApplication::translate("QumbiaProjectWizard", "Files:", Q_NULLPTR));
        leMain->setText(QApplication::translate("QumbiaProjectWizard", "main.cpp", Q_NULLPTR));
        leMain->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "not_empty", Q_NULLPTR)));
        leMain->setProperty("extension", QVariant(QApplication::translate("QumbiaProjectWizard", ".cpp", Q_NULLPTR)));
#ifndef QT_NO_TOOLTIP
        leCppFile->setToolTip(QApplication::translate("QumbiaProjectWizard", "<html><head/><body><p>Widget or main window file</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        leCppFile->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$CPPFILE$", Q_NULLPTR)));
        leCppFile->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "not_empty", Q_NULLPTR)));
        leCppFile->setProperty("extension", QVariant(QApplication::translate("QumbiaProjectWizard", ".cpp", Q_NULLPTR)));
#ifndef QT_NO_TOOLTIP
        leCHFile->setToolTip(QApplication::translate("QumbiaProjectWizard", "<html><head/><body><p>Widget or main window header file</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        leCHFile->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$HFILE$", Q_NULLPTR)));
        leCHFile->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "not_empty", Q_NULLPTR)));
        leCHFile->setProperty("extension", QVariant(QApplication::translate("QumbiaProjectWizard", ".h", Q_NULLPTR)));
        label_10->setText(QApplication::translate("QumbiaProjectWizard", "Main Widget Name:", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        leFormFile->setToolTip(QApplication::translate("QumbiaProjectWizard", "<html><head/><body><p>Widget or main window designer form &quot;ui&quot; file</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        leFormFile->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$FORMFILE$", Q_NULLPTR)));
        leFormFile->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "not_empty", Q_NULLPTR)));
        leFormFile->setProperty("extension", QVariant(QApplication::translate("QumbiaProjectWizard", ".ui", Q_NULLPTR)));
        leMainWidgetName->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$MAINCLASS$", Q_NULLPTR)));
        leMainWidgetName->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "not_empty", Q_NULLPTR)));
        leProFile->setProperty("checkContents", QVariant(QApplication::translate("QumbiaProjectWizard", "not_empty", Q_NULLPTR)));
        leProFile->setProperty("extension", QVariant(QApplication::translate("QumbiaProjectWizard", ".pro", Q_NULLPTR)));
        groupBox_4->setTitle(QApplication::translate("QumbiaProjectWizard", "App&lication Name", Q_NULLPTR));
        leApplicationName->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$APPLICATION_NAME$", Q_NULLPTR)));
        groupBox_2->setTitle(QApplication::translate("QumbiaProjectWizard", "Organization &Name", Q_NULLPTR));
        leOrganizationName->setText(QApplication::translate("QumbiaProjectWizard", "Elettra", Q_NULLPTR));
        leOrganizationName->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$ORGANIZATION_NAME$", Q_NULLPTR)));
        groupBox_5->setTitle(QApplication::translate("QumbiaProjectWizard", "Hardware Referent or Referents, comma separated", Q_NULLPTR));
        leHardwareReferent->setProperty("placeholder", QVariant(QApplication::translate("QumbiaProjectWizard", "$HW_REFERENT$", Q_NULLPTR)));
        pbAddProperty->setText(QApplication::translate("QumbiaProjectWizard", "Add", Q_NULLPTR));
        pbRemoveProperty->setText(QApplication::translate("QumbiaProjectWizard", "Remove ", Q_NULLPTR));
        QTreeWidgetItem *___qtreewidgetitem = twProperties->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("QumbiaProjectWizard", "Type", Q_NULLPTR));
        ___qtreewidgetitem->setText(1, QApplication::translate("QumbiaProjectWizard", "property value", Q_NULLPTR));
        ___qtreewidgetitem->setText(0, QApplication::translate("QumbiaProjectWizard", "Main widget property name", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QumbiaProjectWizard: public Ui_QumbiaProjectWizard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUMBIAPROJECTWIZARD_H
