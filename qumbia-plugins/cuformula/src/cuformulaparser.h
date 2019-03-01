#ifndef CUFORMULAPARSER_H
#define CUFORMULAPARSER_H

#include <QMap>
#include <QStringList>
#include <QString>
#include <vector>
#include <cuvariant.h>

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

    QString compiledFormula() const;

    State compile(const std::vector<CuVariant> &values) const;

    QString message() const;

    bool error() const;

    QString normalizedFormulaPattern() const;

    bool isNormalized(const QString& expr) const;

private:
    CuFormulaParserPrivate *d;
};

#endif // CUFORMULAPARSER_H
