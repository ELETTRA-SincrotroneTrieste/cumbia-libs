#include "main2cu.h"
#include "findreplace.h"
#include "conversionhealth.h"
#include <QTextStream>
#include <QFileInfo>
#include <QtDebug>

Main2Cu::Main2Cu(const QString &fname)
{
    m_filenam = fname;
    m_error = false;
    m_w_inHeap = false;
}

Main2Cu::Main2Cu()
{
    m_error = false;
    m_w_inHeap = false;
}

Main2Cu::~Main2Cu()
{

}

void Main2Cu::setFileName(const QString &fname)
{
    m_filenam = fname;
}

/**
 * @brief Main2Cu::findMainWidget finds classnam declaration in main.cpp
 * @param classnam the main widget class name to find
 * @return true if the main file was correctly opened AND the main widget
 *         class variable names have been detected
 */
bool Main2Cu::findMainWidget(const QString &classnam)
{
    QFile file(m_filenam);
    m_error = !file.open(QIODevice::Text|QIODevice::ReadOnly);
    if(!m_error) {
        /* \s*MyWidget\s+([a-zA-Z0-9_]*); */
        QRegExp re1(QString("\\s*%1\\s+([a-zA-Z0-9_]*);").arg(classnam));
        /* allocated with new operator
         * \s*MyWidget\s*\*\s*([a-zA-Z0-9_]*)\s*=\s*new\s+MyWidget\s*\(
         */
        QRegExp re2(QString("\\s*%1\\s*\\*\\s*([a-zA-Z0-9_]*)\\s*=\\s*new\\s+%1\\s*\\(").arg(classnam));
        QTextStream in(&file);
        QString maincpp = in.readAll();
        int pos = re1.indexIn(maincpp);
        if(pos > -1) {
            m_mainwidget_varname = re1.cap(1);
            m_w_inHeap = false;
        }
        else if((pos = re2.indexIn(maincpp)) > -1) {
            m_mainwidget_varname = re1.cap(1);
            m_w_inHeap = true;
        }
        qDebug() << __FUNCTION__ << "pos" << pos << " in " << maincpp;
        file.close();
    }
    else
        m_errMsg = file.errorString();

    return !m_error && !m_mainwidget_varname.isEmpty();
}

/*
 *  a.setOrganizationName("Elettra");
    a.setApplicationName("Danfisik9000");
    QString version(CVSVERSION);
    a.setApplicationVersion(version);
    a.setProperty("author", "Giacomo");
    a.setProperty("mail", "giacomo.strangolino@elettra.trieste.it");
    a.setProperty("phone", "375-8073");
    a.setProperty("office", "T2PT025");
    a.setProperty("hwReferent", "Stefano Cleva"); // name of the referent that provides the device server
*/
QMap<QString, QString> Main2Cu::parseProps()
{
    QMap<QString, QString> props;
    QFile file(m_filenam);
    file.open(QIODevice::Text|QIODevice::ReadOnly);
    m_error = !file.isOpen();
    if(!m_error) {
        QTextStream in(&file);
        QString maincpp = in.readAll();
        int pos;
        // 1. organization name
        QRegExp re("setOrganizationName\\(\"(.*)\"\\)");
        re.setMinimal(true);
        pos = re.indexIn(maincpp);
        if(pos > -1)
            props["orgname"] = re.cap(1);
        // 2. application name
        re.setPattern("setApplicationName\\(\"(.*)\"\\)");
        pos = re.indexIn(maincpp);
        if(pos > -1)
            props["appname"] = re.cap(1);
        // 3. properties: example: setProperty\(\s*"author"\s*,\s*"(.*)"\s*\)\s*;
        QStringList properties = QStringList() << "author" << "mail" << "phone" << "office" << "hwReferent";
        foreach (QString p, properties) {
            re.setPattern(QString("setProperty\\(\\s*\"%1\"\\s*,\\s*\"(.*)\"\\s*\\)\\s*;").arg(p));
            pos = re.indexIn(maincpp);
            if(pos > -1)
                props[p] = re.cap(1);
        }
        qDebug() << __FUNCTION__ << "map" << props;
        file.close();
    }
    else
        m_errMsg = file.errorString();

    return props;
}

bool Main2Cu::error() const
{
    return m_error;
}

QString Main2Cu::errorMessage() const
{
    return m_errMsg;
}

QString Main2Cu::mainWidgetVar() const
{
    return m_mainwidget_varname;
}

bool Main2Cu::mainWidgetInHeap() const
{
    return m_w_inHeap;
}


