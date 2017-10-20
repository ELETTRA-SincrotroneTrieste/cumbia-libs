#ifndef DEFS_H
#define DEFS_H

#include <QString>
#include <QStringList>
#include <QDomNode>
#include <QMap>
#include <QMultiMap>
#include <QtDebug>

#define qstoc(x) 			 	x.toStdString().c_str()

class Search
{
public:
    Search(const QString& fa) { factory = fa; }

    QStringList searches;
    QString factory;

    QString toString() const {
        QString s;
        s += "Search: factory \"" + factory + "\" must have the following definitions:\n";
        foreach(QString se, searches)
            s += "\n\t- " + se;
        s += "\n";
        return s;
    }
};

class Params
{
public:
    Params() {}

    QMap<QString, QStringList> map;

    bool isValid() const {
        return map.size() > 0;
    }

    QString toString() const {
        QString s;
        s += "Params:\n";
        foreach(QString nam, map.keys())
        {
            s += "\n\t- factory: " + nam;
            foreach(QString p, map[nam])
                s += "\n\t\t: " + p;
        }
        s += "\n";
        return s;
    }
};

class Substitutions
{
public:


    void selectMode(const QString& m) {
        mode = m;
    }

    QString selectedMode() const {
        return mode;
    }

    void insertSubstitution(const QString& className, const QString& var_name) {
        subst_map.insert(className, var_name);
    }

    QString getSubstitution(const QString& className) const {
        return subst_map.value(className);
    }

    QString toString() const {
        QString s;
        s += " selected mode: " + mode;
        s += " subst.: ";
        foreach(QString key, subst_map.keys())
            s += key + " -> " + subst_map[key] + "   ";
        return s;
    }

private:
    QMap<QString, QString> subst_map;
    QString mode; // cumbia, cumbiapool
};

class Expand
{
public:
    Expand(const QString& wnam, const Params& par, bool from_local_file)
    {
        widget = wnam;
        params = par;
        m_fromLocalConf = from_local_file;
    }

    Expand() {}


    QString widget;
    Params params;

    bool fromLocalConf() const { return m_fromLocalConf; }

    QString toString() const {
        QString s;
        s += "Expand: widget \"" + widget + "\" must add the following parameters to the constructor:\n";
        s += params.toString();
        s += "\n";
        return s;
    }

private:
    bool m_fromLocalConf;

};

class SearchDirInfo
{
public:
    SearchDirInfo(const QString& dirnam, const QString& filters)
    {
        m_name = dirnam;
        m_filters = filters.split(QRegExp("\\s+"));
    }

    QString name() const { return m_name; }
    QStringList filters() const { return m_filters;}

    SearchDirInfo() {}

    QString toString() const
    {
        QString s = "\t- SearchDirInfo: ";
        s += "\t will look look for: ";
        foreach(QString f, m_filters)
            s += "\"" +f + "\"   ";
        s += "files under the dir \"" + m_name + "\"";
        return s;
    }

private:
    QString m_name;
    QStringList m_filters;
};

class SearchDirInfoSet
{
public:
    enum Type { Source, Ui, Ui_H };

    SearchDirInfoSet() {}

    void add(const QString& dirnm, const QString& filters, Type t)
    {
        m_map.insert(t, SearchDirInfo(dirnm, filters));
    }

    QList<SearchDirInfo> getDirInfoList(Type t) const
    {
        return m_map.values(t);
    }

    bool isEmpty() const { return m_map.isEmpty() ; }

private:
    QMultiMap<Type, SearchDirInfo> m_map;
};

class Defs
{
public:
    Defs();

    bool loadConf(const QString& default_conf, const QString &localfname);

    bool load(const QString& fname);

    QString lastError() const;

    bool error() const;

    void m_getParams(const QDomNode& parent, Params &params);

    void setDebug(bool db);

    QMap<QString, Expand> getWidgetMap() const;

    QList<Search > getSearchList() const;

    SearchDirInfoSet srcDirsInfo() const;

private:

    bool loadXmlConf(const QString& fname);

    bool loadLocalConf(const QString& fname);

    QMap<QString, Expand> m_widgetmap;

    Params m_default_pars;

    QList<Search> m_searchlist;

    SearchDirInfoSet m_srcd_infoset;

    QString m_lastError;
    bool m_error;

    bool m_debug;
};

#endif // DEFS_H
