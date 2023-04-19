#ifndef CUFORMULAUTILS_H
#define CUFORMULAUTILS_H

#include <QStringList>

class CuFormulaUtils
{
public:

    QString replaceWildcards(const QString &s, const QStringList &args) const;
};

#endif // CUFORMULAUTILS_H
