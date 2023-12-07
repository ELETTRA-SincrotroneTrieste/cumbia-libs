#include "qumbianewcontrolw.h"
#include "ui_qumbianewcontrolw.h"

#include <QDir>
#include <QRegularExpression>
#include <QFileDialog>
#include <QMessageBox>
#include <QValidator>
#include <QTextStream>
#include <QtDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    foreach(QWidget *wi, QList<QWidget *>() << ui->leCpp << ui->leH)
        connect(wi, SIGNAL(textEdited(QString)), this, SLOT(setTextEdited(QString)));

    connect(ui->leName, SIGNAL(textChanged(QString)), this, SLOT(nameTextChanged(QString)));
    connect(ui->leOutDir, SIGNAL(textChanged(QString)), this, SLOT(outDirTextChanged(QString)));

    connect(ui->pbChangeOutDir, SIGNAL(clicked()), this, SLOT(changeOutDir()));
    connect(ui->pbGen, SIGNAL(clicked()), this, SLOT(generate()));

    QDir current;
    ui->leOutDir->setText(current.absolutePath());
    if(current.exists("src")) {  // current.exists("src") !cudata
        ui->leOutDir->setText(ui->leOutDir->text() + "/src");
        ui->leProUnder->setText("src");
        ui->cbAddToPro->setChecked(true);
    }

    QStringList entries = current.entryList(QStringList() << "*.pro", QDir::Files);
    if(entries.size() > 0)
        ui->leProFile->setText(entries.first());

    ui->pbGen->setDisabled(true);
    ui->leInclude->setText(ui->cbParent->currentText());
    connect(ui->cbParent, SIGNAL(currentTextChanged(QString)), this, SLOT(parentClassNameChanged(QString)));

    m_genIncludeMap();

    foreach(QString classnam, m_includeMap.keys()) {
        if(classnam.startsWith("Qu") || classnam.contains(QRegularExpression("\\bE[A-Z]")))
            ui->cbParent->insertItem(ui->cbParent->count(), classnam);
    }

    ui->leName->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z0-9_]+")));

    ui->pteBrief->setProperty("current_superclass", ui->cbParent->currentText());
    updateBrief();

    foreach(QWidget *wi, QList<QWidget *>() << ui->pteBrief << ui->pteDesc)
        connect(wi, SIGNAL(textChanged()), this, SLOT(setTextEdited()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::nameTextChanged(const QString &txt)
{

    if(!ui->leH->property("edited").toBool())
        ui->leH->setText(txt.toLower() + ".h");
    if(!ui->leCpp->property("edited").toBool())
        ui->leCpp->setText(txt.toLower() + ".cpp");
    ui->pbGen->setEnabled(!txt.isEmpty());
    ui->pteBrief->disconnect(SIGNAL(textChanged()));
    updateBrief();
    connect(ui->pteBrief, SIGNAL(textChanged()), this, SLOT(setTextEdited()));
    ui->pteBrief->setProperty("current_classnam", txt);
}

void Widget::updateBrief() {
    qDebug() << __FUNCTION__ << "cur classnam" << ui->pteBrief->property("current_classnam").toString()
             << "cur super " << ui->pteBrief->property("current_superclass").toString();
    QString s;
    if(ui->pteBrief->property("edited").toBool()) {
        s = ui->pteBrief->toPlainText();
        QString curclass = ui->pteBrief->property("current_classnam").toString();
        if(!curclass.isEmpty())
            s.replace(QRegularExpression("\\b" + curclass + "\\b"), "$MAINCLASS$");
        QString super =  ui->pteBrief->property("current_superclass").toString();
        if(!super.isEmpty())
            s.replace(QRegularExpression("\\b" + super + "\\b"), "$SUPERCLASS$");
    }
    else {
        QString htemplate = m_getTemplateContents(ui->leH);
        QRegularExpression rebrief("\\\\brief\\s+(.*)\\n");
        rebrief.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
        QRegularExpressionMatch ma = rebrief.match(htemplate);
        if(ma.hasMatch()) {
            s = ma.captured(1);
        }
    }
    qDebug() << __FUNCTION__ <<" replacing from " << s;
    ui->pteBrief->setPlainText(m_replace_from_template(s));
}

void Widget::setTextEdited(const QString &)
{
    sender()->setProperty("edited", true);
}

void Widget::changeOutDir()
{
    QString path = QFileDialog::getExistingDirectory(this);
    if(!path.isEmpty()) {
        ui->leOutDir->setText(path);
    }
}

QString Widget::m_replace_from_template(const QString &templ)
{
    QString out = templ;
    out.replace("$SUPER_INCLUDE$", ui->leInclude->text());
    out.replace("$MAINCLASS$", ui->leName->text());
    out.replace("$SUPERCLASS$", ui->cbParent->currentText());
    out.replace("$INCLUDE$", ui->leH->text());
    out.replace("$DOCDIR$", DOC_PATH);

    QRegularExpression classdocRe("/\\*\\*\\s+\\\\brief\\s+(.*) \\*/\\nclass\\s+");
    classdocRe.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    QString brief = ui->pteBrief->toPlainText();
    brief.replace("\n", "\n * ");
    QString desc = ui->pteDesc->toPlainText();
    desc.replace("\n", "\n * ");
    QString cldoc = "/*! \\brief " + brief + "\n *\n * " + desc + "\n *\n*/\nclass ";
    out.replace(classdocRe, cldoc);
    return out;
}

QString Widget::m_getTemplateContents(const QLineEdit* le)
{
    QString out, wtype;
    QString templates_path = TEMPLATES_PATH;
    foreach(QRadioButton *rb, ui->gbType->findChildren<QRadioButton *>())
        if(rb->isChecked())
            wtype = rb->property("type").toString();
    QString ft = templates_path + "/" + le->property("type").toString() + "_" + wtype + le->property("ext").toString();
    QFile ftemplate(ft);
    m_error = !ftemplate.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_error){
        QTextStream in_template(&ftemplate);
        out = in_template.readAll();
        ftemplate.close();
    }
    else {
        m_message = "Error opening template file in read mode: " + ftemplate.fileName() + ": " + ftemplate.errorString();
    }
    return out;
}

void Widget::generate()
{
    QString path = ui->leOutDir->text();
    QDir d;
    QList<QLineEdit*> les = QList<QLineEdit *>() << ui->leH << ui->leCpp;
    m_error = false;

    if(d.mkpath(path)) {
        foreach(QLineEdit *le, les) {
            QString fou = path + "/" + le->text();
            QFile fout(fou);
            QString template_s = m_getTemplateContents(le);
            if(!m_error) {
                if(fout.open(QIODevice::WriteOnly|QIODevice::Text)) {
                    QTextStream out(&fout);
                    out << m_replace_from_template(template_s);
                    fout.close();
                }
                else {
                    m_message = "Error opening file in write mode: " + fout.fileName() + ": " + fout.errorString();
                    m_error = true;
                }
            }
            if(m_error)
                break;
        } // foreach QLineEdit
    }

    // Process project file
    if(!m_error && !ui->leProFile->text().isEmpty() && ui->cbAddToPro->isChecked())
        m_add_to_pro();
    if(m_error)
        QMessageBox::critical(this, "Error", m_message);

}

void Widget::outDirTextChanged(const QString &s)
{
    ui->pbGen->setEnabled(!s.isEmpty());
}

void Widget::parentClassNameChanged(const QString &cn)
{
    if(m_includeMap.contains(cn))
        ui->leInclude->setText(m_includeMap[cn]);
    else
        ui->leInclude->setText(cn);

    ui->pteBrief->disconnect(SIGNAL(textChanged()));
    updateBrief();
    connect(ui->pteBrief, SIGNAL(textChanged()), this, SLOT(setTextEdited()));
    ui->pteBrief->setProperty("current_superclass", cn);
}

void Widget::m_genIncludeMap()
{
    QString incpath = QString(INCLUDE_PATH);
    QDir incdir(incpath);
    foreach(QFileInfo fi, incdir.entryInfoList(QStringList() << "*.h", QDir::Files)) {
        QFile f(fi.absoluteFilePath());
        if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QTextStream in(&f);
            QString s = in.readAll();
            QRegularExpression classre("class\\s+([A-Za-z0-9_]+)\\s*:\\s*public\\s+Q");
            QRegularExpressionMatch ma = classre.match(s);
            if(ma.hasMatch()) {
                qDebug() << __FUNCTION__ << fi.fileName() << " <---> " << ma.captured(1);
                m_includeMap[ma.captured(1)] = fi.fileName();
            }
            f.close();
        }
        else
            printf("\e[1;31m* \e[0merror opening include file \"%s\"\n", fi.absoluteFilePath().toStdString().c_str());
    }
}

