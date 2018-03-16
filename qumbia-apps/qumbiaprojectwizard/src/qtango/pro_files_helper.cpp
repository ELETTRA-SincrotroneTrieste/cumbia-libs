#include "pro_files_helper.h"
#include "main2cu.h"
#include <QtDebug>
#include <QFileInfoList>
#include <QDomDocument>
#include <QDir>


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
 *   <class>Danfisik9000</class> <-- ui.firstChildElement("class")
 *   <widget class="QWidget" name="Danfisik9000">
 */
bool ProjectFilesHelper::findMainWidgetProps(const QDir& wdir)
{
    m_err = false;
    bool mainw_found = false;
    QString dom_err, classnam;
    QFileInfoList uifil = findFiles(wdir, "*.ui");
    QString maincpp_nam = findFile(wdir, "main.cpp");
    Main2Cu m2cu;
    if(maincpp_nam.isEmpty()) {
        m_err = true;
        m_errMsg = "QTangoImport.m_getMainWidgetProps: file main.cpp not found under\n" + wdir.absolutePath();
        return false;
    }
    QFile maincpp(maincpp_nam);
    qDebug() << __FUNCTION__ << "processing " + maincpp_nam << wdir.absolutePath() << "ui files" << uifil.size();
    if(maincpp.exists()) {
        m_proFiles["main"] = maincpp_nam;
        for(int i = 0; i < uifil.size() && !mainw_found; i++) {
            QFileInfo fi = uifil.at(i);
            QDomDocument dom(fi.absoluteFilePath());
            qDebug() << __FUNCTION__ << "opening " << fi.absoluteFilePath();
            QFile f(fi.absoluteFilePath());
            if(f.open(QIODevice::ReadOnly)) {
                qDebug() << __FUNCTION__ << "processing main: " + maincpp.fileName() + " + ui: " + f.fileName();
                if (dom.setContent(&f, &dom_err)) {
                    QDomElement uiEl = dom.firstChildElement("ui");
                    if(!uiEl.isNull() && uiEl.attribute("version").contains(QRegExp("4\\.[\\d]+"))) {
                        QDomElement classEl = uiEl.firstChildElement("class");
                        qDebug() << classEl.tagName() << "<<--- class el" << dom.documentElement().tagName() << dom.childNodes().count();
                        if(!classEl.isNull()) {
                            classnam = classEl.text();
                            qDebug() << __FUNCTION__ << "detected main widget " << classnam << "in " << f.fileName();
                            if(!classnam.isEmpty()) {
                                m2cu.setFileName(maincpp_nam);
                                mainw_found = m2cu.findMainWidget(classnam);
                                m_proFiles["uifile"] = fi.fileName();
                            }
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
        }
    }
    if(!m_err && mainw_found) {
        m_appPropMap = m2cu.parseProps();
        m_mainwidgetvarnam = m2cu.mainWidgetVar();
        m_mainwidgetnam = classnam;
    }
    if(!m_err && !mainw_found) {
        m_err = true;
        m_errMsg = "QTangoImport.m_getMainWidgetProps: failed to find a main widget instantiation in main.cpp";
    }
    return !m_err && mainw_found;
}

bool ProjectFilesHelper::findMainProjectFiles(const QDir &wdir)
{
    QFileInfoList fil = findFiles(wdir, "*.cpp");
    QRegExp cppre(QString("%1::%1\\s*\\(").arg(this->m_mainwidgetnam)); // find class constructor implementation in cpp
    foreach(QFileInfo cf, fil){
        QFile f(cf.absoluteFilePath());
        if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            if(cf.fileName().contains(QRegExp("main.cpp"))) {
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
    QRegExp hre(QString("class\\s+%1\\s*\\n*:\\s*public\\s+").arg(this->m_mainwidgetnam)); // find class constructor declaration in h
    foreach(QFileInfo cf, fil){
        if(cf.fileName().contains(QRegExp("ui_.*.h")))
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
        m_errMsg = "QTangoImport.m_findMainProjectFiles: did not find definitions for \"" + m_mainwidgetnam + "\" in cpp/h files";
    }
    return !m_err;
}

QFileInfoList ProjectFilesHelper::findFiles(QDir wdir, const QString& filter, const QString& exclude_regexp) const
{
    QFileInfoList uifil = wdir.entryInfoList(QStringList() << filter, QDir::Files);
    if(wdir.cd("src")) {
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
        if(exclude_regexp.isEmpty() || !fi.fileName().contains(QRegExp(exclude_regexp)))
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
