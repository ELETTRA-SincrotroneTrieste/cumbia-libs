#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class Quality
{
public:
    enum Level { Ok, Warn, Critical };

    Quality(const QString& _ok) {
        setOk(_ok);
    }

    Quality() {

    }

    void setOk(const QString& _ok)
    {
        if(_ok == "ok") quality = Ok;
        else if(_ok == "warn") quality = Warn;
        else quality = Critical;
    }

    Level quality;
};

class Subst : public Quality
{
public:

    enum Type { Undefined, ReplaceInclude, MapClass, ReplaceLine, ReplaceExpr,
              ExpandReaderInstantiation, ExpandWriterInstantiation };

    Subst(const QJsonObject& jo) {

        QString t = jo.value("type").toString();
        if(t == "replace include") m_type = ReplaceInclude;
        else if (t == "map class") m_type = MapClass;
        else if (t == "replace expr") m_type = ReplaceExpr;
        else if(t == "replace line") m_type = ReplaceLine;
        else if( t ==  "expand reader instantiation") m_type = ExpandReaderInstantiation;
        else if( t ==  "expand writer instantiation") m_type = ExpandWriterInstantiation;
        else m_type = Undefined;

        if(jo.contains("in")) m_in = jo.value("in").toString();
        if(jo.contains("out") && jo.value("out").isArray()) {
            QJsonArray out_array = jo.value("out").toArray();
            for(int i = 0; i < out_array.size(); i++)
                if(out_array.at(i).isString())
                    m_out_options << out_array.at(i).toString();
        }
        else if(jo.contains("out") && !jo.value("out").isArray())
            m_out_options << jo.value("out").toString();

        if(jo.contains("comment")) m_comment = jo.value("comment").toString();
        if(jo.contains("include")) m_include = jo.value("include").toString();
        if(jo.contains("file")) m_file = jo.value("file").toString();
        setOk(jo.value("level").toString());

    }
    QString typeStr() const {
        if(m_type == ReplaceLine) return "replace line";
        if(m_type == MapClass) return "map class";
        if(m_type == ReplaceExpr) return "replace expr";
        if(m_type == ReplaceInclude) return "replace include";
        if(m_type == ExpandReaderInstantiation) return "expand reader instantiation";
        if(m_type == ExpandWriterInstantiation) return "expand writer instantiation";
        return "unknown";
    }

    QString getFirstOutOption() const {
        if(m_out_options.size())
            return m_out_options.first();
        return QString();
    }

    QString m_in, m_comment, m_include, m_file;
    QStringList m_out_options;


    Type m_type;

};


/*! \brief load definitions from the qtango.keywords
 *
 * Substitutions:
 * \li includes TLabel --> qulabel.h
 * \li class names TLabel --> QuLabel, TLed --> QuLed...
 */
class Definitions
{
public:
    Definitions();

    bool load(const QString& filename);


    QList<Subst> getSubstitutions() {
        return m_substs;
    }

    bool error () const {
        return m_err;
    }

    QString errorMessage() const {
        return m_msg;
    }

private:
    QList<Subst> m_substs;
    QString m_msg;
    bool m_err;

};

#endif // DEFINITIONS_H
