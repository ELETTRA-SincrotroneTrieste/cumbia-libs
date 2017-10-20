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

        foreach(QString s, m_widgetmap.keys())
            printf("%s:\n%s\n", s.toStdString().c_str(), m_widgetmap[s].toString().toStdString().c_str());
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
                QStringList list;
                QString facname = defel.attribute("factory");
                QDomNodeList params_nl = defel.childNodes();
                for(int l = 0; l < params_nl.size() && !m_error; l++)
                {
                    QDomElement param = params_nl.at(l).toElement();
                    if(!param.isNull() && param.tagName() == "param")
                    {
                        if(param.hasAttribute("add"))
                            list << (param.attribute("add"));
                        else
                        {
                            m_error = true;
                            m_lastError = "Defs.m_getParams: tag \"param\" must have both \"add\" and \"pos\" attributes, line " + QString::number(param.lineNumber());
                        }
                    }
                }

                if(!facname.isEmpty() && !list.isEmpty() && !m_error)
                    params.map[facname] = list; // must be ordered by pos
            }
        }
    }
}

void Defs::setDebug(bool db)
{
    m_debug = db;
}

QMap<QString, Expand> Defs::getWidgetMap() const
{
    return m_widgetmap;
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
        return m_error;
    }
    m_error = !doc.setContent(&file, &m_lastError);
    file.close();
    if(m_error) {
        m_lastError = "Defs.loadXmlConf: error parsing document: " + m_lastError;
        return m_error;
    }

    QDomNodeList srcdirs_node = doc.elementsByTagName("srcdirs");
    m_error = (srcdirs_node.size() != 1);
    if(m_error)
    {
        m_lastError = "Defs.loadXmlConf: there must be only one \"srcdirs\" node";
        return m_error;
    }
    else
    {
        QDomNodeList srcnodes = srcdirs_node.at(0).childNodes();
        for(int i = 0; i < srcnodes.size(); i++)
        {
            QDomElement srcel = srcnodes.at(i).toElement();
            if(!srcel.isNull() && srcel.tagName() == "srcdir" && srcel.hasAttribute("name") && srcel.hasAttribute("filters"))
                m_srcd_infoset.add(srcel.attribute("name"), srcel.attribute("filters"), SearchDirInfoSet::Source);
            else if(!srcel.isNull() &&  srcel.tagName() == "uidir" && srcel.hasAttribute("name") && srcel.hasAttribute("filters"))
                m_srcd_infoset.add(srcel.attribute("name"), srcel.attribute("filters"), SearchDirInfoSet::Ui);
            else if(!srcel.isNull() &&  srcel.tagName() == "uihdir" && srcel.hasAttribute("name") && srcel.hasAttribute("filters"))
                m_srcd_infoset.add(srcel.attribute("name"), srcel.attribute("filters"), SearchDirInfoSet::Ui_H);
            else if(!srcel.isNull())
            {
                m_error = true;
                m_lastError = "Defs.loadXmlConf: \"srcdir\", \"uidir\" and \"uihdir\" nodes must have the \"name\" and \"filters\" attributes";
                return m_error;
            }

        }
    }

    QDomNodeList factories_el = doc.elementsByTagName("factories");
    m_error = (factories_el.size() != 1);
    if(m_error)
    {
        m_lastError = "Defs.loadXmlConf: there must be only one \"factories\" node";
        return m_error;
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
        // default parameter expansion
        QDomNodeList defaults = doc.elementsByTagName("defaults");
        if(defaults.size() == 1)
        {
            m_getParams(defaults.at(0), m_default_pars);
            if(m_error) // m_getParams issues an error
                return false;
        }
        else
        {
            m_error = true;
            m_lastError = "Defs.loadConf: there must be one (and only one) \"defaults\" node";
        }

        if(m_error)
            return false;

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
                        m_widgetmap[nam] = Expand(nam, custom_pars, false);
                    else if(!nam.isEmpty() && m_default_pars.isValid())
                        m_widgetmap[nam] = Expand(nam, m_default_pars, false);

                    if(m_widgetmap.isEmpty())
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

        if(m_error)
            return false;
    }
    return true;
}

bool Defs::loadLocalConf(const QString &fname)
{
    QFile file(fname);
    m_error = !file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(m_error) {
        m_lastError = "Defs.loadConf: " + file.errorString();
        return m_error;
    }
    while (!file.atEnd()) {
          QByteArray line = file.readLine().replace("\n", "");
          m_widgetmap[line] = Expand(line, m_default_pars, true);
    }
    if(m_error) {
        m_lastError = "Defs.loadConf: error parsing document: " + m_lastError;
        return m_error;
    }
    return true;
}
