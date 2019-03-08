#ifndef CUFORMULAPARSER_H
#define CUFORMULAPARSER_H

#include <QMap>
#include <QStringList>
#include <QString>
#include <vector>
#include <cuvariant.h>
#include <cuformulaplugininterface.h> // formula regexp patterns

class CuFormulaParserPrivate;


class CuFormulaParser : public CuFormulaParserI
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

    std::string joinedSources() const;

    std::string source(size_t i) const;

    void updateSource(size_t i, const std::string& s);

    long indexOf(const std::string& src) const;

    QString expression() const;

    QString formula() const;

    QString preparedFormula() const;

    QString message() const;

    QString name() const;

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
