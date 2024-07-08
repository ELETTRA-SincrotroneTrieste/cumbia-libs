#include "pro_files_helper.h"
#include "main2cu.h"
#include <QtDebug>
#include <QFileInfoList>
#include <QDomDocument>
#include <QDir>
#include <QRegularExpression>


ProjectFilesHelper::ProjectFilesHelper() {

}

ProjectFilesHelper::ProjectFilesHelper(const QString& projnam)
{
    m_projnam = projnam;
    m_proFiles["pro"] = projnam;
}

/*
 * <?xml version="1.0" encoding="UTF-8"?>
 *   <ui version="4.0"> <-- document.firstChildElement("ui")
 *   <class>Danfisik9000</class> <-- ui.firstChildElement(TTT::Class)  // ui.firstChildElement("class")
 *   <widget class="QWidget" name="Danfisik9000">
 */
bool ProjectFilesHelper::findMainWidgetProps(const QDir& wdir)
{
    m_err = false;
    bool mainw_found = false;
    QString dom_err;
    QFileInfoList uifil = findFiles(wdir, "*.ui");
    QFileInfoList hfiles = findFiles(wdir, "*.h");
    QString maincpp_nam = findFile(wdir, "main.cpp");
    if(maincpp_nam.isEmpty()) {
        m_err = true;
        m_errMsg = "QTangoImport.m_getMainWidgetProps: file main.cpp not found under\n" + wdir.absolutePath();
        return false;
    }
    QFile maincpp(maincpp_nam);
    qDebug() << __FUNCTION__ << "processing " + maincpp_nam << wdir.absolutePath() << "ui files" << uifil.size();
    m_err = !maincpp.exists();
    if(!m_err) {
        m_proFiles["main"] = maincpp_nam;

        QString classnam;

        Main2Cu m2cu; // "main to cumbia migration helper class
        m2cu.setFileName(maincpp_nam);
        // for each h file, find the QWidget derived class name defined in the header file
        // for each class name, see which one is instantiated in the main.cpp
        for(int i = 0; i < hfiles.size() && !mainw_found; i++) {
            QFileInfo fi = hfiles.at(i);
            classnam = m_findWidgetClassName(fi.absoluteFilePath());
            if(!classnam.isEmpty()) {
                qDebug() << __FUNCTION__ << "detected widget " << classnam << "in " << fi.fileName();
                mainw_found = m2cu.findMainWidget(classnam);
            }
        }

        if(!m_err && mainw_found) {
            m_appPropMap = m2cu.parseProps();
            m_mainwidgetvarnam = m2cu.mainWidgetVar();
            m_mainwclassnam = classnam;
        }
        if(!m_err && !mainw_found) {
            m_err = true;
            m_errMsg = "QTangoImport.m_getMainWidgetProps: failed to find a main widget instantiation in main.cpp";
        }

        // if there is only one ui file we can extract the class name from the form,
        // if there are multiple ui files extract the top level widget object name
        // from the "class" element text and try to see if it matches the name of the
        // main widget (m_mainwclassnam). If it matches, we can assume we've guessed
        // the correct form class name
        //
        for(int i = 0; i < uifil.size(); i++)
        {
            QFileInfo fi = uifil.at(i);
            m_proFiles["uifile"] = fi.fileName();
            QDomDocument dom(fi.absoluteFilePath());
            qDebug() << __FUNCTION__ << "opening " << fi.absoluteFilePath();
            QFile f(fi.absoluteFilePath());
            if(f.open(QIODevice::ReadOnly)) {
                qDebug() << __FUNCTION__ << "processing main: " + maincpp.fileName() + " + ui: " + f.fileName();
                if (dom.setContent(&f, &dom_err)) {
                    QDomElement uiEl = dom.firstChildElement("ui");
                    if(!uiEl.isNull() && uiEl.attribute("version").contains(QRegularExpression("4\\.[\\d]+"))) {
                        QDomElement classEl = uiEl.firstChildElement("class");  // uiEl.firstChildElement("class") !cudata
                        qDebug() << classEl.tagName() << "<<--- class el" << dom.documentElement().tagName() << dom.childNodes().count();
                        if(!classEl.isNull()) {
                            m_uiclassnam = classEl.text();
                        }
                        else {
                            m_err = true;
                            m_errMsg = "QTangoImport.m_getMainWidgetProps: no \"class\" child element";
                        }
                    }
                    else {
                        m_err = true;
                        m_errMsg =  "QTangoImport.m_getMainWidgetProps: no \"ui\" child element or \"ui\" \"version\" not \"4.x\"";
                    }
                }
                else {
                    m_err = true;
                    m_errMsg = "QTangoImport.m_getMainWidgetProps: failed to parse ui xml file " + f.fileName() + ": " + dom_err;
                }
                f.close();
            }
            else {
                m_err = true;
                m_errMsg = "QTangoImport.m_getMainWidgetProps: failed to open file " + fi.absoluteFilePath() + " in read mode";
            }
            if(m_uiclassnam == m_mainwclassnam && uifil.size() > 1)
                break;
        }
    } // closes if(maincpp exists)
    return !m_err && mainw_found;
}

