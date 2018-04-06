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
#include <QMessageBox>

#include "findreplace.h"
#include "definitions.h"
#include "pro_files_helper.h"
#include "cumbiacodeinjectcmd.h"
#include "maincpppreparecmd.h"
#include "proconvertcmd.h"
#include "codeextractors.h"
#include "prosectionextractor.h"
#include "mainwidgetcppconstructprocesscmd.h"
#include "mainwidgethprocesscmd.h"
#include "maincppexpandcmd.h"
#include "cppinstantiationexpand.h"

QTangoImport::QTangoImport()
{

}

bool QTangoImport::open(const QString &pro_filepath)
{
    bool ok = true;
    qDebug() << __FUNCTION__ << pro_filepath;
    QFile f(pro_filepath);
    QFileInfo fi(f);
    QDir wd = fi.absoluteDir();
    m_projdir = wd.absolutePath();
    QString projnam = fi.fileName();
    m_pro_file_path = pro_filepath;
    m_proFHelper = ProjectFilesHelper(projnam.remove(".pro"));
    ok = m_checkPro(f);
    if(ok)
        ok = m_proFHelper.findMainWidgetProps(wd);
    if(ok)
        ok = m_proFHelper.findMainProjectFiles(wd);

    if(f.isOpen())
        f.close();
    return ok;

}

QMap<QString, QString> QTangoImport::getAppProps() const
{
    return m_proFHelper.appPropMap();
}

QMap<QString, QString> QTangoImport::getProjectFiles() const
{
    return m_proFHelper.projectFilesMap();
}

QString QTangoImport::projectName() const
{
    return m_proFHelper.projectName();
}

QString QTangoImport::projectDir() const
{
    return m_projdir;
}

QString QTangoImport::projectFilePath() const
{
    return m_pro_file_path;
}

QString QTangoImport::mainWidgetVarName() const
{
    return m_proFHelper.mainWidgetVarName();
}

QString QTangoImport::mainWidgetClassName() const
{
    return m_proFHelper.mainWidgetClassName();
}

QString QTangoImport::uiFormClassName() const
{
    return m_proFHelper.uiClassName();
}

QString QTangoImport::errorMessage() const
{
    return m_errMsg;
}

bool QTangoImport::error() const
{
    return m_err;
}

bool QTangoImport::convert() {
    Definitions defs;
    m_err =  !defs.load(QString(TEMPLATES_PATH) + "/qtango.keywords.json");
    if(m_err) {
        m_errMsg = defs.errorMessage();
        return false;
    }

    QDir wdir(m_projdir);
    QMap<QString, QString> project_files = m_proFHelper.projectFilesMap();
    QString maincpp = "main.cpp";
    QString mainwcpp = project_files["cppfile"];
    QString mainwh = project_files["hfile"];
    QString pronam = project_files["pro"] + ".pro";
    QStringList mainfiles = QStringList() << maincpp << mainwcpp << mainwh;
    QFileInfoList hfiles = m_proFHelper.findFiles(wdir, "*.h", "ui_.*");
    QFileInfoList cppfiles = m_proFHelper.findFiles(wdir, "*.cpp");
    QFileInfoList uifiles = m_proFHelper.findFiles(wdir, "*.ui");
    QFileInfoList profiles= m_proFHelper.findFiles(wdir, "*.pro");
    QFileInfoList allfiles(hfiles);
    allfiles.append(cppfiles);
    allfiles.append(uifiles);
    allfiles.append(profiles);
    m_err = maincpp.isEmpty() || mainwcpp.isEmpty() || mainwh.isEmpty();

    bool can_proceed = !m_err && !m_proFHelper.mainWidgetClassName().isEmpty() && !m_proFHelper.mainWidgetVarName().isEmpty()
            && !maincpp.isEmpty() && !mainwcpp.isEmpty() && !mainwh.isEmpty();

    m_err = !can_proceed;
    // * open must have been called and main widget definition and implementation must have been
    //   found
    // * apply find replace command to all h, cpp and ui files
    if(can_proceed) {
        foreach(QFileInfo fi, allfiles)
        {
            QString contents = m_get_file_contents(fi.absoluteFilePath());
            if(m_err)
                return false;

            MacroFileCmd fcmd(contents);
            if(fi.fileName() == pronam)
                fcmd.registerCommand(new ProConvertCmd(fi.fileName()));
            if(fi.fileName() == maincpp)
                fcmd.registerCommand(new MainCppPrepareCmd(fi.fileName(), m_proFHelper.mainWidgetClassName()));
            fcmd.registerCommand(new FindReplace(fi.fileName()));
            if(mainfiles.contains(fi.fileName()))
                fcmd.registerCommand(new CumbiaCodeInjectCmd(fi.fileName(), mainWidgetClassName(),
                                                             mainWidgetVarName(), uiFormClassName()));
            if(fi.fileName() == mainwcpp) {
                fcmd.registerCommand(new MainWidgetCppConstructProcessCmd(fi.fileName(), mainWidgetClassName()));
                fcmd.registerCommand(new CppInstantiationExpand(fi.fileName()));
            }
            if(fi.fileName() == mainwh)
                fcmd.registerCommand(new MainWidgetHProcessCmd(fi.fileName(), mainWidgetClassName(), m_proFHelper.uiClassName()));
            if(fi.fileName() == maincpp)
                fcmd.registerCommand(new MainCppExpandCmd(fi.fileName(), mainWidgetClassName()));
            m_err = !fcmd.process(defs);
            if(m_err) {
                m_errMsg = fcmd.errorMessage();
                break;
            }
            else {
                printf("\e[1;32m:-) \e[0m successfully processed file %s\n", fi.absoluteFilePath().toStdString().c_str());
                printf("\e[1;32m    +\e[0m mapped file %s\e[0m\n", fi.fileName().toStdString().c_str());
                m_contents[fi.fileName()] = fcmd.contents();
            }
            emit newLog(fcmd.log());

        }
        if(m_err)
            return !m_err;
    }
    else
        m_errMsg = "QTangoImport.convert: cannot proceed: some files are missing";

    emit conversionFinished(!m_err);
    return !m_err;
}

