#include "qumbiaprojectwizard.h"
#include "ui_qumbiaprojectwizard.h"

#include <QSettings>
#include <QMessageBox>
#include <QTimer>
#include <QDir>
#include <QFileDialog>
#include <QCheckBox>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QtDebug>
#include <QDomDocument>
#include <QDomNode>
#include <QItemDelegate>
#include <QComboBox>
#include <QProcess>
#include <math.h>

#include "src/qtango/conversiondialog.h"
#include "src/qtango/qtangoimport.h"
#include "projectbackup.h"

MyFileInfo::MyFileInfo(const QString &templateFileNam, const QString &newFileNam, const QString &subdirnam)
{
    templateFileName = templateFileNam;
    newFileName = newFileNam;
    subDirName = subdirnam;
}

QumbiaProjectWizard::QumbiaProjectWizard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QumbiaProjectWizard)
{
    ui->setupUi(this);
    m_qtangoImport = NULL;
    m_rbFactorySaveChecked = NULL;
    qApp->setApplicationName("QumbiaProjectWizard");
    QTimer::singleShot(200, this, SLOT(init()));
    m_usage();
}

QumbiaProjectWizard::~QumbiaProjectWizard()
{
    delete ui;
}

void QumbiaProjectWizard::m_usage() {
    printf("\n================================================================================================\n"
           "OPTIONS\n"
           "\e[0;32m--from-qtango\e[0m start the application in the \"import project from qtango\" mode\n"
           "\e[0;32m--from-qtango --in-current-wdir\e[0m search for \e[1;36m.pro\e[0m files and output in the current directory\n"
           "\e[0;32m--from-qtango --fast\e[0m (implies --in-current-wdir) same as previous option, minimise user interaction (only confirm operation)"
           "================================================================================================\n\n");
}

void QumbiaProjectWizard::init()
{
    QSettings s;
    if(s.allKeys().isEmpty())
    {
        QMessageBox::information(this, "Information",
                                 "This seems to be the first time you use the QTango Project Wizard.\n"
                                 "Fill in the form to create your first project. This application will\n"
                                 "remember many of your personal settings.");
    }

    QHBoxLayout *hblo = qobject_cast<QHBoxLayout *>(ui->gbSupport->layout());
    foreach(QString f, findSupportedFactories())
    {
        QRadioButton *rb = new QRadioButton(ui->gbSupport);
        rb->setObjectName("rb-" + f);
        rb->setProperty("factory", f);
        rb->setText(f);
        connect(rb, SIGNAL(toggled(bool)), this, SLOT(setFactory(bool)));
        if(rb->objectName() == "rb-tango")
            rb->setChecked(true);
        if(hblo)
            hblo->addWidget(rb);
    }

    connect(ui->cbLaunch1, SIGNAL(toggled(bool)), ui->leLaunch1, SLOT(setEnabled(bool)));
    connect(ui->cbLaunch2, SIGNAL(toggled(bool)), ui->leLaunch2, SLOT(setEnabled(bool)));

    connect(ui->pbCreate, SIGNAL(clicked()), this, SLOT(create()));
    connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->pbChooseLocation, SIGNAL(clicked()), this, SLOT(selectLocation()));
    connect(ui->pbAddProperty, SIGNAL(clicked()), this, SLOT(addProperty()));
    connect(ui->pbRemoveProperty, SIGNAL(clicked()), this, SLOT(removeProperty()));
    connect(ui->pbImport, SIGNAL(clicked()), this, SLOT(qtangoImport()));
    connect(ui->rbImport, SIGNAL(toggled(bool)), this, SLOT(importRbToggled(bool)));
    connect(ui->rbQml, SIGNAL(toggled(bool)), this, SLOT(qmlToggled(bool)));
    connect(ui->pbAddAndroidLib, SIGNAL(clicked()), this, SLOT(addAndroidLib()));
    connect(ui->pbRemoveAndroidLib, SIGNAL(clicked()), this, SLOT(removeAndroidLib()));



    // default: widget: hide qml related elements in the gui
    qmlToggled(false);

    connect(ui->leProjectName, SIGNAL(textChanged(QString)), this, SLOT(projectNameChanged(QString)));
    foreach(QLineEdit *le, findChildren<QLineEdit *>())
    {
        if(le->property("save").isValid() && le->property("save").toBool()
                && s.contains(le->objectName()))
            le->setText(s.value(le->objectName()).toString());
        if(le->property("checkContents").isValid())
            connect(le, SIGNAL(textChanged(QString)), this, SLOT(checkText(QString)));
        /* initialize to invalid the empty line edits with property checkContents to not_empty */
        QStringList nonempty  = QStringList() << "not_empty" << "path" << "project_name";
        if(le->property("checkContents").isValid() &&  nonempty.contains(le->property("checkContents").toString()))
            setValid(le, !le->text().isEmpty() );
    }
    foreach(QRadioButton *r, findChildren<QRadioButton *>())
    {
        if(r->property("save").isValid())
            r->setChecked(s.value(r->objectName()).toBool());
    }
    foreach(QCheckBox *cb, findChildren<QCheckBox *>())
    {
        if(cb->property("save").isValid())
            cb->setChecked(s.value(cb->objectName()).toBool());
    }

    /* finally, group boxes */
    foreach(QGroupBox *gb, findChildren<QGroupBox *>())
    {
        if(gb->property("save").isValid())
            gb->setChecked(s.value(gb->objectName()).toBool());
    }

    QStringList treeItems = s.value("treeItems").toStringList();
    foreach(QString ti, treeItems)
        addProperty(ti.split(";;"));
    checkValidity();

    if(qApp->arguments().contains("--from-qtango")) {
        bool fast = qApp->arguments().contains("--in-current-wdir") || qApp->arguments().contains("--fast");
        ui->rbImport->setChecked(true);
        bool success = qtangoImport(qApp->arguments().contains("--in-current-wdir") || fast);
        if(success && fast)
            create();
    }
    else
        importRbToggled(false);

    // android libs
    initAndroidLibs();
    ui->gbAndroidLibs->setChecked(s.value("ANDROID_LIBS_ENABLED", false).toBool());
}

