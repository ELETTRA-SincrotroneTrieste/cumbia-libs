#ifndef CUFORMULAPARSER_H
#define CUFORMULAPARSER_H

#include <QMap>
#include <QStringList>
#include <QString>
#include <vector>
#include <cuvariant.h>

//                                       FORMULA REGULAR EXPRESSION PATTERN

// regexp ^\{([\$A-Za-z0-9/,\._\-\:\s&\(\)>]+)\}[\s\n]*(function)?[\s\n]*\({0,1}([a-z,\s]+)\)[\s\n]*(.*)$
// example formula:
// {test/device/1/double_scalar test/device/1->DevDouble(10.1)} (a,b ){ return a +b; }
// example 2
// {test/device/1/double_scalar test/device/1->DevDouble(10.1)}  ( function(a, b) { return (a -sqrt(b)); })
#define FORMULA_RE "^\\{([\\$A-Za-z0-9/,\\._\\-\\:\\s&\\(\\)>]+)\\}[\\s\\n]*(function)?" \
                    "[\\s\\n]*\\({0,1}([a-z,\\s]+)\\)[\\s\\n]*(.*)$"

//                                             ------------------------
//

class CuFormulaParserPrivate;


class CuFormulaParser
{
public:
    enum State { Undefined = 0, ReadingsIncomplete, CompileOk, CompileError, ValueNotScalar,
                 ToDoubleConversionFailed, MaxState = 16 };

    const char states[MaxState][32] = {
        "ReadingsIncomplete", "CompileOk", "CompileError", "ValueNotScalar",
        "ToDoubleConversionFailed", "", "", "", "", "", "", "", "MaxState"
    };

    CuFormulaParser();

    ~CuFormulaParser();

    bool parse(const QString& expression);

    size_t sourcesCount() const;

    std::vector<std::string> sources() const;

    std::string source(size_t i) const;

    void updateSource(size_t i, const std::string& s);

    long indexOf(const std::string& src) const;

    QString expression() const;

    QString formula() const;

    QString preparedFormula() const;

    QString message() const;

    bool error() const;

    QString normalizedFormulaPattern() const;

    bool isNormalized(const QString& expr) const;

    bool isValid(const QString& expr) const;

    const char* formula_regexp_pattern = FORMULA_RE;

private:
    CuFormulaParserPrivate *d;

    QString m_makePreparedFormula() const;
};

#endif // CUFORMULAPARSER_H
