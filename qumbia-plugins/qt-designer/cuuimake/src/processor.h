#ifndef PROCESSOR_H
#define PROCESSOR_H

class Substitutions;

#include <QStringList>
#include <QMap>

class SearchDirInfoSet;

class Processor
{
public:
    Processor();

    void apply(const Substitutions& subs, const SearchDirInfoSet &dirInfoSet) const;

    QMap<QString, bool> findUI_H(const SearchDirInfoSet &dirInfoSet);

    void setDebug(bool d) { m_debug = d; }

private:
    bool m_debug;
};

#endif // PROCESSOR_H