void QumbiaProjectWizard::convertStart()
{
    if(!m_qtangoImport->convert())
        QMessageBox::critical(this, "QTango to cumbia conversion error",
                              QString("Conversion error: " + m_qtangoImport->errorMessage()));
}

void QumbiaProjectWizard::create()
{
    QString location = ui->leLocation->text();
    QString project_name = ui->leProjectName->text();
    if(!location.endsWith("/"))
        location += "/";
    QString project_path = location + project_name;

    if(ui->rbImport->isChecked()) {
        project_path = ui->leLocation->text();
        bool overwrite = qApp->arguments().contains("--in-current-wdir") || qApp->arguments().contains("--fast");
        ConversionDialog *cd = new ConversionDialog(this, project_path, overwrite);
        cd->setObjectName("conversionDialog");
        connect(cd, SIGNAL(okClicked()), this, SLOT(conversionDialogOkClicked()));
        connect(m_qtangoImport, SIGNAL(newLog(const QList<OpQuality> &)), cd, SLOT(addLogs(const QList<OpQuality>&)));
        connect(m_qtangoImport, SIGNAL(conversionFinished(bool)), cd, SLOT(conversionFinished(bool)));
        connect(m_qtangoImport, SIGNAL(outputFileWritten(QString,QString,bool)), cd, SLOT(outputFileWritten(QString,QString, bool)));
        cd->show();
        QTimer::singleShot(200, this, SLOT(convertStart()));

        return;
    }

    QString pro_file = ui->leProFile->text();
    QString maincpp = ui->leMain->text();
    QString cpp = ui->leCppFile->text();
    QString h = ui->leCHFile->text();
    QString form;

    ui->rbQml->isChecked() ? form = ui->leMainQml->text() : form = ui->leFormFile->text();

    if(maincpp.isEmpty() || cpp.isEmpty() || h.isEmpty() || form.isEmpty() || pro_file.isEmpty()
            || location.isEmpty() || project_name.isEmpty())
    {
        QMessageBox::critical(this, "Fields empty", "Some mandatory fields have been left empty");
        return;
    }

    m_saveUISettings();

    QDir newDir(project_path);
    if(newDir.exists() && !newDir.entryList().isEmpty())
    {
        QMessageBox::StandardButton  proceed = QMessageBox::question(this, "Directory exists", "The directory \n\""
                                                                     + newDir.path() + "\"\nAlready exists and is not empty.\n"
                                                                                       "Do you want to create the new files there?",
                                                                     QMessageBox::Yes|QMessageBox::Cancel);
        if(proceed != QMessageBox::Yes)
            return;
    }

    if(newDir.mkpath(project_path)) /* returns true if dir already exists */
    {
        QString wtype, subdirname, outdirname, outfilename;
        QDir subdir;
        bool subdirOk = true;
        if(ui->rbWidget->isChecked())
            wtype = "widget";
        else if(ui->rbQml->isChecked())
            wtype = "qml";
        ui->rbWidget->isChecked() ? wtype = "widget" : wtype = "mainwindow";
        QString template_path = TEMPLATES_PATH;
        if(!template_path.endsWith('/'))
            template_path += '/';

        QList<MyFileInfo> filesInfo;

        MyFileInfo readmei("README", "README", "");
        MyFileInfo changelogi("CHANGELOG", "CHANGELOG", "");
        MyFileInfo proi("qumbiaproject-" + m_selectedFactory + ".pro", pro_file, "");
        MyFileInfo maini("main-" + m_selectedFactory + ".cpp", maincpp, "src");
        MyFileInfo cppi(wtype + "-" + m_selectedFactory + ".cpp", cpp, "src");
        MyFileInfo hi (wtype + "-" + m_selectedFactory +  ".h", h, "src");
        MyFileInfo formi = MyFileInfo(wtype + ".ui", form, "src");
        if(ui->rbQml->isChecked()) {
            formi.templateFileName = "main.qml";
            maini.templateFileName = "mainqml-" + m_selectedFactory + ".cpp";
            MyFileInfo qrcfi("qml.qrc", "qml.qrc", "src");
            filesInfo << proi << maini << formi << qrcfi << readmei << changelogi;
        }
        else {
            filesInfo << proi << maini << cppi << hi << formi << readmei << changelogi;
        }



        newDir.setPath(project_path);

        foreach(MyFileInfo fi, filesInfo)
        {
            QString fname = fi.templateFileName;
            QString placeh_lower;
            QFile f(template_path + fname);
            qDebug() << __FUNCTION__ << "reading from file template " << fname;
            QString contents;
            if(f.open(QIODevice::ReadOnly|QIODevice::Text))

            {
                contents = QString(f.readAll());
                contents.replace("$INCLUDE_DIR$", INCLUDE_PATH);

                foreach(QLineEdit *le, findChildren<QLineEdit *>())
                {
                    if(le->isEnabled() && !le->text().isEmpty())
                    {
                        QVariant placeholder = le->property("placeholder");
                        if(placeholder.isValid())
                        {
                            contents.replace(placeholder.toString(), le->text());
                            /* look for _LOWERCASE in contents */
                            placeh_lower = placeholder.toString();
                            placeh_lower.remove(placeh_lower.length() - 1, 1) + "_LOWERCASE$";
                            contents.replace(placeh_lower, le->text().toLower());
                        }
                    }

                }

                f.close(); /* done with the file */

                /* replace unmatched place holders with "-" */
                //     contents.replace(QRegExp("\\$[A-Za-z_0-9]*\\$"), "-");

                if(fname.endsWith(".ui"))
                    addProperties(contents);
                if(fname.endsWith(".pro")) {
                    QRegExp re("contains\\(ANDROID_TARGET_ARCH,armeabi-v7a\\)\\s*\\{.*\\}");
                    if(contents.contains(re)) {
                        contents.replace(re, QString("contains(ANDROID_TARGET_ARCH,armeabi-v7a)"
                            " {\n\tANDROID_EXTRA_LIBS = \\\n%1}").arg(m_formatAndroidLibs()));
                    }
                    // $UI_FORMFILE_H$ in the section HEADERS += adds the ui_*.h file
                    // to the HEADERS in oreder to ensure that a changed ui file is rebuilt after
                    // modification
                    QString form_base(form);
                    if(form_base.endsWith(".ui"))
                        form_base.remove(".ui");
                    contents.replace("$UI_FORMFILE_H$", "ui_" + form_base + ".h");
                }

                subdirname = fi.subDirName;
                outdirname = newDir.absolutePath() + "/" + subdirname;
                qDebug() << __FUNCTION__ << " out dir name " << outdirname;
                if(!outdirname.endsWith('/'))
                    outdirname += "/";
                outfilename = outdirname + fi.newFileName;

                if(!subdirname.isEmpty())
                    subdir = QDir(newDir.absolutePath() + "/" + subdirname);
                if(!subdir.exists())
                    subdirOk = newDir.mkdir(subdirname);
                if(!subdirOk) {
                    QMessageBox::critical(this, "Error creating directory", "Error creating subdirectory \'" + subdirname + "\"\n"
                           "under \"" + newDir.path() + "\"");
                }
                else
                {
                    QFile out(outfilename, this);
                    if(!out.open(QIODevice::WriteOnly|QIODevice::Text))
                    {
                        QMessageBox::critical(this, "Failed to open file for writing",
                                              QString("Failed to open \"%1\" for writing:\n%2"
                                                      "\nFile generation will be incomplete.").arg(outfilename).arg(out.errorString()));
                        break;
                    }
                    else
                    {
                        QTextStream fo(&out);
                        fo << contents;
                        f.close();
                    }
                }
            }
            else
            {
                QMessageBox::critical(this, "Failed to create file",
                                      "Error reading template file \"" + f.fileName() +
                                      "\"\nProject creation will be incomplete!\n"
                                      "The error was: \"" + f.errorString() + "\"");
                break;
            }
        }

        m_launchApps(project_path);

        if(ui->cbCloseAfterCreate->isChecked())
            close();
    }
    else
        QMessageBox::critical(this, "Failed to create directory",
                              QString("Failed to create directory \n\"%1\"").arg(newDir.path()));
}

