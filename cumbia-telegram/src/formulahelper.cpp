#include "formulahelper.h"
#include <QStringList>

FormulaHelper::FormulaHelper()
{

}

FormulaHelper::FormulaHelper(const QString &formula)
{
    m_formula = formula;
}

/**
 * @brief FormulaHelper::requiresLeftOperand returns true if the formula is not valid unless
 *        an operand is provided at its left. For example the formula " / 2 * 3" requires a
 *        number before division, while the formula  sqrt(16) does not
 *
 * @return true if the formula requires to prepend a number before the start, false otherwise
 */
bool FormulaHelper::requiresLeftOperand() const
{
    QStringList operators = QStringList() << "=" << "-" << "+"
                                          << "/" << "*" << "!="
                                          << "<" << "<=" << ">"
                                          << ">=";
    QString ft = m_formula.trimmed();
    foreach(QString op, operators)
        if(ft.startsWith(op))
            return true;
    return false;
}

QString FormulaHelper::formula() const
{
    return m_formula;
}

QString FormulaHelper::replaceWildcards(const std::vector<double> &v)
{
    QString s = m_formula;
    for(size_t i = 0; i < v.size(); i++)
        s.replace(QString("$%1").arg(i), QString::number(v[i]));
    return s;
}

QString FormulaHelper::replaceWildcards(const double d)
{
    QString s = m_formula;
    s.replace("$0", QString::number(d)).replace("$", QString::number(d));
    return s;
}

// telegram does not like "<" symbol and its &lt; representation either
//
QString FormulaHelper::escape(const QString &formula) const
{
    QString s(formula);
    // "< something" or " < something", but not "<something" which could be a tag
    s.replace("+", "%26#43;"); // + sign
    s.replace("<", "%26#60; "); // < sign
    s.replace(">", "%26#62;"); // > sign
    s.replace("?", "%26#63;");
    s.replace("&", "%26#38;");
    return s;
}