bool ProjectFilesHelper::findMainProjectFiles(const QDir &wdir)
{
    QFileInfoList fil = findFiles(wdir, "*.cpp");
    QRegularExpression cppre(QString("%1::%1\\s*\\(").arg(this->m_mainwclassnam)); // find class constructor implementation in cpp
    foreach(QFileInfo cf, fil){
        QFile f(cf.absoluteFilePath());
        if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            if(cf.fileName().contains(QRegularExpression("main.cpp"))) {
                m_proFiles["main"] = "main.cpp";
                continue;
            }
            QTextStream in(&f);
            qDebug() << __FUNCTION__ << "searching " << cppre.pattern() << " in " << f.fileName();
            if(in.readAll().contains(cppre)) {
                m_proFiles["cppfile"] = cf.fileName();
                break;
            }
            f.close();
        }
        else {
            m_err = true;
            m_errMsg = "QTangoImport.m_findMainProjectFiles: failed to open file " + f.fileName() + ": " + f.errorString();
        }
    }
    fil = findFiles(wdir, "*.h");
    QRegularExpression hre(QString("class\\s+%1\\s*\\n*:\\s*public\\s+").arg(this->m_mainwclassnam)); // find class constructor declaration in h
    foreach(QFileInfo cf, fil){
        if(cf.fileName().contains(QRegularExpression("ui_.*.h")))
            continue;

        QFile f(cf.absoluteFilePath());
        if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QTextStream in(&f);
            if(in.readAll().contains(hre)) {
                m_proFiles["hfile"] = cf.fileName();
                break;
            }
            f.close();
        }
        else {
            m_err = true;
            m_errMsg = "QTangoImport.m_findMainProjectFiles: failed to open file " + f.fileName() + ": " + f.errorString();
        }
    }
    if(!m_proFiles.contains("cppfile") || !m_proFiles.contains("hfile")) {
        m_err = true;
        m_errMsg = "QTangoImport.m_findMainProjectFiles: did not find definitions for \"" + m_mainwclassnam + "\" in cpp/h files";
    }
    return !m_err;
}

QFileInfoList ProjectFilesHelper::findFiles(QDir wdir, const QString& filter, const QString& exclude_regexp) const
{
    QFileInfoList uifil = wdir.entryInfoList(QStringList() << filter, QDir::Files);
    if(wdir.cd("src")) {  // wdir.cd("src") !cudata
        uifil << wdir.entryInfoList(QStringList() << filter, QDir::Files);
        qDebug() << __FUNCTION__ << "found ui under " << wdir.absolutePath() << uifil.size();
        wdir.cdUp();
    }
    if(wdir.cd("ui")) {
        uifil << wdir.entryInfoList(QStringList() << filter, QDir::Files);
        qDebug() << __FUNCTION__ << "found ui under " << wdir.absolutePath() << uifil.size();
        wdir.cdUp();
    }
    QFileInfoList outfi;
    foreach (QFileInfo fi, uifil) {
        if(exclude_regexp.isEmpty() || !fi.fileName().contains(QRegularExpression(exclude_regexp)))
            outfi << fi;
    }
    return outfi;
}

QString ProjectFilesHelper::findFile(QDir wdir, const QString &name) const
{
    QFileInfoList uifil = wdir.entryInfoList(QStringList() << name, QDir::Files);

    if(uifil.isEmpty()) {
        QFileInfoList subdirs = wdir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
        foreach(QFileInfo subd, subdirs) {
            wdir.cd(subd.fileName());
            qDebug() << __FUNCTION__ << "searching " << name << " under " << wdir.absolutePath();
            uifil = wdir.entryInfoList(QStringList() << name, QDir::Files);
            wdir.cdUp();
            if(!uifil.isEmpty())
                break;
        }
    }
    if(uifil.size() > 0)
        return uifil.first().absoluteFilePath();
    return "";
}

// find class name
// \s*class ([A-Za-z0-9_]+)\s*:\s*public\s+(?:QWidget|QMainWindow)
// example:
// class PS2605: public QWidget {
// will capture PS2605
QString ProjectFilesHelper::m_findWidgetClassName(const QString &filenam)
{
    QString classn;
    QFile f(filenam);
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QTextStream in(&f);
        QString contents = in.readAll();
        QRegularExpression classDefRe("\\s*class ([A-Za-z0-9_]+)\\s*:\\s*public\\s+(?:QWidget|QMainWindow)");
        QRegularExpressionMatch ma = classDefRe.match(contents);
        if(ma.hasMatch() && ma.capturedTexts().size() > 1)
            classn =  ma.capturedTexts().at(1);
        f.close();
    }
    else
        m_errMsg = "ProjectFilesHelper::m_findWidgetClassName: " + f.errorString();
    return classn;
}