void QumbiaProjectWizard::checkText(const QString &txt)
{
    bool valid = true;
    QString checkType = sender()->property("checkContents").toString();
    if(checkType == "project_name")
        valid = !txt.isEmpty() && !txt.contains(QRegExp("\\s+"));
    else if(checkType == "not_empty")
        valid = !txt.isEmpty();
    else if(checkType == "email")
        valid = txt.isEmpty() || txt.count("@") == 1;
    else if(checkType == "path")
    {
        QDir d(txt);
        valid = d.exists();
    }

    setValid(qobject_cast<QWidget *>(sender()), valid);

    checkValidity();
}

void QumbiaProjectWizard::selectLocation()
{
    QString startDirName = ui->leLocation->text();
    QDir startDir(startDirName);
    if(!startDir.exists())
        startDirName = QDir::homePath();
    QString path = QFileDialog::getExistingDirectory(this, "Choose Directory", startDirName,
                                                     QFileDialog::ShowDirsOnly);
    if(!path.isEmpty())
        ui->leLocation->setText(path);

}

void QumbiaProjectWizard::addProperty(const QStringList parts)
{
    Q_ASSERT(parts.size() == 3);
    QTreeWidgetItem *it = new QTreeWidgetItem(ui->twProperties, QStringList() << parts.at(0) << parts.at(1) << parts.at(2));
    it->setFlags(it->flags() | Qt::ItemIsEditable);
    QComboBox *propDataTypeCombo = new QComboBox(ui->twProperties);
    propDataTypeCombo->addItems(QStringList() << "bool" << "string" << "int" << "float" << "double");
    propDataTypeCombo->setCurrentIndex(propDataTypeCombo->findText(parts.at(2)));
    ui->twProperties->setItemWidget(it, 2, propDataTypeCombo);
    ui->twProperties->editItem(it, 0);
}

