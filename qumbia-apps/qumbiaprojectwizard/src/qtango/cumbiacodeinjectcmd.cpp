#include "cumbiacodeinjectcmd.h"
#include "codeextractors.h"
#include "codeinjector.h"
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QtDebug>

CumbiaCodeInjectCmd::CumbiaCodeInjectCmd(const QString &filename,
                                         const QString &mainwidgetclass,
                                         const QString &mainwidgetvar,
                                         const QString &uiformclass)
    : FileCmd(filename)
{
    m_err = false;
    m_mainwidgetclass = mainwidgetclass;
    m_mainwidgetvar = mainwidgetvar;
    m_uiformclass = uiformclass;
    qDebug() << __FUNCTION__ << "  --> " << m_mainwidgetclass << m_mainwidgetvar;
}

QString CumbiaCodeInjectCmd::process( const QString &input)
{
    m_err = false;
    qDebug() << __FUNCTION__ << "entering!" << filename();
    QString output;
    QString fnam = filename();
    if(fnam.endsWith("main.cpp")) {
        MainCppCodeExtractor mcppex;
        QList<Section> main_sections = mcppex.extract(CodeExtractorA::MainCpp);
        if((m_err = mcppex.error()))
            m_msg = mcppex.errorMessage();
        else {
            CodeInjector ci(fnam, m_mainwidgetclass, m_mainwidgetvar, m_uiformclass);
            output = ci.inject(input, main_sections);
            m_err = ci.error();
            if(m_err)
                m_msg = ci.errorMessage();
            m_log.append(ci.getLog());
        }
    }
    else if(fnam.endsWith(".cpp")) {
        MainWidgetCppCodeExtractor mwce;
        QList<Section> cpp_sections = mwce.extract(CodeExtractorA::MainWCpp);
        if((m_err = mwce.error()))
            m_msg = mwce.errorMessage();
        else {
            CodeInjector ci(fnam, m_mainwidgetclass, m_mainwidgetvar, m_uiformclass);
            output = ci.inject(input, cpp_sections);
            m_err = ci.error();
            if(m_err)
                m_msg =  ci.errorMessage();
            m_log.append(ci.getLog());
        }
    }
    else if(fnam.endsWith(".h")) {
        MainWidgetHCodeExtractor mwhe;
        QList<Section> h_sections = mwhe.extract(CodeExtractorA::MainWH);
        if((m_err = mwhe.error()))
            m_msg = mwhe.errorMessage();
        else
        {
            CodeInjector ci(fnam, m_mainwidgetclass, m_mainwidgetvar, m_uiformclass);
            output = ci.inject(input, h_sections);
            m_err = ci.error();
            if(m_err)
                m_msg = ci.errorMessage();
            m_log.append(ci.getLog());
        }
    }

    return output;
}

QString CumbiaCodeInjectCmd::name()
{
    return "CumbiaCodeInjectCmd";
}


QString CodeExtractorA::m_get_file_contents(CodeExtractorA::Type t)
{
    QString s;
    QMap<CodeExtractorA::Type, QString> filenamMap;
    filenamMap[CodeExtractorA::MainCpp] = QString(TEMPLATES_PATH) + "/main-tango.cpp";
    filenamMap[CodeExtractorA::MainWCpp] = QString(TEMPLATES_PATH) + "/widget-tango.cpp";
    filenamMap[CodeExtractorA::MainWH] = QString(TEMPLATES_PATH) + "/widget-tango.h";
    filenamMap[CodeExtractorA::Pro] = QString(TEMPLATES_PATH) + "qumbiaproject-tango.pro";

    QFile f(filenamMap[t]);
    m_err = !f.open(QIODevice::ReadOnly|QIODevice::Text);
    if(!m_err) {
        QTextStream in(&f);
        s = in.readAll();
        f.close();
    }
    else
        m_msg = "CumbiaCodeInjectCmd::m_get_file_contents: error opening template file " + f.fileName() + ": " + f.errorString();

    return s;
}
