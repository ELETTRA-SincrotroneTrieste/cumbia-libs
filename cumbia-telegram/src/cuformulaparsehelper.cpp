#include "cuformulaparsehelper.h"
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QtDebug>
#include <cumacros.h>

CuFormulaParseHelper::CuFormulaParseHelper()
{
    const char* tg_host_pattern_p = "(?:[A-Za-z0-9\\.\\-_]+:[\\d]+/){0,1}";
    const char* tg_pattern_p = "[A-Za-z0-9\\.\\-_:]+";
    QString tg_att_pattern = QString("%1%2/%2/%2/%2").arg(tg_host_pattern_p).arg(tg_pattern_p);
    m_src_patterns << tg_att_pattern;
}

bool CuFormulaParseHelper::isNormalizedForm(const QString &f, const QString& norm_pattern) const
{
    QRegularExpression re(norm_pattern);
    QString e(f);
    e.remove("formula://");
    QRegularExpressionMatch match = re.match(e);
    printf("\e[1;35mIsNormalised form %d pattern is \"%s\" %s\e[0m\n", match.hasMatch() , qstoc(norm_pattern),
           qstoc(f));
    return match.hasMatch();
}

QString CuFormulaParseHelper::toNormalizedForm(const QString &f) const
{
    QString norm(f);
    norm.remove("formula://");
    QString pattern = m_buildSrcPattern();
    QRegularExpression re(pattern);
    QRegularExpressionMatchIterator i = re.globalMatch(f);
    QStringList srcs;
    while(i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString src = match.captured(1);
        if(!srcs.contains(src))
            srcs << src;
    }

    if(srcs.size() > 0) {

        QString function_decl;
        QString function_body(f);

        QString params;
        norm = "formula://{" + srcs.join(",") + "}";
        // now build function arguments
        char c = 'a', maxc = 'Z';
        for(char i = 0; i < srcs.size() && i < maxc ; i++) {
            c = c + static_cast<char>(i);
            i < srcs.size() - 1 ? params += QString("%1,").arg(c) : params += QString("%1").arg(c);
            function_body.replace(srcs[i], QString(c));
        }
        function_decl = QString("function (%1)").arg(params);
        function_body = QString( "{"
                                 "return %1"
                                 "}").arg(function_body);

        norm += function_decl + function_body;
        qDebug() << __PRETTY_FUNCTION__ << "pattern" << pattern <<  "matched sources " << srcs;
        qDebug() << __PRETTY_FUNCTION__ <<  "normalized to"     << norm;
    }
    else {
        if(!norm.startsWith("formula://"))
            norm = "formula://" + norm;
    }

    return norm;
}

QString CuFormulaParseHelper::injectHost(const QString &host, const QString &src)
{
    QString s(src);
    if(!host.isEmpty()) {
        QString src_section;
        // (\{.+\}).+
        // example {test/device/1/double_scalar} function (a){return 2 + a}
        QString srclist_section_pattern = "(\\{.+\\}).+";
        QRegularExpression re_srcs_section(srclist_section_pattern);
        QRegularExpressionMatch match = re_srcs_section.match(src);
        if(match.hasMatch()) {
            const QString srclist = match.captured(1);
            // in the example above captured(1): {test/device/1/double_scalar}
            QRegularExpression re(m_buildSrcPattern());
            QRegularExpressionMatchIterator i = re.globalMatch(srclist);
            QStringList srcs;
            while(i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                QString src = host + "/" + match.captured(1);
                if(!srcs.contains(src))
                    srcs << src;
            }
            QString s_inj = srcs.join(",");
            s.replace(srclist, "{" + s_inj + "}");
        }

    }
    qDebug() << __PRETTY_FUNCTION__ <<  "source with host (" << host << ") becomes " << s;
    return s;
}

/**
 * @brief CuFormulaParseHelper::sources extract sources from a formula
 * @param srcformula a source (also not in normal form {src1,src2}(@0 + @1) )
 * @return list of detected formulas according to the source patterns
 */
QStringList CuFormulaParseHelper::sources(const QString &srcformula) const
{
    QString pattern = m_buildSrcPattern();
    QRegularExpression re(pattern);
    QRegularExpressionMatchIterator i = re.globalMatch(srcformula);
    QStringList srcs;
    while(i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString src = match.captured(1);
        if(!srcs.contains(src))
            srcs << src;
    }
    return srcs;
}

QStringList CuFormulaParseHelper::srcPatterns() const
{
    return m_src_patterns;
}

void CuFormulaParseHelper::setSrcPatterns(const QStringList &srcp)
{
    m_src_patterns = srcp;
}

void CuFormulaParseHelper::addSrcPattern(const QString &p)
{
    m_src_patterns << p;
}

/**
 * @brief CuFormulaParseHelper::sourceMatch finds if one source matches the given patter (regexp match)
 * @param pattern the regexp pattern
 * @return true if *one of* the sources matches pattern
 *
 * \par Example
 * \code
 * reader->setSource("{test/device/1/long_scalar, test/device/2/double_scalar}(@1 + @2)");
 * CuFormulaParserHelper ph;
 * bool match = ph.sourceMatch(reader->source(), "double_scalar");
 * // match is true
 *
 * \endcode
 */
bool CuFormulaParseHelper::sourceMatch(const QString& src, const QString &pattern) const
{
    if(src == pattern)
        return true;
    QStringList srcs = sources(src);
    QRegularExpression re(pattern);
    foreach(QString s, srcs) {
        QRegularExpressionMatch match = re.match(s);
        if(match.hasMatch())
            return true;
    }
    return false;
}

bool CuFormulaParseHelper::identityFunction(const QString& expression) const
{
    // pattern for a function that does NOT imply formulas
    // function\s*\(a\)\s*\{\s*return\s+a\s*\}
    //
    QString pat = "function\\s*\\(a\\)\\s*\\{\\s*return\\s+a\\s*\\}";
    QRegularExpression re(pat);
    QRegularExpressionMatch match = re.match(expression);
    return match.hasMatch();
}

QString CuFormulaParseHelper::m_buildSrcPattern() const
{
    QString pattern = "((?:";
    for(int i = 0; i < m_src_patterns.size(); i++) {
        pattern += m_src_patterns[i];
        if(i < m_src_patterns.size() - 1)
            pattern += ")|(?:";
    }
    pattern += "))";
    return pattern;
}
