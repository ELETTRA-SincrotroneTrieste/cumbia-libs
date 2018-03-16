#include "codeextractors.h"
#include <QtDebug>

//
// regexp
// (//\s+cumbia\-tango\s*\n*(.*:?)// cumbia\-tango\s*\n*)
// two sections are expected: 1. include files 2. cumbia code
//
QList<Section> ExtractorHelper::extractCumbiaTangoSections(const QString& s)
{
    QList<Section>  sections;
    QRegExp re1("(//\\s+cumbia\\-tango\\s*\\n*(.*:?)// cumbia\\-tango\\n*)");
    re1.setMinimal(true);

    int pos = 0; // where we are in the string
    while(pos >= 0) {
        pos = re1.indexIn(s, pos);
        if(pos >=0) {
            qDebug() << __FUNCTION__ << "pos" << pos << re1.cap(1);
            sections << Section(newline_wrap(re1.cap(1)), Section::NoSection);
            pos += re1.cap(1).length();
        }
    }
    return sections;
}

QString ExtractorHelper::newline_wrap(const QString &s)
{
    return "\n" + s + "\n";
}

CodeExtractorA::Type MainCppCodeExtractor::type() const
{
    return CodeExtractorA::MainCpp;
}

QList<Section> MainCppCodeExtractor::extract(Type t)
{
    QList<Section> sections;
    QString s = m_get_file_contents(t);
    qDebug() << __FUNCTION__ << "enter type " << t << "error flag" << m_err;

    foreach(QString l, s.split("\n")) {
        // CumbiaTango *cu_t = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
        if(l.contains(QRegExp("new\\s+CumbiaTango")))
            sections.push_back(Section(ExtractorHelper().newline_wrap(l), Section::MainCppBeforeNewWidget));
    }

    QList<Section> cumbia_t_sections = ExtractorHelper().extractCumbiaTangoSections(s);
    m_err = cumbia_t_sections.size() != 1;
    if(!m_err)
    {
        cumbia_t_sections[0].where = Section::Includes;
        sections.append(cumbia_t_sections[0]);
        // extract this section from the template:
        // exec application loop
        // ret = a.exec();
        // delete resources and return
        // delete w;
        // delete cu_t;
        // return ret;
        // }
        QRegExp qapp_exec_re("(// exec application loop\\n*\\s*.*)\\}");
        int pos = qapp_exec_re.indexIn(s);
        if(pos > -1)
            sections.push_back(Section(ExtractorHelper().newline_wrap(qapp_exec_re.cap(1)), Section::EndOfMain));
    }
    else
        m_msg = "MainCppCodeExtractor.extract: missing cumbia-tango section in main.cpp template file";
    return sections;
}


CodeExtractorA::Type MainWidgetCppCodeExtractor::type() const
{
    return CodeExtractorA::MainWCpp;
}

// extract this sections from the template
// 1 (start of constructor)
// (//\s+cumbia\-tango\s*\n*(.*:?)// cumbia\-tango)
// cumbia-tango
// cu_t = cut;
// m_log = new CuLog(&m_log_impl);
// cu_t->getServiceProvider()->registerService(CuServices::Log, m_log);
//
// ui->setupUi(this, cu_t, cu_tango_r_fac, cu_tango_w_fac);
// cumbia-tango
//
// 2 (end of constructor)
// new CuContextActionBridge(this, cu_t, cu_tango_r_fac);
QList<Section> MainWidgetCppCodeExtractor::extract(Type t)
{
    QList<Section> sections;
    QString s = m_get_file_contents(t); // sets m_err
    qDebug() << __FUNCTION__ << "enter type " << t << "error flag" << m_err;
    if(m_err) // error opening template file
        return sections;

    // extraction 1
    QList<Section> cu_t_secs = ExtractorHelper().extractCumbiaTangoSections(s);
    m_err = cu_t_secs.size() != 2;
    if(m_err)
        m_msg = "MainWidgetCppCodeExtractor::extract: found " + QString::number(cu_t_secs.size()) +
                " cumbia-tango sections in template file. Must be 2";
    if(!m_err) {
        cu_t_secs[0].where = Section::Includes;
        cu_t_secs[1].where = Section::StartOfCppConstructor;
        sections.append(cu_t_secs);
        // extraction 2
        // regexp (\s*new\s+CuContextActionBridge[A-Za-z0-9_,\s\)\(]*;\n)
        // in template the line is:
        // new CuContextActionBridge(this, cu_t, cu_tango_r_fac);
        int pos;
        QRegExp re2("(\\s*new\\s+CuContextActionBridge[A-Za-z0-9_,\\s\\)\\(]*;\\n)");
        pos = re2.indexIn(s);
        m_err = pos < 0;
        if(!m_err) {
            sections << Section(re2.cap(1), Section::EndOfCppConstructor);
        }
        else
            m_msg = "MainWidgetCppCodeExtractor::extract: CuContextActionBridge line not found in template file";
    }
    return sections;
}

CodeExtractorA::Type MainWidgetHCodeExtractor::type() const
{
    return CodeExtractorA::MainWH;
}

QList<Section> MainWidgetHCodeExtractor::extract(CodeExtractorA::Type t)
{
    QList<Section> sections;
    QString s = m_get_file_contents(t); // sets m_err
    if(m_err) // error opening template file
        return sections;

    // extraction
    QList<Section> cu_t_secs = ExtractorHelper().extractCumbiaTangoSections(s);
    m_err = cu_t_secs.size() != 2;
    if(m_err)
        m_msg = "MainWidgetCppCodeExtractor::extract: found " + QString::number(cu_t_secs.size()) +
                " cumbia-tango sections in template file. Must be 2";
    else {
        cu_t_secs[0].where = Section::Includes;
        cu_t_secs[1].where = Section::EndOfHConstructor;
        sections.append(cu_t_secs);
    }

    qDebug() << __FUNCTION__ << "extraction result from H file" << cu_t_secs.size();
    return sections;
}

