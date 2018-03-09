#include "qtangoimport.h"
#include "main2cu.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QtDebug>

#include "findreplace.h"


QTangoImport::QTangoImport()
{

}

bool QTangoImport::open(const QString &pro_filenam)
{
    bool ok = true;
    qDebug() << __FUNCTION__ << pro_filenam;
    QFile f(pro_filenam);
    QFileInfo fi(f);
    QDir wd = fi.absoluteDir();
    m_projdir = wd.absolutePath();
    QString projnam = fi.fileName();
    m_proFiles["pro"] = projnam;
    m_projnam = projnam.remove(".pro");
    ok = m_checkPro(f);
    if(ok)
        ok = m_getMainWidgetProps(wd, m_appPropMap);
    if(ok)
        ok = m_findMainProjectFiles(wd);
    QFileInfoList fil = wd.entryInfoList(QStringList() << "*.h" << "*.cpp" << "*.ui", QDir::Files);
    foreach(QFileInfo fi, fil) {

    }

    return ok;

}

QMap<QString, QString> QTangoImport::getAppProps() const
{
    return m_appPropMap;
}

QMap<QString, QString> QTangoImport::getProjectFiles() const
{
    return m_proFiles;
}

QString QTangoImport::projectName() const
{
    return m_projnam;
}

QString QTangoImport::projectDir() const
{
    return m_projdir;
}

QString QTangoImport::mainWidgetVarName() const
{
    return m_mainwidgetvarnam;
}

QString QTangoImport::mainWidgetName() const
{
    return m_mainwidgetnam;
}

QString QTangoImport::errorMessage() const
{
    return m_errMsg;
}

bool QTangoImport::error() const
{
    return m_err;
}

bool QTangoImport::convert()
{
    m_err = !m_conversionDefs.load(QString(TEMPLATES_PATH) + "/qtango.keywords");
    if(m_err)
        m_errMsg = m_conversionDefs.errorMessage();

    QDir wdir(m_projdir);

    QString maincpp = m_findFile(wdir, "main.cpp");
    m_err = maincpp.isEmpty();
    if(m_err)
        m_errMsg = "QTangoImport.convert: main.cpp not found in " + m_projdir;
    if(!m_err && !m_mainwidgetnam.isEmpty() && !m_mainwidgetvarnam.isEmpty()) {
        Main2Cu m2cu(maincpp);
        m_err = !m2cu.convert(m_mainwidgetnam, m_mainwidgetvarnam, m_conversionDefs);
        if(m_err)
            m_errMsg = m2cu.errorMessage();
    }

    return !m_err;
}

FindReplace QTangoImport::conversionDefs() const
{
    return m_conversionDefs;
}

bool QTangoImport::m_checkPro(QFile &f)
{
    m_err = true;
    if(f.open(QIODevice::ReadOnly|QIODevice::Text) ) {
        QTextStream in(&f);
        QString pro = in.readAll();
        QRegExp qtango_inc_re("include\\(.*/qtango[3-6]/qtango.pri\\)");
        m_err = !pro.contains(qtango_inc_re);
        if(m_err)
            m_errMsg = "QTangoImport.m_checkPro: file " + f.fileName() + " is not a qtango project file";
        f.close();
    }
    else
        m_errMsg = "QTangoImport.m_checkPro: failed to open " + f.fileName() + ": " + f.errorString();

    return !m_err;
}

bool QTangoImport::m_checkCpp(const QString &f)
{

}

bool QTangoImport::m_checkH(const QString &h)
{

}

/*
 * <?xml version="1.0" encoding="UTF-8"?>
 *   <ui version="4.0"> <-- document.firstChildElement("ui")
 *   <class>Danfisik9000</class> <-- ui.firstChildElement("class")
 *   <widget class="QWidget" name="Danfisik9000">
 */
bool QTangoImport::m_getMainWidgetProps(const QDir& wdir, QMap<QString, QString>& props)
{
    m_err = false;
    bool mainw_found = false;
    QString dom_err, classnam;
    QFileInfoList uifil = m_findFiles(wdir, "*.ui");
    QString maincpp_nam = m_findFile(wdir, "main.cpp");
    Main2Cu m2cu;
    if(maincpp_nam.isEmpty()) {
        m_err = true;
        m_errMsg = "QTangoImport.m_getMainWidgetProps: file main.cpp not found under\n" + wdir.absolutePath();
        return false;
    }
    QFile maincpp(maincpp_nam);
    qDebug() << __FUNCTION__ << "processing " + maincpp_nam << wdir.absolutePath() << "ui files" << uifil.size();
    if(maincpp.exists()) {
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
        props = m2cu.parseProps();
        m_mainwidgetvarnam = m2cu.mainWidgetVar();
        m_mainwidgetnam = classnam;
    }
    if(!m_err && !mainw_found) {
        m_err = true;
        m_errMsg = "QTangoImport.m_getMainWidgetProps: failed to find a main widget instantiation in main.cpp";
    }
    return !m_err && mainw_found;
}

bool QTangoImport::m_findMainProjectFiles(const QDir &wdir)
{
    QFileInfoList fil = m_findFiles(wdir, "*.cpp");
    QRegExp cppre(QString("%1::%1\\s*\\(").arg(this->m_mainwidgetnam)); // find class constructor implementation in cpp
    foreach(QFileInfo cf, fil){
        QFile f(cf.absoluteFilePath());
        if(f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            if(cf.fileName().contains(QRegExp("main.cpp")))
                continue;
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
    fil = m_findFiles(wdir, "*.h");
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

QFileInfoList QTangoImport::m_findFiles(QDir wdir, const QString& filter) const
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
    return uifil;
}

QString QTangoImport::m_findFile(QDir wdir, const QString &name) const
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