// remove from every section SOURCES += HEADERS +=... [A-Z\s+=]*
// ([A-Za-z0-9_\./\-]+)
bool QTangoImport::findFilesRelPath()
{
    ProSectionExtractor pe(m_pro_file_path);
    m_err = pe.error();
    if(m_err) {
        m_errMsg = pe.errorMessage();
    }
    else {
        QString filename, relpath;
        QRegExp srcre("([A-Za-z0-9_\\./\\-]+)");
        int pos;
        QStringList files_sections = QStringList() << "SOURCES" << "HEADERS" << "FORMS";
        foreach(QString section, files_sections) {
            QString filelist = pe.get(section).remove(QRegExp(section + "\\s*\\+="));
            qDebug() << __FUNCTION__ << "searching sources within " << filelist ;
            // capture sources now
            pos = 0;
            while( (pos = srcre.indexIn(filelist, pos)) != -1) {
                filename = srcre.cap(1).section('/', -1);
                relpath = srcre.cap(1).section('/', 0, srcre.cap(1).count('/') - 1);
                m_outFileRelPaths[filename] = relpath;
                pos += srcre.matchedLength();
                qDebug() << __FUNCTION__ << "found " << filename << relpath;
            }
        }
        m_outFileRelPaths[QFileInfo(m_pro_file_path).fileName()] = "";
    }
    return !m_err;
}

/*! \brief write on path/name the contents of the file specified by name
 *
 * \par Warning
 * call findFilesRelPath first
 *
 */
bool QTangoImport::outputFile(const QString &name, const QString &path)
{
    int ret = QMessageBox::Yes;
    QString relpath = m_outFileRelPaths[name];
    m_err = !m_outFileRelPaths.contains(name);
    if(m_err) {
        printf("\e[1;31mQTangoImport.outputFile: relative path unavailable for %s\e[0m\n", name.toStdString().c_str());
    }
    else {
        QString contents = m_contents[name];
        m_err = contents.isEmpty();
        if(m_err) {
            m_errMsg = "QTangoImport.outputFile: no file with name " + name + " has been processed";
        }
        else {
            QDir wdir(path);
            if(!relpath.isEmpty() && !wdir.exists(relpath))
                m_err = !wdir.mkpath(relpath);
            if(!m_err) {
                QFile f(path.endsWith("/") ? path : path + "/" + (!relpath.isEmpty() ? relpath + "/" : "") +  name);
                if(f.exists())
                    ret = QMessageBox::information(0, "File already exists", "File \"" + f.fileName() + "\" already exists\n"
                                                                                                        "Do you want to replace it?", QMessageBox::Yes, QMessageBox::No);
                if(ret == QMessageBox::Yes) {
                    m_err = !f.open(QIODevice::WriteOnly|QIODevice::Text);
                    if(!m_err) {
                        qDebug() << __FUNCTION__ << "WRITING " << f.fileName();
                        QTextStream out(&f);
                        out << contents;
                        f.close();
                    }
                    else
                        m_errMsg = "QTangoImport.outputFile: error writing " + name + ": " + f.errorString();
                }
            }
            else
                m_errMsg = "QTangoImport.outputFile: error creating directory " + relpath + " under " + path;
        }
    }
    emit outputFileWritten(name, relpath, !m_err);
    return !m_err;
}

QStringList QTangoImport::convertedFileList() const
{
    return m_contents.keys();
}

bool QTangoImport::m_checkPro(QFile &f) {
    m_err = true;
    if(f.open(QIODevice::ReadOnly|QIODevice::Text) ) {
        QTextStream in(&f);
        QString pro = in.readAll();
        QRegExp qtango_inc_re("include\\s*\\(.*/qtango[3-6]/qtango.pri\\)");
        m_err = !pro.contains(qtango_inc_re);
        if(m_err)
            m_errMsg = "QTangoImport.m_checkPro: file " + f.fileName() + " is not a qtango project file";
        f.close();
    }
    else
        m_errMsg = "QTangoImport.m_checkPro: failed to open " + f.fileName() + ": " + f.errorString();

    return !m_err;
}

bool QTangoImport::m_checkCpp(const QString &f) {

}

bool QTangoImport::m_checkH(const QString &h) {

}

QString QTangoImport::m_get_file_contents(const QString& filepath)
{
    QString s;
    QFile f(filepath);
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QTextStream in(&f);
        s = in.readAll();
        f.close();
    }
    else
        m_errMsg = "QTangoImport::m_get_file_contents: error opening file " + f.fileName() + ": " + f.errorString();

    return s;
}