void QumbiaProjectWizard::removeProperty()
{
    foreach(QTreeWidgetItem *it, ui->twProperties->selectedItems())
        delete it;
}

void QumbiaProjectWizard::projectNameChanged(const QString &p)
{
    QList<QLineEdit *> les = ui->grBoxFiles->findChildren<QLineEdit *>();
    /* add application name */
    les << ui->leApplicationName;
    foreach(QLineEdit *le, les)
    {
        QString s = le->text();
        QString p1(p), s1(s);
        QString extension = le->property("extension").toString();
        s1.remove(extension);

        if(fabs(s1.length() - p.length()) < 2)
        {
            p1.truncate(qMin(s1.length(), p.length()));
            s1.truncate(qMin(s1.length(), p.length()));
            if(s1.compare(p1, Qt::CaseInsensitive) == 0)
                s = p + extension;
            if(s.length() > 0 && extension.isEmpty())
                s[0] = s[0].toUpper();

            le->setText(s);
        }
    }

}

void QumbiaProjectWizard::setFactory(bool rbchecked)
{
    if(rbchecked)
        m_selectedFactory = sender()->property("factory").toString();
}

/*! \brief set up tango import view.
 *
 * @param from_cwd true conversion output will be produced in the same directory as the qtango project
 */
bool QumbiaProjectWizard::qtangoImport(bool from_cwd)
{
    bool ok;
    QSettings s;
    QString pro_f;
    QFileInfoList pro_list = QDir::current().entryInfoList(QStringList() << "*.pro", QDir::Files);
    if(pro_list.size() == 1 && from_cwd) {
        pro_f = pro_list.first().fileName();
        ui->leLocation->setText(QDir::currentPath());
    }
    else {
        QString lastProjectDirnam = s.value("LAST_PROJECT_DIRNAM", QDir::homePath()).toString();
        pro_f = QFileDialog::getOpenFileName(this,
                                                 "Select a QTango project file [*.pro]",
                                                 lastProjectDirnam, "*.pro");
        QFileInfo fii(pro_f);
        ui->leLocation->setText(fii.absolutePath());
    }
    if(!pro_f.isEmpty())
    {
        if(m_qtangoImport)
            delete m_qtangoImport;
        m_qtangoImport = new QTangoImport();
        ok = m_qtangoImport->open(pro_f);
        if(ok) {
            m_setAppProps(m_qtangoImport->getAppProps());
            m_setProjectFiles(m_qtangoImport->getProjectFiles());
            ui->leMainWidgetName->setText(m_qtangoImport->mainWidgetClassName());
            ui->leProjectName->setText(m_qtangoImport->projectName());
            ui->leUiFormClassName->setText(m_qtangoImport->uiFormClassName());
            ui->pbCreate->setText("Convert...");
        }
        if(m_qtangoImport->error()) {
            QMessageBox::critical(this, "Error converting qtango project", "Error converting the qtango project:\n"
                                  + m_qtangoImport->errorMessage());
        }
        else {
        }
        QFileInfo fi(pro_f);
        s.setValue("LAST_PROJECT_DIRNAM", fi.absoluteDir().absolutePath());

    }
    return ok;
}

