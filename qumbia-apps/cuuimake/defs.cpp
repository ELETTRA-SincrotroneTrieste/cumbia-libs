#include "defs.h"
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include <QtDebug>
#include <QString>
#include "conf.h"

Defs::Defs()
{
    m_error = false;
    m_lastError = "";
}

bool Defs::loadConf(const QString &default_conf, const QString& localfname)
{
    bool success = loadXmlConf(default_conf);
    if(success && !localfname.isEmpty())
        success = loadLocalConf(localfname);

    if(m_debug)
    {
        foreach(Search s, m_searchlist)
            printf("Search analysis parameters: %s\n", s.toString().toStdString().c_str());

        foreach(QString s, m_objectmap.keys())
            printf("%s:\n%s\n", s.toStdString().c_str(), m_objectmap[s].toString().toStdString().c_str());
    }

    return success;
}

bool Defs::load(const QString &fname)
{

}

QString Defs::lastError() const
{
    return m_lastError;
}

bool Defs::error() const
{
    return m_error;
}

void Defs::m_getParams(const QDomNode &parent, Params& params)
{
    QDomElement parentEl = parent.toElement();
    if(!parentEl.isNull())
    {
        QDomNodeList nld = parentEl.childNodes();
        for(int k = 0; k < nld.size() && !m_error; k++)
        {
            QDomElement defel = nld.at(k).toElement();
            if(!defel.isNull() && defel.tagName() == "params")
            {
                QList<Par> parlist;
                QString facname = defel.attribute("factory");
                QDomNodeList params_nl = defel.childNodes();
                for(int l = 0; l < params_nl.size() && !m_error; l++)
                {
                    QDomElement param = params_nl.at(l).toElement();
                    if(!param.isNull() && param.tagName() == "param")
                    {
                        if(param.hasAttribute("add"))
                        {
                            Par p(param.attribute("add"));
                            if(param.hasAttribute("definition"))
                                p.pardef = param.attribute("definition");
                            parlist << p;
                        }
                        else
                        {
                            m_error = true;
                            m_lastError = "Defs.m_getParams: tag \"param\" must have both \"add\" and \"pos\" attributes, line " + QString::number(param.lineNumber());
                        }
                    }
                }

                if(!facname.isEmpty() && !parlist.isEmpty() && !m_error)
                    params.add(facname, parlist);
            }
        }
    }
}

void Defs::setDebug(bool db)
{
    m_debug = db;
}

QMap<QString, Expand> Defs::getObjectMap() const
{
    return m_objectmap;
}

QList<Search> Defs::getSearchList() const
{
    return m_searchlist;
}

SearchDirInfoSet Defs::srcDirsInfo() const
{
    return m_srcd_infoset;
}

