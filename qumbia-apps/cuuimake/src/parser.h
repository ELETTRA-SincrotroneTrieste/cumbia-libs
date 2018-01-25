#ifndef MODEDETECTOR_H
#define MODEDETECTOR_H

#include <QStringList>
#include "defs.h"



class Parser
{
public:
    Parser(const QString &force_mode = "");

    QString detect(const SearchDirInfoSet &dirInfoSet, const QList<Search> &searchlist, Substitutions &subs);

    bool error() const;

    QString lastError() const;

    void setDebug(bool dbg) ;

    const char *tos(const QString& s);

    bool find_match(const QString& contents, const QString& classname, QString &varname) const;

private:
    bool m_error;
    bool m_debug;
    QString m_lastError;
    QString m_forceMode;
};

#endif // MODEDETECTOR_H