void QumbiaProjectWizard::checkValidity()
{
    bool valid = true;
    foreach(QLineEdit *le, findChildren<QLineEdit *>())
    {
        if(le->property("valid").isValid() && le->property("valid").toBool() == false)
            valid = false;
        if(!valid)
            break;
    }
    ui->pbCreate->setEnabled(valid);
}

void QumbiaProjectWizard::setValid(QWidget *w, bool valid)
{
    /* take the palette from a line edit whose palette is never changed */
    QPalette p = ui->leAuthorPhone->palette();
    if(!valid)
        p.setColor(QPalette::Text, QColor(Qt::red));
    w->setPalette(p);
    w->setProperty("valid", valid);
}

void QumbiaProjectWizard::loadSettings()
{

}

void QumbiaProjectWizard::addProperties(QString &uixml)
{
    if(ui->twProperties->model()->rowCount() > 0)
    {
        QDomDocument doc("uixml");
        if(doc.setContent(uixml))
        {
            QDomElement widget;
            QDomElement uiel = doc.firstChildElement("ui");
            if(!uiel.isNull())
                widget = uiel.firstChildElement("widget");
            if(!widget.isNull())
            {
                foreach(QTreeWidgetItem *it, ui->twProperties->findItems("*", Qt::MatchWildcard))
                {
                    QString pName = it->text(0);
                    QString pVal  = it->text(1);
                    QString type = qobject_cast<QComboBox *>(ui->twProperties->itemWidget(it, 2))->currentText();
                    if(pName != "-" && pVal != "-")
                    {
                        QDomElement propEl = doc.createElement("property");
                        propEl.setAttribute("name", pName);
                        propEl.setAttribute("stdset", "0");
                        widget.appendChild(propEl);
                        QDomElement valElement = doc.createElement(type);
                        valElement.appendChild(doc.createTextNode(pVal));
                        propEl.appendChild(valElement);
                    }
                }
            }
            else
                qDebug() << __FUNCTION__ << "widget el is null!!";
            uixml = doc.toString();
            qDebug() << __FUNCTION__ << uixml;
        }
        else
        {
            QMessageBox::critical(this, "Error parsing ui xml file!", "Error parsing ui file!");
        }
    }
}

