#include "cuformulaparser.h"
#include <QRegularExpression>
#include <cumacros.h>

class CuFormulaParserPrivate {
public:
    std::vector<std::string> srcs;
    bool error;
    QString message;
    QString expression; // the whole expression from the source, given as input to parse
    QString formula;    // the formula extracted from the expression, either within enclosing () or not
    QString prepared_formula; // the formula on one line, trimmed, and enclosed in () ready to use
    QString normalized_pattern;
};

CuFormulaParser::CuFormulaParser()
{
    d = new CuFormulaParserPrivate;
    d->error = false;

    // regexp ^\{([\$A-Za-z0-9/,\._\-\:\s&\(\)>]+)\}[\s\n]*(function)?[\s\n]*\({0,1}([a-z,\s]+)\)[\s\n]*(.*)$
    // example formula:
    // {test/device/1/double_scalar test/device/1->DevDouble(10.1)} (a,b ){ return a +b; }
    // example 2
    // {test/device/1/double_scalar test/device/1->DevDouble(10.1)}  ( function(a, b) { return (a -sqrt(b)); })
    d->normalized_pattern = QString(FORMULA_RE);
}

CuFormulaParser::~CuFormulaParser()
{
    delete d;
}

bool CuFormulaParser::parse(const QString &expr)
{
    d->expression = expr;
    d->message = QString();
    bool mat;
    QRegularExpression re(d->normalized_pattern);
    QString e(d->expression);
    e.remove("formula://");
    e.remove("\n");
    QRegularExpressionMatch match = re.match(e);
    mat = match.hasMatch() && match.capturedTexts().size() > 2;
    printf("\e[1;25mDETECTED CAPTURES %d from expression \"%s\"\e[0m\n\n\n", match.capturedTexts().size(), qstoc(e));
    if(mat) {
        // four captures
        // 1 sources, comma or space or space + comma separated
        // 2 "function" word (optional, may be empty )
        // 3 parameter list, without parentheses, e.g. a, b are captured from (a,b)
        // 4 function body, including { }
        //
        // 1
        QString sources = match.captured(1);
        // 2
        QString function_word = match.captured(2);
        if(function_word.isEmpty())
            function_word = "function";
        QStringList slist;
        // split sources by space or comma
        sources.contains(QRegExp("[\\s*,\\s*]")) ?  slist = sources.split(QRegExp("[\\s*,\\s*]"), QString::SkipEmptyParts) :
                slist = sources.split(" ", QString::SkipEmptyParts);
        foreach(QString s, slist)
                d->srcs.push_back(s.toStdString());
        // 3
        QString params = match.captured(3).remove(QRegExp("\\s*"));
        QStringList paramList = params.split(",");
        d->error = (paramList.size() != static_cast<int>(d->srcs.size()));
        if(d->error)
            d->message = QString("CuFormulaParser.parse: parameter list count %1 in \"(%2)\""
                                 " does not match sources count (%3)")
                    .arg(paramList.size()).arg(params).arg(d->srcs.size());
        else {
            // 4
            QString functionBody = match.captured(4);
            d->formula = QString("function(%1) %2").arg(params).arg(functionBody);
            printf("\e[1;32mcorrectly detected formyula %s\e[0m\n", qstoc(d->formula));
        }
    }
    else {
        d->formula = e;
    }

    d->prepared_formula = m_makePreparedFormula();

    return !d->error;
}

size_t CuFormulaParser::sourcesCount() const
{
    return d->srcs.size();
}

std::vector<std::string> CuFormulaParser::sources() const
{
    return  d->srcs;
}

std::string CuFormulaParser::source(size_t i) const
{
    if(i < d->srcs.size())
        return d->srcs.at(i);
    perr("CuFormulaParser.source: index %ld out of range", i);
    return std::string();
}

/**
 * @brief CuFormulaParser::updateSource updates the source at position i with the new
 *        source name s
 * @param i the index of the source to be updated
 * @param s the new name
 *
 * This can be used to update sources after wildcards ("$1, $2) have been replaced by
 * engine specific readers.
 *
 * \par Example
 * CuFormulaReader calls updateSource after QuWatcher::setSource so that CuFormulaParser
 * contains the complete source name, after wildcards have been replaced by QuWatcher.
 *
 * @see CuFormulaReader::setSource
 *
 */
void CuFormulaParser::updateSource(size_t i, const std::string &s)
{
    printf("\e[1;35mupdateSource replacing source at idx %zu setting to %s srcs size %zu\e[0m\n",
           i, s.c_str(), d->srcs.size());
    if(i < d->srcs.size())
        d->srcs[i] = s;
}

long int CuFormulaParser::indexOf(const std::string &src) const
{
    printf("\e[1;33mfinding %s in\n", src.c_str());
    for(int i = 0; i < d->srcs.size(); i++)
        printf("   - %s\n", d->srcs[i].c_str());
    printf("\e[0m\n");
    long int pos = std::find(d->srcs.begin(), d->srcs.end(), src) - d->srcs.begin();
    if(pos >= static_cast<long>(d->srcs.size()))
        return -1;
    return pos;
}

QString CuFormulaParser::expression() const
{
    return d->expression;
}

QString CuFormulaParser::formula() const
{
    return d->formula;
}

QString CuFormulaParser::preparedFormula() const
{
    printf("CuFormulaParser::preparedFormula \e[1;31mPREPARED FORMULA RETURNING %s\e[0m\n", qstoc(d->prepared_formula));
    return d->prepared_formula;
}

QString CuFormulaParser::m_makePreparedFormula() const
{
    QString em(d->formula.trimmed());
    // ^\(.+\)$
    QRegularExpression re("^\\(.+\\)$");
    QRegularExpressionMatch match = re.match(em);
    if(!match.hasMatch())
        em = "(" + em + ")";
    printf("CuFormulaParser::m_makePreparedFormula() MADE PREPARED FORMULA %s\n", qstoc(em));
    return em;
}

QString CuFormulaParser::message() const
{
    return d->message;
}

bool CuFormulaParser::error() const
{
    return d->error;
}

QString CuFormulaParser::normalizedFormulaPattern() const
{
    return  d->normalized_pattern;
}

bool CuFormulaParser::isNormalized(const QString &expr) const
{
    QRegularExpression re(d->normalized_pattern);
    QRegularExpressionMatch match = re.match(expr);
    return match.hasMatch();
}

/**
 * @brief CuFormulaParser::isValid returns true if a formula is valid
 * @param expr the expression to be matched against the FORMULA_RE regexp
 * @return returns isNormalized
 *
 * \par Note
 * returns isNormalized
 */
bool CuFormulaParser::isValid(const QString &expr) const
{
    return isNormalized(expr);
}

