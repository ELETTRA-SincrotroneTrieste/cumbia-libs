#ifndef CONVERSIONDEFS_H
#define CONVERSIONDEFS_H

#include <QString>
#include <QList>

class Subst
{
  public:
    enum Quality { Ok, Warn, Critical };

    Subst(const QString& name, const QString& inc, const QString& comment, const QString& _ok) {
        i_name = name;
        i_inc = inc;
        i_comment = comment;
        if(_ok == "ok") quality = Ok;
        else if(_ok == "warn") quality = Warn;
        else quality = Critical;
    }

    Subst(const QString& name, const QString& new_cl,
          const QString& inc, const QString& comment,
          const QString& _ok) {
        i_name = name;
        i_class = new_cl;
        i_comment = comment;
        i_inc = inc;
        ok = _ok == "ok";
        if(_ok == "ok") quality = Ok;
        else if(_ok == "warn") quality = Warn;
        else quality = Critical;
    }

    QString i_name, i_inc, i_comment, i_class;
    bool ok;
    Quality quality;
};

class ReplaceQuality
{
public:
    ReplaceQuality(const QString& ty, const QString& old, const QString& ne,
                   const QString& comm, Subst::Quality q, unsigned line) {
        type = ty;
        old_e = old;
        new_e = ne;
        quality = q;
        comment = comm;
        lineno = line;
    }

    QString type, old_e, new_e, quality, comment;
    unsigned lineno;
};

class FindReplace
{
public:
    FindReplace();

    bool load(const QString& filename);

    QString replace(const QString& filename);

    QList<Subst> includes, classes;

    bool error() const { return m_err; }

    QString errorMessage() const { return m_msg; }

    QList<ReplaceQuality> log() const {
        return m_log;
    }

private:
    QString m_msg;
    bool m_err;
    QList<ReplaceQuality> m_log;

};

#endif // CONVERSIONDEFS_H