void QumbiaProjectWizard::importRbToggled(bool t)
{
    t ? ui->pbCreate->setText("Convert...") : ui->pbCreate->setText("Create");
    ui->leLocation->setEnabled(!t);
    ui->pbImport->setVisible(t);
}

void QumbiaProjectWizard::conversionDialogOkClicked()
{
    ConversionDialog *cd = findChild<ConversionDialog *>();
    // will connect if the conversion is successful
    disconnect(cd, SIGNAL(finished(int)));

    bool proceed = false;
    ConversionDialog *cdlg = findChild<ConversionDialog *>();
    QString project_path = cdlg->outputPath();
    ProjectBackup::Result res;
    proceed = QDir(project_path) != QDir(m_qtangoImport->projectDir());
    if(!proceed) {
        ProjectBackup backup(project_path);
        res = backup.popup_message();
        if(res == ProjectBackup::NeedsBackup)
            proceed = backup.save("qtango_backup") == ProjectBackup::BackedUp;
        proceed = (res != ProjectBackup::Cancel);
    }
    if(proceed) {
        QList<QString> convfiles = m_qtangoImport->convertedFileList();
        bool err = !m_qtangoImport->findFilesRelPath();
        if(!err)
        {
            for(int i = 0; i < convfiles.size() && !err; i++) {
                QString f = convfiles.at(i);
                err = !m_qtangoImport->outputFile(f, project_path);
            }
        }
        if(err) {
            QMessageBox::critical(this, "Error writing output files",
                                  "Error writing output files: " + m_qtangoImport->errorMessage());
        }
        else // conversion successful, when dialog is closed onConversionDialogFinished is invoked
            connect(cd, SIGNAL(finished(int)), this, SLOT(onConversionDialogFinished()));
    }
}

void QumbiaProjectWizard::onConversionDialogFinished()
{
    ConversionDialog *cdlg = findChild<ConversionDialog *>();
    QString project_path = cdlg->outputPath();
    ui->leLocation->setText(project_path);
    m_launchApps(project_path);
}