void Widget::m_add_to_pro()
{
    // (\bSOURCES\s*[\+]*=\s*[a-zA-Z0-9/\s\\\.\$_]*)(\n|\n*$|\n\b[A-Za-z0-9]*)
    // (\bHEADERS\s*[\+]*=\s*[a-zA-Z0-9/\s\\\.\$_]*)(\n|\n*$|\n\b[A-Za-z0-9]*)
    QFile prof(ui->leProFile->text());
    if(prof.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QRegularExpression srcsre("(\\bSOURCES\\s*[\\+]*=\\s*[a-zA-Z0-9/\\s\\\\\\.\\$_]*)(\\n|\\n*$|\\n\\b[A-Za-z0-9]*)");
        QTextStream in(&prof);
        QString cap, orig;
        QString pros = in.readAll();
        QString prefix = ui->leProUnder->text();
        QString hfile, cppfile;
        prefix.endsWith('/') ? hfile = prefix + ui->leH->text() : hfile = prefix + "/" + ui->leH->text();
        prefix.endsWith('/') ? cppfile = prefix + ui->leCpp->text() : cppfile = prefix + "/" + ui->leCpp->text();
        // 1. cpp
        QRegularExpressionMatch ma = srcsre.match(pros);
        if(ma.hasMatch()) {
            orig = ma.captured(1);
            cap = orig;
            if(!cap.contains(cppfile)) {
                cap = orig.trimmed() + " \\ \n\t" + cppfile + "\n";
                qDebug() << __FUNCTION__ << "orig.trimmed is " << orig.trimmed() << "cap is " << cap;
                pros.replace(orig, cap);
            }
        }
        else
            QMessageBox::critical(this, "Error modifying pro file", "SOURCES section not detected in " + prof.fileName());

        // 2. h
        QRegularExpression hre("(\\bHEADERS\\s*[\\+]*=\\s*[a-zA-Z0-9/\\s\\\\\\.\\$_]*)(\\n|\\n*$|\\n\\b[A-Za-z0-9]*)");
        ma = hre.match(pros);
        if(ma.hasMatch()) {
            orig = ma.captured(1);
            cap = orig;
            if(!cap.contains(hfile)) {
                cap = orig.trimmed() + " \\ \n\t" + hfile + "\n";
                qDebug() << __FUNCTION__ << "orig.trimmed is " << orig.trimmed() << "cap is " << cap;
                pros.replace(orig, cap);
            }
        }
        else
            QMessageBox::critical(this, "Error modifying pro file", "HEADERS section not detected in " + prof.fileName());
        prof.close();

        if(prof.open(QIODevice::WriteOnly|QIODevice::Text)) {
            QTextStream out(&prof);
            out << pros;
            prof.close();
        }
        else
            QMessageBox::critical(this, "Error writing output on project file", "Error writing file " + prof.fileName());
    }
    else
        QMessageBox::critical(this, "Error reading project file", "Error reading file " + prof.fileName());

}
