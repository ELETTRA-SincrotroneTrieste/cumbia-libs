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

class Par
{
public:
    Par(const QString& n, const QString& t = "")
    {
        classname = n; // the class name only: CuTReaderFactory
        pardef = t; // the class name and the parameter type definition, e.g: const CuTReaderFactory&
    }

    Par() { }

    bool isValid() const { return classname != "" ; }

    QString classname; // class name: ClassName
    QString pardef; // parameter definition: const ClassName&

    QString toString() const
    {
        return "Par { name: " + classname + ", parameter: \"" + pardef + "\" }";
    }
};

class Params
{
public:
    Params() {}


    void add(const QString& mode, const QList<Par>& p)
    {
        m_map[mode] = p;
    }

    // returns the parameter type for the given mode (factory) and the parameter name
    QString getType(const QString& mode, const QString& name)
    {
        const QList<Par> &lp = m_map[mode];
        foreach(Par p, lp)
            if(p.classname == name)
                return p.pardef;
        return QString();
    }

    // returns the parameter name for the given mode (factory) and parameter type
    QString getName(const QString& mode, const QString& type)
    {
        const QList<Par> &lp = m_map[mode];
        foreach(Par p, lp)
            if(p.pardef == type)
                return p.classname;
        return QString();
    }

    QList<Par> getParList(const QString& mode) const { return m_map[mode]; }

    bool isValid() const {
        return m_map.size() > 0;
    }

    QString toString() const {
        QString s;
        s += "Params:\n";
        foreach(QString nam, m_map.keys())
        {
            s += "\n\t- factory: " + nam;
            const QList<Par> &lp = m_map[nam];
            foreach(Par p, lp)
                s += "\n\t\t: " + p.toString();
        }
        s += "\n";
        return s;
    }

private:
    QMap<QString, QList<Par> > m_map;
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
        object = wnam;
        params = par;
        m_fromLocalConf = from_local_file;
        m_method = false;
    }

    Expand(const QString& wnam, const Params& par, bool from_local_file, bool method)
    {
        object = wnam;
        params = par;
        m_fromLocalConf = from_local_file;
        m_method = method;
    }

    Expand() {}


    QString object;
    Params params;

    void setAutoDetectRegexp(const QString& pattern) { m_autoDetect_regexp = pattern; }

    QString autoDetectRegexp() const { return m_autoDetect_regexp; }

    void setAutoDetect(bool ad) { m_autoDetect = ad; }

    bool autoDetect() const { return m_autoDetect; }

    bool fromLocalConf() const { return m_fromLocalConf; }

    bool isMethod() const { return m_method; }

    QString toString() const {
        QString s;
        s += "Expand: object \"" + object + "\" is method: " + QString::number(m_method) + " must add the following parameters to the constructor:\n";
        s += params.toString();
        s += "\n";
        return s;
    }

private:
    bool m_fromLocalConf, m_method;
    bool m_autoDetect;

    QString m_autoDetect_regexp;

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
        if(t == Ui) {
            QList<SearchDirInfo> dis = m_map.values(t);
            foreach(SearchDirInfo di, dis)
                 qDebug() << __FUNCTION__ << di.name();
        }
        return m_map.values(t);
    }

    bool isEmpty() const { return m_map.isEmpty() ; }

private:
    QMultiMap<Type, SearchDirInfo> m_map;
};

class CustomClass
{
public:
    CustomClass() {}

    CustomClass(const QString& classnam, const QString& classtype) {
        class_name = classnam;
        class_type = classtype; // customreader, customwriter, customrw
    }

    QString class_name, class_type;
};

class Defs
{
public:

    Defs();

    bool loadConf(const QString& default_conf, const QString &localfname);

    bool load(const QString& fname);

    bool guessFromSources();

    QString lastError() const;

    bool error() const;

    void m_getParams(const QDomNode& parent, Params &params);

    void setDebug(bool db);

    // get Expand from widget name
    QMap<QString, Expand> getObjectMap() const;

    QList<Search > getSearchList() const;

    SearchDirInfoSet srcDirsInfo() const;

    QStringList messages;

private:

    QString m_loadFile(const QString& path);

    bool loadXmlConf(const QString& fname);

    bool loadLocalConf(const QString& fname);

    QMap<QString, Expand> m_objectmap;

    QList<Search> m_searchlist;

    QList<CustomClass> m_customClasses;

    SearchDirInfoSet m_srcd_infoset;

    QString m_lastError;
    bool m_error;

    bool m_debug;
};

#endif // DEFS_H
