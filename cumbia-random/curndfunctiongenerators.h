#ifndef CURNDFUNCTIONGENERATORS_H
#define CURNDFUNCTIONGENERATORS_H

#include <curndfunctiongena.h>
#include <QString>
#include <QScriptValue>

class CuRndRandomFunctionGen : public CuRndFunctionGenA
{
public:
    // CuRndFunctionGenI interface
public:

    void configure(const CuData& options);

    void generate(CuData &res);

    Type getType() const;
};


class CuRndSinFunctionGen : public CuRndFunctionGenA
{
public:
    // CuRndFunctionGenI interface
public:
    void configure(const CuData& options);
    void generate(CuData &res);

    Type getType() const;

    CuData options;
};


class CuRndJsFunctionGen : public CuRndFunctionGenA
{
public:
    CuRndJsFunctionGen(const QString& filenam);

    // CuRndFunctionGenI interface
public:
    void configure(const CuData& options);
    void generate(CuData &res);

    Type getType() const;

private:
    QString m_jscode, m_error, m_filenam;
    CuData m_options;
    QScriptValue m_last_result;
    unsigned m_call_cnt;

    QScriptEngine *m_jse;
};


#endif // CURNDFUNCTIONGENERATORS_H
