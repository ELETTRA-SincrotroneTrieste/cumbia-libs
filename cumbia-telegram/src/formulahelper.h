#ifndef FORMULAHELPER_H
#define FORMULAHELPER_H

#include <QStringList>
#include <vector>

class FormulaHelper
{
public:
    FormulaHelper();

    FormulaHelper(const QString& formula);

    bool requiresLeftOperand() const;

    QString formula() const;

    QString replaceWildcards(const std::vector<double> &v);

    QString replaceWildcards(const double d);

    QString escape(const QString& formula) const;

private:
    QString m_formula;
};

#endif // FORMULAHELPER_H