void QumbiaProjectWizard::qmlToggled(bool t)
{
    foreach(QWidget *w, ui->grBoxFiles->findChildren<QWidget *>())
        w->setVisible(!t);

    ui->cbLaunch2->setVisible(!t);
    ui->leLaunch2->setVisible(!t);
    ui->leMainQml->setVisible(t);
    ui->twProperties->setVisible(!t);
    ui->pbRemoveProperty->setVisible(!t);
    ui->pbAddProperty->setVisible(!t);
    ui->gbAndroidLibs->setVisible(t);

    if(t) { // save checked factory rb
        foreach(QRadioButton *rb, ui->gbSupport->findChildren<QRadioButton *>()) {
            if(rb->isChecked())
                m_rbFactorySaveChecked = rb;
        }
    }
    else if(m_rbFactorySaveChecked != NULL) {
        m_rbFactorySaveChecked->setChecked(true);
    }

    foreach(QRadioButton *rb, ui->gbSupport->findChildren<QRadioButton *>()) {
        rb->setVisible((!ui->rbQml->isChecked() && rb->text() != "websocket") || (rb->text() == "tango-epics" )
                       || (ui->rbQml->isChecked() && rb->text() == "websocket"));
    }
    QRadioButton *rbte = ui->gbSupport->findChild<QRadioButton *>("rb-tango-epics");
    QRadioButton *rbws = ui->gbSupport->findChild<QRadioButton *>("rb-websocket");
    if(rbte && !rbte->isChecked() && ui->rbQml->isChecked() && rbws)
        rbws->setChecked(true);



    if(t) { // show main.cpp, main.qml
        ui->leProFile->setVisible(true);
        ui->leMain->setVisible(true);
        ui->leMainQml->setVisible(true);
    }
}

void QumbiaProjectWizard::addAndroidLib()
{
    QSettings s;
    QString dire = s.value("LAST_ANDROID_LIB_SEARCH_DIR", QDir::homePath()).toString();
    QFileDialog fd;
    QStringList libs = fd.getOpenFileNames(this, "Select a library file", dire, "Library files (*.so)");
    foreach(QString lib, libs) {
        QFileInfo fi(lib);
        if(fi.exists()) {
            QString path = fi.absolutePath();
            s.setValue("LAST_ANDROID_LIB_SEARCH_DIR", path);
            new QTreeWidgetItem(ui->twAndroidLibs, QStringList() << lib);
        }

    }
}

void QumbiaProjectWizard::removeAndroidLib()
{
    QList<QTreeWidgetItem *>selectedItems = ui->twAndroidLibs->selectedItems();
    foreach(QTreeWidgetItem *it, selectedItems)
        delete it;
}

void QumbiaProjectWizard::initAndroidLibs()
{
    QSettings s;
    QStringList al = s.value("ANDROID_LIBS", QStringList()).toStringList();
    foreach (QString s, al) {
        new QTreeWidgetItem(ui->twAndroidLibs, QStringList() << s);
    }
}

QString QumbiaProjectWizard::m_formatAndroidLibs() const {
    QString s;
    int cnt = ui->twAndroidLibs->topLevelItemCount();
    for(int i = 0; i < cnt; i++) {
        s += "\t" + ui->twAndroidLibs->topLevelItem(i)->text(0);
        if(i < cnt - 1)
            s += " \\";
        s += "\n";
    }
    return s;
}

QStringList QumbiaProjectWizard::findSupportedFactories()
{
    QStringList factories;
    QDir d(TEMPLATES_PATH);
    QStringList files = d.entryList(QStringList() << "*.pro", QDir::Files);
    foreach(QString f, files)
    {
        if(f.contains("-") && !f.contains("qml"))
            factories << f.remove("qumbiaproject-").remove(".pro");
    }
    qDebug() << __FUNCTION__ << "Factories detected " << factories;
    return factories;
}

void QumbiaProjectWizard::m_setAppProps(const QMap<QString, QString> &props)
{
    QList<QLineEdit *> les = findChildren<QLineEdit *>();
    foreach(QString content, props.keys()) {
        foreach(QLineEdit *le, les) {
            if(le->property("content").toString() == content)
                le->setText(props[content]);
        }

    }
}

