#include "codeextractors.h"
#include <QtDebug>

//
// regexp
// (//\s+cumbia\s*\n*(.*:?)// cumbia\s*\n*)
// two sections are expected: 1. include files 2. cumbia code
//
QList<Section> ExtractorHelper::extractCumbiaPoolSections(const QString& s)
{
    QList<Section>  sections;
    QRegExp re1("(//\\s+cumbia\\s*\\n*(.*:?)// cumbia\\s*\\n*)");
    re1.setMinimal(true);

    int pos = 0; // where we are in the string
    while(pos >= 0) {
        pos = re1.indexIn(s, pos);
        if(pos >=0) {
            qDebug() << __PRETTY_FUNCTION__ << "pos" << pos << re1.cap(1);
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
    qDebug() << __PRETTY_FUNCTION__ << "enter type " << t << "error flag" << m_err;

    foreach(QString l, s.split("\n")) {
        // CumbiaPool *cu_t = new CumbiaPool();
        if(l.contains(QRegExp("new\\s+CumbiaPool")))
            sections.push_back(Section(ExtractorHelper().newline_wrap(l), Section::MainCppBeforeNewWidget));
    }

    QList<Section> cumbia_pool_sections = ExtractorHelper().extractCumbiaPoolSections(s);
    m_err = cumbia_pool_sections.size() != 1;
    qDebug() << __PRETTY_FUNCTION__ << "MainCppCodeExtractor m_err" << m_err << "going to extract exec app loop sections size" << cumbia_pool_sections.size();
    if(!m_err)
    {
        cumbia_pool_sections[0].where = Section::Includes;
        sections.append(cumbia_pool_sections[0]);
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

        qDebug() << __PRETTY_FUNCTION__ << "MainCppCodeExtractor pos" << pos << "captureth" <<  qapp_exec_re.cap(1);
        if(pos > -1) {
            sections.push_back(Section(ExtractorHelper().newline_wrap(qapp_exec_re.cap(1)), Section::EndOfMain));

        }
    }
    else
        m_msg = "MainCppCodeExtractor.extract: missing \"CumbiaPool\" section in main.cpp template file";
    return sections;
}


CodeExtractorA::Type MainWidgetCppCodeExtractor::type() const
{
    return CodeExtractorA::MainWCpp;
}

// extract this sections from the template
// 1 (start of constructor)
// (//\s+cumbia\n*(.*:?)// cumbia)
// cumbia
//CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
//ui->setupUi(this, cu_pool, m_ctrl_factory_pool);
// ...
// cumbia
//
// 2 (end of constructor)
// new CuContextActionBridge(this, cu_t, cu_tango_r_fac);
QList<Section> MainWidgetCppCodeExtractor::extract(Type t)
{
    QList<Section> sections;
    QString s = m_get_file_contents(t); // sets m_err
    qDebug() << __PRETTY_FUNCTION__ << "enter type " << t << "error flag" << m_err;
    if(m_err) // error opening template file
        return sections;

    // extraction 1
    QList<Section> cu_t_secs = ExtractorHelper().extractCumbiaPoolSections(s);
    m_err = cu_t_secs.size() != 2;
    if(m_err)
        m_msg = "MainWidgetCppCodeExtractor::extract: found " + QString::number(cu_t_secs.size()) +
                " cumbia sections in template file. Must be 2";
    if(!m_err) {
        cu_t_secs[0].where = Section::Includes;
        cu_t_secs[1].where = Section::StartOfCppConstructor;
        sections.append(cu_t_secs);
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
    QList<Section> cu_t_secs = ExtractorHelper().extractCumbiaPoolSections(s);
    m_err = cu_t_secs.size() != 2;
    if(m_err)
        m_msg = "MainWidgetCppCodeExtractor::extract: found " + QString::number(cu_t_secs.size()) +
                " cumbia sections in template file. Must be 2";
    else {
        cu_t_secs[0].where = Section::Includes;
        cu_t_secs[1].where = Section::EndOfHConstructor;
        sections.append(cu_t_secs);
    }

    qDebug() << __PRETTY_FUNCTION__ << "extraction result from H file" << cu_t_secs.size();
    return sections;
}

