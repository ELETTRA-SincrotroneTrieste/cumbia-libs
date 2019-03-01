#ifndef CUFORMULAPARSEHELPER_H
#define CUFORMULAPARSEHELPER_H

#include <QStringList>

class CuFormulaParseHelper
{
public:
    CuFormulaParseHelper();

    bool isNormalizedForm(const QString& f, const QString &norm_pattern) const;

    QString toNormalizedForm(const QString& f) const;

    QString injectHost(const QString& host, const QString& src);

    QStringList sources(const QString& formula) const;

    QStringList srcPatterns() const;

    void setSrcPatterns(const QStringList& srcp);

    void addSrcPattern(const QString& p);

    bool sourceMatch(const QString &src, const QString &pattern) const;

    bool identityFunction(const QString &expression) const;

private:
    QStringList m_src_patterns;

    QString m_buildSrcPattern() const;
};

#endif // CUFORMULAPARSEHELPER_H