void QumbiaProjectWizard::m_setProjectFiles(const QMap<QString, QString> &props)
{
    if(props.contains("hfile")) ui->leCHFile->setText(props["hfile"]);
    if(props.contains("cppfile")) ui->leCppFile->setText(props["cppfile"]);
    if(props.contains("uifile")) ui->leFormFile->setText(props["uifile"]);
    if(props.contains("pro")) ui->leProFile->setText(props["pro"]);
}

void QumbiaProjectWizard::m_saveUISettings()
{
    QSettings s;
    foreach(QLineEdit *le, findChildren<QLineEdit *>())
    {
        if(le->property("save").isValid())
            s.setValue(le->objectName(), le->text());
    }
    foreach(QRadioButton *r, findChildren<QRadioButton *>())
    {
        if(r->property("save").isValid())
            s.setValue(r->objectName(), r->isChecked());
    }
    foreach(QCheckBox *cb, findChildren<QCheckBox *>())
    {
        if(cb->property("save").isValid())
            s.setValue(cb->objectName(), cb->isChecked());
    }

    /* finally, group boxes */
    foreach(QGroupBox *gb, findChildren<QGroupBox *>())
    {
        if(gb->property("save").isValid())
            s.setValue(gb->objectName(), gb->isChecked());
    }

    QStringList treeItems;
    foreach(QTreeWidgetItem *it, ui->twProperties->findItems("*", Qt::MatchWildcard))
        if(it->text(0) != "-" && it->text(1) != "-")
            treeItems << it->text(0) + ";;" + it->text(1)  + ";;" +
                         qobject_cast<QComboBox *>(ui->twProperties->itemWidget(it, 2))->currentText();
    s.setValue("treeItems" , treeItems);

    QStringList androidLibs;
    QList<QTreeWidgetItem *>selectedItems = ui->twAndroidLibs->selectedItems();
    for(int i = 0; i < ui->twAndroidLibs->topLevelItemCount(); i++)
        androidLibs << ui->twAndroidLibs->topLevelItem(i)->text(0);
    s.setValue("ANDROID_LIBS", androidLibs);
    s.setValue("ANDROID_LIBS_ENABLED", ui->gbAndroidLibs->isChecked());
}

void QumbiaProjectWizard::m_launchApps(const QString& path)
{
    QString wtype;
    QString project_path = path;
    QString pro_file = ui->leProFile->text();

    ui->rbWidget->isChecked() ? wtype = "widget" : wtype = "mainwindow";
    MyFileInfo formi(wtype + ".ui", ui->leFormFile->text(), "src");
    MyFileInfo proi("qumbiaproject-" + m_selectedFactory + ".pro", pro_file, "");

    QList<QLineEdit *> lesCmd = QList<QLineEdit *>() << ui->leLaunch1 << ui->leLaunch2;
    foreach(QLineEdit *le, lesCmd)
    {
        if(le->isEnabled() && !le->text().isEmpty())
        {
            QStringList cmdline = le->text().split(QRegExp("\\s+"), QString::SkipEmptyParts);
            QStringList args(cmdline);

            if(cmdline.size() > 1)
            {
                args.removeFirst();
                for(int i = 0; i < args.size(); i++)
                {
                    args[i].replace("$PRO_FILE", proi.newFileName);
                    args[i].replace("$UI_FILE",  formi.subDirName + "/" + formi.newFileName);
                }
            }
            QProcess process(this);
            int res = process.startDetached(cmdline.first(), args, project_path);
            if(!res)
                QMessageBox::critical(this, "Application launch failed",
                                      QString("Failed to launch %1:\n%2").arg(cmdline.join(" "))
                                      .arg(process.errorString()));
        }
    }

    printf("\e[1;32m*\n* \e[0mcopy and paste the following line to change into the project directory:\n\e[1;32m*\e[0m\n");
    printf("\e[1;32mcd %s\e[0m\n", project_path.toStdString().c_str());

    if(ui->cbCloseAfterCreate->isChecked())
        close();
}

