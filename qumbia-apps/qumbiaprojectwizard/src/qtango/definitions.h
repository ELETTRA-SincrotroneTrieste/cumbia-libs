#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QList>
#include <QString>

class Quality
{
  public:
    enum Level { Ok, Warn, Critical };

    Quality(const QString& _ok) {
        if(_ok == "ok") quality = Ok;
        else if(_ok == "warn") quality = Warn;
        else quality = Critical;
    }

    Level quality;
};

class ReplaceLine : public Quality
{
public:
    ReplaceLine(const QString& reg, const QString& repl, const QString& filenam, const QString& comm, const QString& _ok)
        : Quality(_ok)
    {
        regexp = reg;
        replacement = repl;
        filename = filenam;
        comment = comm;
    }

    QString regexp, replacement, filename, comment;
};

class Subst : public Quality
{
  public:

    Subst(const QString& name, const QString& inc, const QString& comment, const QString& _ok) : Quality(_ok) {
        i_name = name;
        i_inc = inc;
        i_comment = comment;
    }

    Subst(const QString& name, const QString& new_cl,
          const QString& inc, const QString& comment,
          const QString& _ok)
        : Quality(_ok)
    {
        i_name = name;
        i_class = new_cl;
        i_comment = comment;
        i_inc = inc;
    }

    QString i_name, i_inc, i_comment, i_class;

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


    QList<Subst> getIncludeSubst() {
        return m_includes;
    }

    QList<Subst> getClassSubst() {
        return m_classes;
    }

    QList<ReplaceLine> getReplaceLines() {
        return m_replacelines;
    }

    bool error () const {
        return m_err;
    }

    QString errorMessage() const {
        return m_msg;
    }

private:
    QList<Subst> m_includes, m_classes;
    QList<ReplaceLine> m_replacelines;
    QString m_msg;
    bool m_err;

};

#endif // DEFINITIONS_H