bool Defs::loadXmlConf(const QString &fname)
{
    QDomDocument doc("defaultconf");
    QFile file(fname);
    m_error = !file.open(QIODevice::ReadOnly);
    if(m_error) {
        m_lastError = "Defs.loadXmlConf: " + file.errorString();
        return false;
    }
    m_error = !doc.setContent(&file, &m_lastError);
    file.close();
    if(m_error) {
        m_lastError = "Defs.loadXmlConf: error parsing document: " + m_lastError;
        return false;
    }

    QDomNodeList srcdirs_node = doc.elementsByTagName("srcdirs");
    m_error = (srcdirs_node.size() != 1);
    if(m_error)
    {
        m_lastError = "Defs.loadXmlConf: there must be only one \"srcdirs\" node";
        return false;
    }
    else
    {
        QDomNodeList srcnodes = srcdirs_node.at(0).childNodes();
        QDomElement srcel;
        for(int i = 0; i < srcnodes.size(); i++)
        {
            srcel = srcnodes.at(i).toElement();
            if(!srcel.isNull() && srcel.tagName() == "srcdir" && srcel.hasAttribute("name") && srcel.hasAttribute("filters"))
                m_srcd_infoset.add(srcel.attribute("name"), srcel.attribute("filters"), SearchDirInfoSet::Source);
            else if(!srcel.isNull() &&  srcel.tagName() == "uidir" && srcel.hasAttribute("name") && srcel.hasAttribute("filters"))
                m_srcd_infoset.add(srcel.attribute("name"), srcel.attribute("filters"), SearchDirInfoSet::Ui);
            else if(!srcel.isNull() &&  srcel.tagName() == "uihdir" && srcel.hasAttribute("name") && srcel.hasAttribute("filters"))
                m_srcd_infoset.add(srcel.attribute("name"), srcel.attribute("filters"), SearchDirInfoSet::Ui_H);
            else if(!srcel.isNull())
            {
                m_error = true;
                m_lastError = "Defs.loadXmlConf: \"srcdir\", \"uidir\" and \"uihdir\" "
                              "nodes must have the \"name\" and \"filters\" attributes, line: " + QString::number(srcel.lineNumber());
                return false;
            }

        }
        m_error = (m_srcd_infoset.getDirInfoList(SearchDirInfoSet::Ui_H).size() != 1);
        if(m_error)
        {
            m_lastError = "Defs.loadXmlConf: there must be one and only one \"uihdir\" node, line: " + QString::number(srcel.lineNumber());
            return false;
        }
    }

    QDomNodeList factories_el = doc.elementsByTagName("factories");
    m_error = (factories_el.size() != 1);
    if(m_error)
    {
        m_lastError = "Defs.loadXmlConf: there must be only one \"factories\" node";
        return false;
    }
    QDomNodeList factories = factories_el.at(0).childNodes();
    for(int i = 0; i < factories.size(); i++)
    {
        QDomNode node = factories.at(i);
        QDomElement factory = node.toElement();
        if(!factory.isNull() && factory.tagName() == "factory" && factory.hasAttribute("name"))
        {
            Search s(factory.attribute("name"));
            QDomNodeList findnl = factory.elementsByTagName("find");
            for(int j = 0; j < findnl.size(); j++)
            {
                QDomElement findel = findnl.at(j).toElement();
                if(!findel.isNull() && !findel.attribute("declaration").isEmpty())
                    s.searches << findel.attribute("declaration");
            }
            if(!s.factory.isEmpty() && !s.searches.isEmpty())
                m_searchlist << s;
        }
        else if(!factory.isNull())
        {
            m_error = true;
            m_lastError = "Defs.loadConf: factory element \"" + factory.tagName() + "\"" +  " null or without \"name\" attribute, line " + QString::number(node.lineNumber());
        }
        // comments are ok
    }
    if(m_searchlist.isEmpty())
    {
        m_error = true;
        m_lastError = "Defs.loadConf: no valid factory definitions found, line " + QString::number(factories_el.at(0).lineNumber());
    }
    else
    {
        QDomNodeList widgetsnl = doc.elementsByTagName("widgets");
        if(widgetsnl.size() == 1)
        {
            QDomElement widgets_el = widgetsnl.at(0).toElement();
            QDomNodeList widgetlist = widgets_el.elementsByTagName("widget");
            for(int w = 0; w < widgetlist.size(); w++)
            {
                QDomElement widget = widgetlist.at(w).toElement();

                if(!widget.isNull() && widget.hasAttribute("class"))
                {
                    QString nam = widget.attribute("class");
                    Params custom_pars;
                    m_getParams(widget, custom_pars);
                    if(m_error)
                        return false;
                    if(custom_pars.isValid())
                        m_objectmap[nam] = Expand(nam, custom_pars, false);

                    if(m_objectmap.isEmpty())
                    {
                        m_error = true;
                        m_lastError = "Defs.loadConf: no valid \"widget\" configurations";
                    }
                }
            }
        }
        else
        {
            m_error = true;
            m_lastError = "Defs.loadConf: there must be one (and only one) \"widgets\" node";
        }
        QDomNodeList methodsnl = doc.elementsByTagName("methods");
        if(methodsnl.size() == 1)
        {
            QDomElement methods_el = methodsnl.at(0).toElement();
            for(int i = 0; i < methods_el.childNodes().count(); i++)
            {
                QDomElement method = methods_el.childNodes().at(i).toElement();
                if(!method.isNull() && method.hasAttribute("name"))
                {
                    QString methodnam = method.attribute("name");
                    Params methodParams;
                    m_getParams(method, methodParams);
                    if(m_error)
                        return false;
                    if(methodParams.isValid())
                        m_objectmap[methodnam] = Expand(methodnam, methodParams, false, true);
                }
            }

        }
        else
        {
            m_error = true;
            m_lastError = "Defs.loadConf: there must be one (and only one) \"methods\" node";
        }

        if(m_error)
            return false;
    }
    return true;
}

/** \brief loads configuration from local files, within the project top level directory where cuuimake
 *         is run.
 *
 * \par example of file contents
 *
 * \code
 * QuInputOutput,cumbiatango,CumbiaTango *,CuTReaderFactory,CuTWriterFactory
 * \endcode
 */
bool Defs::loadLocalConf(const QString &fname)
{
    QFile file(fname);
    m_error = !file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(m_error) {
        m_lastError = "Defs.loadConf: " + file.errorString();
        return false;
    }
    Params pars;
    while (!file.atEnd()) {
        QString line = QString(file.readLine().replace("\n", ""));
        line = line.trimmed();
        QString objectname;
        if(!line.startsWith("#"))
        {
            QStringList parts = line.split(QRegExp("\\s*,\\s*"));
            if(parts.size() > 2)
            {
                QString facname = parts.at(1);
                objectname = parts.at(0);
                QList<Par> parlist;
                for(int i = 2; i < parts.size(); i++)
                {
                    Par p(parts.at(i));
                    parlist << p;
                }
                if(parlist.size() > 0)
                    pars.add(facname, parlist);
            }
            m_objectmap[objectname] = Expand(objectname, pars, true);
        }
    }
    if(m_error) {
        m_lastError = "Defs.loadConf: error parsing document: " + m_lastError;
        return false;
    }
    return true;
}
